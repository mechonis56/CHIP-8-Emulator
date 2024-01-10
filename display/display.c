#include <stdio.h>
#include "display.h"

Display* initDisplay() {
    Display* d = calloc(sizeof(Display), 1);

    d -> window = NULL;
    d -> renderer = NULL;
    d -> texture = NULL;

    //Create a 32-bit pixel array from CHIP-8 screen to load into the texture
    //SDL_PIXELFORMAT_RGBA8888, the easiest format to understand, is 32-bit
    //Tried SDL_PIXELFORMAT_INDEX8 first but couldn't understand how to get it to work
    d -> framebuffer = calloc(SCREEN_WIDTH * SCREEN_HEIGHT, sizeof(uint32_t));
}

bool initSDL(Display *display) {
    //Initialisation flag
    bool success = true;

    //Initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialised. SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        //Create a window
        display -> window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

        if (display -> window == NULL) {
            printf("Window could not be created. SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else {
            //Create renderer for the window
            display -> renderer = SDL_CreateRenderer(display -> window, -1, SDL_RENDERER_ACCELERATED);

            if (display -> renderer == NULL) {
                printf("Renderer could not be created. SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else {
                //Upscale resolution and use nearest pixel sampling to prevent blurring of pixels
                SDL_RenderSetLogicalSize(display -> renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
                SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

                //Create a texture to display pixels
                display -> texture = SDL_CreateTexture(display -> renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

                if (display -> texture == NULL) {
                    printf("Texture could not be created. SDL Error: %s\n", SDL_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

void updateDisplay(CHIP8State *state, Display *display) {
    //Need to convert 8-bit pixels into 32-bit ARGB colour format
    //0x00FFFFFF = white, so multiply CHIP-8 pixel value (0 or 1) by this, then set opacity to max
    for (int i = 0; i < (SCREEN_WIDTH * SCREEN_HEIGHT); i++) {
        uint8_t screenPixel = state -> screen[i];
        display -> framebuffer[i] = (0x00FFFFFF * screenPixel) | 0xFF000000;
    }

    //Pitch = no. of bytes in a row of pixels
    SDL_RenderClear(display -> renderer);
    SDL_UpdateTexture(display -> texture, NULL, display -> framebuffer, SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderCopy(display -> renderer, display -> texture, NULL, NULL);
    SDL_RenderPresent(display -> renderer); 
    state -> displayFlag = 0;
}

void closeSDL(Display *display) {
    //Deallocate pixels
    if (display -> framebuffer) {
        free(display -> framebuffer);
    }
    SDL_DestroyTexture(display -> texture);

    //Destroy window
    SDL_DestroyRenderer(display -> renderer);
    SDL_DestroyWindow(display -> window);

    //Quit SDL subsystems
    SDL_Quit();
}

void closeDisplay(Display *display) {
    closeSDL(display);
    if (display != NULL) {
        free(display);
    }
}