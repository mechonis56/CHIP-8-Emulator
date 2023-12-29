#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "machine/machine.h"

//Window dimension constants
const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;

//Start up SDL and create a window
bool initSDL();

//The renderer and the window to render to
SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

//Load pixel array as a texture to be displayed
//SDL_Texture* loadTexture();
//SDL_Texture *texture = NULL;

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
    //SDL_DestroyTexture(texture);
    //texture = NULL;

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
    CHIP8Machine *machine = initMachine();
    char *filename = argv[1];
    printf("Loading program %s...\n", filename);
    if (openROM(machine, filename) != 0) {
        return 1;
    }

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
                        case SDLK_1: keyDown(machine -> state, 1);
                        case SDLK_2: keyDown(machine -> state, 2);
                        case SDLK_3: keyDown(machine -> state, 3);
                        case SDLK_4: keyDown(machine -> state, 0xc);

                        case SDLK_q: keyDown(machine -> state, 4);
                        case SDLK_w: keyDown(machine -> state, 5);
                        case SDLK_e: keyDown(machine -> state, 6);
                        case SDLK_r: keyDown(machine -> state, 0xd);
                        
                        case SDLK_a: keyDown(machine -> state, 7);
                        case SDLK_s: keyDown(machine -> state, 8);
                        case SDLK_d: keyDown(machine -> state, 9);
                        case SDLK_f: keyDown(machine -> state, 0xe);

                        case SDLK_z: keyDown(machine -> state, 0xa);
                        case SDLK_x: keyDown(machine -> state, 0);
                        case SDLK_c: keyDown(machine -> state, 0xb);
                        case SDLK_v: keyDown(machine -> state, 0xf);
                    }
                }
                else if (e.type == SDL_KEYUP) {
                    switch (e.key.keysym.sym) {
                        case SDLK_1: keyUp(machine -> state, 1);
                        case SDLK_2: keyUp(machine -> state, 2);
                        case SDLK_3: keyUp(machine -> state, 3);
                        case SDLK_4: keyUp(machine -> state, 0xc);

                        case SDLK_q: keyUp(machine -> state, 4);
                        case SDLK_w: keyUp(machine -> state, 5);
                        case SDLK_e: keyUp(machine -> state, 6);
                        case SDLK_r: keyUp(machine -> state, 0xd);
                        
                        case SDLK_a: keyUp(machine -> state, 7);
                        case SDLK_s: keyUp(machine -> state, 8);
                        case SDLK_d: keyUp(machine -> state, 9);
                        case SDLK_f: keyUp(machine -> state, 0xe);

                        case SDLK_z: keyUp(machine -> state, 0xa);
                        case SDLK_x: keyUp(machine -> state, 0);
                        case SDLK_c: keyUp(machine -> state, 0xb);
                        case SDLK_v: keyUp(machine -> state, 0xf);
                    }
                }
            }

            //Execute CHIP-8 instructions
            //CHIP-8 updates the display at 60Hz, so need to time that here as well
            executeCPU(machine);

            //Clear screen
            SDL_RenderClear(gRenderer);

            //TODO: Need to pass the pixels into a surface/texture and render here
            //Render texture to screen
            //SDL_RenderCopy(gRenderer, texture, NULL, NULL);

            //Update screen
            SDL_RenderPresent(gRenderer);
        }
    }

    //Free resources and close SDL
    freeMachine(machine);
    closeSDL();

    return 0;
}