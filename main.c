#include <stdio.h>
#include "display/display.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: emulator.exe <path-to-rom>\n");
        return 0;
    }

    //Start the CHIP-8 interpreter machine and load the program
    CHIP8State *machine = initCHIP8();
    char *filename = argv[1];
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
    Display *display = initDisplay();

    if (!initSDL(display)) {
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
                        case SDLK_1: keyDown(machine, 1); break;
                        case SDLK_2: keyDown(machine, 2); break;
                        case SDLK_3: keyDown(machine, 3); break;
                        case SDLK_4: keyDown(machine, 0xc); break;

                        case SDLK_q: keyDown(machine, 4); break;
                        case SDLK_w: keyDown(machine, 5); break;
                        case SDLK_e: keyDown(machine, 6); break;
                        case SDLK_r: keyDown(machine, 0xd); break;
                        
                        case SDLK_a: keyDown(machine, 7); break;
                        case SDLK_s: keyDown(machine, 8); break;
                        case SDLK_d: keyDown(machine, 9); break;
                        case SDLK_f: keyDown(machine, 0xe); break;

                        case SDLK_z: keyDown(machine, 0xa); break;
                        case SDLK_x: keyDown(machine, 0); break;
                        case SDLK_c: keyDown(machine, 0xb); break;
                        case SDLK_v: keyDown(machine, 0xf); break;
                        case SDLK_ESCAPE: machine -> halt = 1; quit = true; break;

                        default: break;
                    }
                }
                else if (e.type == SDL_KEYUP) {
                    switch (e.key.keysym.sym) {
                        case SDLK_1: keyUp(machine, 1); break;
                        case SDLK_2: keyUp(machine, 2); break;
                        case SDLK_3: keyUp(machine, 3); break;
                        case SDLK_4: keyUp(machine, 0xc); break;

                        case SDLK_q: keyUp(machine, 4); break;
                        case SDLK_w: keyUp(machine, 5); break;
                        case SDLK_e: keyUp(machine, 6); break;
                        case SDLK_r: keyUp(machine, 0xd); break;
                        
                        case SDLK_a: keyUp(machine, 7); break;
                        case SDLK_s: keyUp(machine, 8); break;
                        case SDLK_d: keyUp(machine, 9); break;
                        case SDLK_f: keyUp(machine, 0xe); break;

                        case SDLK_z: keyUp(machine, 0xa); break;
                        case SDLK_x: keyUp(machine, 0); break;
                        case SDLK_c: keyUp(machine, 0xb); break;
                        case SDLK_v: keyUp(machine, 0xf); break;
                        default: break;
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

                if (!machine -> halt) {
                    if (machine -> delay > 0) {
                        machine -> delay -= 1;
                    }

                    if (machine -> sound > 0) {
                        machine -> sound -= 1;
                    } 
                }

                //Update pixel array and load it into the texture, but only if the display flag is on
                if (machine -> displayFlag) {
                    updateDisplay(machine, display);
                }
            }
            //Sleep for the rest of the frame to reduce CPU usage
            else {
                SDL_Delay((1000 / SCREEN_FPS) - timer);
            }
        }
    }

    //Free resources and close SDL
    freeCHIP8(machine);
    closeDisplay(display);

    return 0;
}