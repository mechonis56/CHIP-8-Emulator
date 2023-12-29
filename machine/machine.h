#include <string.h>
#include <time.h>
#include "../CHIP8emu.h"

typedef struct CHIP8Machine {
    CHIP8State *state;

    double lastTick;
    double lastTimer;
    //time_t *emulatorTimer;
} CHIP8Machine;

CHIP8Machine* initMachine();
void memcpyMachine(CHIP8Machine *machine, size_t start, void *src, size_t len);
int openROM(CHIP8Machine *machine, char *filename);
void freeMachine(CHIP8Machine *machine);

double timeInMicroseconds();
void handleTimers(CHIP8Machine *machine, double currentTime);
void executeCPU(CHIP8Machine *machine);

void keyDown(CHIP8State *state, uint8_t key);
void keyUp(CHIP8State *state, uint8_t key);
