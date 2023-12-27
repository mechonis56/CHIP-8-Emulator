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

    printf("SDL succesfully initialised.\n");
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
                if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                    
                }
            }

            //Clear screen
            SDL_RenderClear(gRenderer);

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