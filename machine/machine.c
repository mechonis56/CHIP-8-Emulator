#include <stdio.h>
#include <stdlib.h>
#include "machine.h"

CHIP8Machine* initMachine() {
    CHIP8State *s = initCHIP8();

    CHIP8Machine *m = calloc(sizeof(CHIP8Machine), 1);
    m -> state = s;
    //m -> lastTimer = 0.0;
    //m -> lastTick = 0.0;
    //m -> emulatorTimer = malloc(sizeof(time_t));

    printf("Initialised CHIP-8 machine.\n");
    return m;    
}

void memcpyMachine(CHIP8Machine *machine, size_t start, void *src, size_t len) {
    if (start + len > 4096) {
        printf("Error: Memory copy out of bounds.");
        return;
    }

    memcpy(&(machine -> state -> memory[start]), src, len);
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