#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "machine.h"

int openROM(CHIP8State *state, char *filename) {
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
    memcpy(&(state -> memory[0x200]), buffer, fsize);
    free(buffer);

    return 0;
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

void printState(CHIP8State *state) {
    //Print value in each register
    for (int i = 0; i < 16; i++) {
        printf("V%01X = %02x\n", i, state -> V[i]);
    }
    //Print whether timers are on or off
    if (state -> delay) {
        printf("DELAY = ON\n");
    }
    else {
        printf("DELAY = OFF\n");
    }
    if (state -> sound) {
        printf("SOUND = ON\n");
    }
    else {
        printf("SOUND = OFF\n");
    }
    //Print stack pointer, program counter and memory
    printf("STACK POINTER = %04x\n", state -> sp);
    printf("PROGRAM COUNTER = %04x\n", state -> pc);
    printf("MEMORY REGISTER = %04x\n", state -> I);
}