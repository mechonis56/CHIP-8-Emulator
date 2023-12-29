#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "machine/machine.h"

//Window dimension constants
const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = (int) 1000 / SCREEN_FPS;

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

void updateDisplay(CHIP8Machine *machine, uint8_t *pixelbuffer) {
    //Take the 1-bit bitmap from CHIP-8 and convert into 8-bit so that SDL can load it as a surface
    uint8_t *framebuffer = machine -> state -> memory;
    
    for (int i = 0; i < (64 / 8) * 32; i++) {
        uint8_t pixel = framebuffer[0];

        if (pixel & 0x80) pixelbuffer[0] = 0xff; else pixelbuffer[0] = 0;
        if (pixel & 0x40) pixelbuffer[1] = 0xff; else pixelbuffer[1] = 0;
        if (pixel & 0x20) pixelbuffer[2] = 0xff; else pixelbuffer[2] = 0;
        if (pixel & 0x10) pixelbuffer[3] = 0xff; else pixelbuffer[3] = 0;
        if (pixel & 0x08) pixelbuffer[4] = 0xff; else pixelbuffer[4] = 0;
        if (pixel & 0x04) pixelbuffer[5] = 0xff; else pixelbuffer[5] = 0;
        if (pixel & 0x02) pixelbuffer[6] = 0xff; else pixelbuffer[6] = 0;
        if (pixel & 0x01) pixelbuffer[7] = 0xff; else pixelbuffer[7] = 0;

        pixelbuffer += 8;
        framebuffer++;
    }
}

SDL_Texture* loadTexture(uint8_t *pixelbuffer) {
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(pixelbuffer, 64, 32, 8, 64 * 8, 0, 0, 0, 0);
    SDL_Texture *texture = NULL;

    if (surface == NULL) {
        printf("Unable to load surface. SDL Error: %s\n", SDL_GetError());
    }
    else {
        texture = SDL_CreateTextureFromSurface(gRenderer, surface);
        if (texture == NULL) {
            printf("Unable to create texture from display surface. SDL Error: %s\n", SDL_GetError());
        }
        SDL_FreeSurface(surface);
    }

    return texture;
}

void closeSDL() {
    //Deallocate loaded image
    SDL_DestroyTexture(texture);
    texture = NULL;

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

    //Create pixel array
    uint8_t *pixelbuffer = calloc(64 * 32, 1);

    uint64_t currentTick = SDL_GetPerformanceCounter();
    uint64_t lastTick = 0;
    double elapsedTicks = 0;
    double delta = 0;
    double drawTimer = 0.0;

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

            currentTick = SDL_GetPerformanceCounter();
            elapsedTicks = (double) (currentTick - lastTick);
            delta = (double) ((double) (elapsedTicks * 1000.0) / (double) SDL_GetPerformanceFrequency());
            lastTick = currentTick;
            drawTimer += delta;
            
            if (drawTimer > (double) 1000 / SCREEN_FPS) {
                drawTimer = 0.0;
                //Load pixel array into a texture then render texture to screen
                updateDisplay(machine, pixelbuffer);
                if (!loadTexture(pixelbuffer)) {
                    printf("Failed to load display.\n");
                }
                else {
                    //Clear screen, copy texture to the screen and update it
                    SDL_RenderClear(gRenderer);
                    SDL_RenderCopy(gRenderer, texture, NULL, NULL);
                    SDL_RenderPresent(gRenderer);
                }
            }
        }
    }

    //Free resources and close SDL
    free(pixelbuffer);
    freeMachine(machine);
    closeSDL();

    return 0;
}