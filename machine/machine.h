#include <string.h>
#include <time.h>
#include "../CHIP8emu.h"

int openROM(CHIP8State *state, char *filename);

double timeInMicroseconds();

void keyDown(CHIP8State *state, uint8_t key);
void keyUp(CHIP8State *state, uint8_t key);

void printState(CHIP8State *state);
