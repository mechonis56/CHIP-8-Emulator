#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "machine.h"

CHIP8Machine* initMachine() {
    CHIP8State *s = initCHIP8();

    CHIP8Machine *m = calloc(sizeof(CHIP8Machine), 1);
    m -> state = s;
    m -> lastTick = 0.0;
    m -> lastTimer = 0.0;
    //m -> emulatorTimer = malloc(sizeof(time_t));

    printf("Initialised CHIP-8 machine.\n");
    return m;    
}

void memcpyMachine(CHIP8Machine *machine, size_t start, void *src, size_t len) {
    if (start + len > 4096) {
        printf("Error: Memory copy out of bounds.\n");
        return;
    }

    memcpy(&(machine -> state -> memory[start]), src, len);
}

int openROM(CHIP8Machine *machine, char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        printf("Error: Couldn't open %s\n", filename);
        return 1;
    }

    //Get file size
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    //CHIP-8 convention puts programs into memory at 0x200, with hardcoded addresses expecting this
    //Therefore programs can't be larger than the 4KB of memory minus 512 bytes 
    if (fsize > (4096 - 0x200)) {
        printf("Error: File size is greater than available memory space (4096 - 0x200 = 3584 bytes).\n");
        fclose(f);
        return 1;
    }

    //Read file into memory buffer and close it
    uint8_t *buffer = malloc(fsize);
    if (buffer == NULL) {
        printf("Error: Unable to allocate memory for file %s.\n", filename);
        fclose(f);
        return 1;
    }
    fread(buffer, fsize, 1, f);
    fclose(f);
    
    //Copy buffer into memory at 0x200, then free it as it's no longer needed
    memcpyMachine(machine, 0x200, buffer, fsize);
    free(buffer);

    return 0;
}

void freeMachine(CHIP8Machine *machine) {
    printf("Freeing CHIP8Machine...\n");
    if (machine -> state != NULL) {
        printf("Freeing CHIP8State...\n");
        freeCHIP8(machine -> state);
    }

    if (machine != NULL) {
        free(machine);
    }
}

double timeInMicroseconds() {
    clock_t time = clock();
    double microtime = (double) (time * 1000000) / CLOCKS_PER_SEC;
    return microtime;
}

void handleTimers(CHIP8Machine *machine, double currentTime) {
    //CHIP-8 updates the display at 60Hz, 1/60 = 16.667ms = 16667us
    bool tick = false;
    if ((currentTime - (machine -> lastTick)) > 16667.0) {
        tick = true;
    }
    //Timers are decremented at this frequency, as long as their values remain above 0
    if (tick && (machine -> state -> delay) > 0) {
        machine -> state -> delay -= 1;
    }
    if (tick && (machine -> state -> sound) > 0) {
        machine -> state -> sound -= 1;
    }

    machine -> lastTick = currentTime;
}

void executeCPU(CHIP8Machine *machine) {
    double currentTime = timeInMicroseconds();
    if (machine -> lastTick == 0.0) {
        machine -> lastTick = currentTime;
    }

    //If CHIP-8 is waiting, then don't do anything and return out
    if (machine -> state -> halt) {
        return;
    }

    //See if there's been a tick (i.e. has 16667us passed) and adjust machine timers accordingly
    handleTimers(machine, currentTime);

    //Can pick any number of instructions per second, went with 700 as recommended
    int instructionsPerSecond = 700;
    double elapsedTime = (currentTime - (machine -> lastTimer)) / 1000000;
    int cyclesRemaining = (int) instructionsPerSecond * elapsedTime;
    int cycles = 0;

    while (cycles < cyclesRemaining) {
        if (!(machine -> state -> halt)) {
            emulateCHIP8(machine -> state);
        }
        cycles += 1;
    }
    
    machine -> lastTimer = currentTime;
}

void keyDown(CHIP8State *state, uint8_t key) {
    switch (key) {
        case 0: state -> keyState[0] = 1; break;
        case 1: state -> keyState[1] = 1; break;
        case 2: state -> keyState[2] = 1; break;
        case 3: state -> keyState[3] = 1; break;
        case 4: state -> keyState[4] = 1; break;
        case 5: state -> keyState[5] = 1; break;
        case 6: state -> keyState[6] = 1; break;
        case 7: state -> keyState[7] = 1; break;
        case 8: state -> keyState[8] = 1; break;
        case 9: state -> keyState[9] = 1; break;

        case 0xa: state -> keyState[0xA] = 1; break;
        case 0xb: state -> keyState[0xB] = 1; break;
        case 0xc: state -> keyState[0xC] = 1; break;
        case 0xd: state -> keyState[0xD] = 1; break;
        case 0xe: state -> keyState[0xE] = 1; break;
        case 0xf: state -> keyState[0xF] = 1; break;

        default: return;
    }
}

void keyUp(CHIP8State *state, uint8_t key) {
    switch (key) {
        case 0: state -> keyState[0] = 0; break;
        case 1: state -> keyState[1] = 0; break;
        case 2: state -> keyState[2] = 0; break;
        case 3: state -> keyState[3] = 0; break;
        case 4: state -> keyState[4] = 0; break;
        case 5: state -> keyState[5] = 0; break;
        case 6: state -> keyState[6] = 0; break;
        case 7: state -> keyState[7] = 0; break;
        case 8: state -> keyState[8] = 0; break;
        case 9: state -> keyState[9] = 0; break;

        case 0xa: state -> keyState[0xA] = 0; break;
        case 0xb: state -> keyState[0xB] = 0; break;
        case 0xc: state -> keyState[0xC] = 0; break;
        case 0xd: state -> keyState[0xD] = 0; break;
        case 0xe: state -> keyState[0xE] = 0; break;
        case 0xf: state -> keyState[0xF] = 0; break;
        
        default: return;
    }
}