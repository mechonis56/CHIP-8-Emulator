#include <string.h>
#include <time.h>
#include "../CHIP8emu.h"

typedef struct CHIP8Machine {
    CHIP8State *state;

    //double lastTimer;
    //double lastTick;

    //time_t *emulatorTimer;
} CHIP8Machine;

CHIP8Machine* initMachine();
void memcpyMachine(CHIP8Machine *machine, size_t start, void *src, size_t len);
void freeMachine(CHIP8Machine *machine);

int openROM(CHIP8Machine *machine, char *filename);
void executeCPU();

void keyDown(uint8_t key);
void keyUp(uint8_t key);
