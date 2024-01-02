#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "machine/machine.h"

//Window dimensions and frequency constants
const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
const int SCREEN_FPS = 60;
const int INSTRUCTION_FREQUENCY = 60;

//Start up SDL and create a window
bool initSDL();

//The renderer and the window to render to
SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

//Load pixel array as a texture to be displayed
//SDL_Texture* loadTexture();
SDL_Texture *texture = NULL;

//Free resources and shut down SDL
void closeSDL();

bool initSDL() {
    //Initialisation flag
    bool success = true;

    //Initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialised. SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        //Create a window
        gWindow = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (gWindow == NULL) {
            printf("Window could not be created. SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else {
            //Create renderer for the window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL) {
                printf("Renderer could not be created. SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else {
                //Initialise renderer colour
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }

    printf("SDL successfully initialised.\n");
    return success;
}

void closeSDL() {
    //Deallocate loaded image
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }

    //Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: emulator.exe <path-to-rom>\n");
        return 0;
    }

    //Start the CHIP-8 interpreter machine and load the program
    CHIP8State *machine = initCHIP8();
    char *filename = argv[1];
    printf("Loading program %s...\n", filename);
    if (openROM(machine, filename) != 0) {
        return 1;
    }

    //Set up timers for updating the display at 60Hz
    uint64_t currentTick = SDL_GetPerformanceCounter();
    uint64_t lastTick = 0;
    double elapsedTicks = 0;
    double delta = 0.0;
    double timer = 0.0;

    //Start up SDL and create a window
    if (!initSDL()) {
        printf("Failed to initialise.\n");
    }
    else {
        //Main loop flag
        bool quit = false;

        //Event handler
        SDL_Event e;

        //While the application is running
        while (!quit) {
            //Handle events in queue
            while (SDL_PollEvent(&e) != 0) {
                //User requests quit
                if (e.type == SDL_QUIT) {
                    quit = true;
                }

                //Handle key presses and releases
                //Original CHIP-8 keypad was 123C, 456D, 789E, A0BF
                //Modern CHIP-8 emulators typically use 1234, QWER, ASDF, ZXCV to replace original keypad
                if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                        case SDLK_1: keyDown(machine, 1);
                        case SDLK_2: keyDown(machine, 2);
                        case SDLK_3: keyDown(machine, 3);
                        case SDLK_4: keyDown(machine, 0xc);

                        case SDLK_q: keyDown(machine, 4);
                        case SDLK_w: keyDown(machine, 5);
                        case SDLK_e: keyDown(machine, 6);
                        case SDLK_r: keyDown(machine, 0xd);
                        
                        case SDLK_a: keyDown(machine, 7);
                        case SDLK_s: keyDown(machine, 8);
                        case SDLK_d: keyDown(machine, 9);
                        case SDLK_f: keyDown(machine, 0xe);

                        case SDLK_z: keyDown(machine, 0xa);
                        case SDLK_x: keyDown(machine, 0);
                        case SDLK_c: keyDown(machine, 0xb);
                        case SDLK_v: keyDown(machine, 0xf);
                    }
                }
                else if (e.type == SDL_KEYUP) {
                    switch (e.key.keysym.sym) {
                        case SDLK_1: keyUp(machine, 1);
                        case SDLK_2: keyUp(machine, 2);
                        case SDLK_3: keyUp(machine, 3);
                        case SDLK_4: keyUp(machine, 0xc);

                        case SDLK_q: keyUp(machine, 4);
                        case SDLK_w: keyUp(machine, 5);
                        case SDLK_e: keyUp(machine, 6);
                        case SDLK_r: keyUp(machine, 0xd);
                        
                        case SDLK_a: keyUp(machine, 7);
                        case SDLK_s: keyUp(machine, 8);
                        case SDLK_d: keyUp(machine, 9);
                        case SDLK_f: keyUp(machine, 0xe);

                        case SDLK_z: keyUp(machine, 0xa);
                        case SDLK_x: keyUp(machine, 0);
                        case SDLK_c: keyUp(machine, 0xb);
                        case SDLK_v: keyUp(machine, 0xf);
                    }
                }
            }

            //CHIP-8 updates the display at 60Hz, 1/60 = 16.667ms = 16667us
            currentTick = SDL_GetPerformanceCounter();
            elapsedTicks = (double) (currentTick - lastTick);
            delta = (double) ((double) (elapsedTicks * 1000.0) / (double) SDL_GetPerformanceFrequency());
            lastTick = currentTick;
            timer += delta;

            if (timer > 1000 / INSTRUCTION_FREQUENCY) {
                //If CHIP-8 is waiting, then don't do anything and return out, otherwise execute an instruction
                if (!(machine -> halt)) {
                    emulateCHIP8(machine);
                }
            }

            if (timer > 1000 / SCREEN_FPS) {
                timer = 0.0;

                if (machine -> delay > 0) {
                    machine -> delay -= 1;
                }

                if (machine -> sound > 0) {
                    machine -> sound -= 1;
                }  

                //Update pixel array and load it into the texture
                //Only update the pixel array if display flag is set to 1
                if (machine -> displayFlag) {
                    printState(machine);
                    machine -> displayFlag = 0;
                }
            } 

            //Clear the screen, (update the texture), then update the screen
            SDL_RenderClear(gRenderer);
            SDL_RenderCopy(gRenderer, texture, NULL, NULL);
            SDL_RenderPresent(gRenderer);
        }
    }

    //Free resources and close SDL
    freeCHIP8(machine);
    closeSDL();

    return 0;
}