#include <stdbool.h>
#include <SDL2/SDL.h>
#include "../machine/machine.h"

//Window, CHIP-8 dimensions and frequency constants
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 640
#define SCREEN_FPS 60
#define INSTRUCTION_FREQUENCY 700

typedef struct Display {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint32_t *framebuffer;
} Display;

Display* initDisplay();
bool initSDL(Display *display);
void updateDisplay(CHIP8State *state, Display *display);
void closeSDL(Display *display);
void closeDisplay(Display *display);