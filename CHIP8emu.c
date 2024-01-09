#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "CHIP8emu.h"
#include "font4x5.h"

#define FONT_BASE 0
#define FONT_SIZE 5*16

CHIP8State* initCHIP8(void) {
    CHIP8State *s = calloc(sizeof(CHIP8State), 1);          //calloc initialises every byte to 0; second argument is block size in bytes
    
    s -> pc = 0x200;
    s -> sp = 0xfa0;
    s -> memory = calloc(4 * 1024, 1);                      //4KB = 4 * 1024 = 4096 bytes
    //s -> screen = &s -> memory[0xf00];                      //Display buffer at 0xF00
    s -> screen = calloc(64 * 32, 1);

    memcpy(&(s -> memory[FONT_BASE]), font4x5, FONT_SIZE);   //Put font in first 512 bytes of memory

    printf("Initialised CHIP8State.\n");
    return s;
}

void freeCHIP8(CHIP8State *state) {
    if (state -> screen != NULL) {
        //printf("Freeing CHIP8State screen...\n");
        free(state -> screen);
    }
    
    if (state -> memory != NULL) {
        //printf("Freeing CHIP8State memory...\n");
        free(state -> memory);
    }

    if (state != NULL) {
        //printf("Freeing CHIP8State...\n");
        free(state);
    }
}

/*
//Instructions are 2 bytes = 4 nibbles
//First hex number = first nibble (e.g. 1 in 1NNN)
//X = second nibble, used to look up one of the 16 registers (e.g X in 6XNN)
//Y = third nibble, also used to look up registers (e.g. Y in 8XY0)
//N = fourth nibble, a 4-bit number (e.g. N in DXYN)
//NN = third and fourth nibbles, an 8-bit number (e.g. NN in 6XNN)
//NNN = second, third and fourth nibbles, a 12-bit immediate memory address (e.g. NNN in 1NNN)
*/
void decodeCHIP8(uint8_t *buffer, int pc) {
    uint8_t *code = &buffer[pc];
    uint8_t firstNibble = code[0] >> 4;                     //>> shifts code[0] to the right by 4 bits
    printf("%04x %02x %02x ", pc, code[0], code[1]);        //code[0] is first instruction byte, code[1] is the second

    switch (firstNibble) {
        case 0x0:
            switch (code[1]) {
                case 0xe0: printf("%-10s", "CLS\n"); break;   //00E0: Clear the screen
                case 0xee: printf("%-10s", "RTS\n"); break;   //00EE: Return from a subroutine
                default: printf("UNKNOWN 0\n"); break;
            }
            break;

        //code[0] & 0xf is bitmasking
        //0xf = 0b1111 so 0xf is the bitmask to mask out 4 bits i.e. a nibble, in this case the first
        case 0x1: printf("%-10s $%01x%02x", "JUMP\n", code[0] & 0xf, code[1]); break;              //1NNN: Jump to address NNN
        case 0x2: printf("%-10s $%01x%02x", "CALL\n", code[0] & 0xf, code[1]); break;              //2NNN: Execute subroutine starting at address NNN
        case 0x3: printf("%-10s V%01X,#$%02x", "SKIP_EQ\n", code[0] & 0xf, code[1]); break;        //3XNN: Skip following instruction if value of VX equals NN
        case 0x4: printf("%-10s V%01X,#$%02x", "SKIP_NE\n", code[0 ]& 0xf, code[1]); break;        //4XNN: Skip following instruction if value of VX doesn't equal NN
        case 0x5: printf("%-10s V%01X,V%01X", "SKIP_EQ\n", code[0] & 0xf, code[1] >> 4); break;    //5XY0: Skip following instruction if value of VX equals value of VY
        case 0x6: printf("%-10s V%01X,#$%02x", "MVI\n", code[0] & 0xf, code[1]); break;            //6XNN: Store NN in VX
        case 0x7: printf("%-10s V%01X,#$%02x", "ADI\n", code[0] & 0xf, code[1]); break;            //7XNN: Add NN to VX
        case 0x8:
            uint8_t fourthNibble = code[1] & 0xf;
            switch (fourthNibble) {
                case 0: printf("%-10s V%01X,V%01X", "MOV\n", code[0] & 0xf, code[1] >> 4); break;   //8XY0: Store value of VY in VX
                case 1: printf("%-10s V%01X,V%01X", "OR\n", code[0] & 0xf, code[1] >> 4); break;    //8XY1: Set VX to (VX OR VY)
                case 2: printf("%-10s V%01X,V%01X", "AND\n", code[0] & 0xf, code[1] >> 4); break;   //8XY2: Set VX to (VX AND VY)
                case 3: printf("%-10s V%01X,V%01X", "XOR\n", code[0] & 0xf, code[1] >> 4); break;   //8XY3: Set VX to (VX XOR VY)

                //"." indicates instruction modifies VF

                //8XY4: Add value of VY to VX; set VF to 01 if a carry occurs, else set VF to 00
                case 4: printf("%-10s V%01X,V%01X", "ADD.\n", code[0] & 0xf, code[1] >> 4); break;

                //8XY5: Subtract value of VY from VX; set VF to 01 if a borrow occurs, else set VF to 00  
                case 5: printf("%-10s V%01X,V%01X,V%01X", "SUB.\n", code[0] & 0xf, code[0] & 0xf, code[1] >> 4); break;

                //8XY6: Store value of VY shifted right one bit in VX; set VF to least significant bit prior to shift; VY unchanged   
                case 6: printf("%-10s V%01X,V%01X", "SHR.\n", code[0] & 0xf, code[1] >> 4); break;

                //8XY7: Set VX to value of VY minus VX; set VF to 01 if a borrow occurs, else set VF to 00  
                case 7: printf("%-10s V%01X,V%01X,V%01X", "SUBB.\n", code[0] & 0xf, code[1] >> 4, code[1] >> 4); break;

                //8XYE: Store value of VY shifted left one bit in VX; set VF to most significant bit prior to shift; VY unchanged 
                case 0xe: printf("%-10s V%01X,V%01X", "SHL.\n", code[0] & 0xf, code[1] >> 4); break;

                default: printf("UNKNOWN 8\n"); break;
            }
            break;
        //9XY0: Skip following instruction if value of VX doesn't equal value of VY
        case 0x9: printf("%-10s V%01X,V%01X", "SKIP_NE\n", code[0] & 0xf, code[1] >> 4); break; 

        //ANNN: Store memory address NNN in I
        case 0xa: printf("%-10s I,#$%01x%02x", "MVI\n", code[0] & 0xf, code[1]); break;

        //BNNN: Jump to address NNN + V0 
        case 0xb: printf("%-10s $%01x%02x(V0)", "JUMP\n", code[0] & 0xf, code[1]); break;

        //CXNN: Set VX to random number with mask NN 
        case 0xc: printf("%-10s V%01X,#$%02x", "RNDMSK\n", code[0] & 0xf, code[1]); break;

        //DXYN: Draw sprite at (VX, VY) with N bytes of data starting at address stored in I  
        case 0xd: printf("%-10s V%01X,V%01X,#$%01x", "SPRITE\n", code[0] & 0xf, code[1] >> 4, code[1]&0xf); break; 

        case 0xe:
            switch (code[1]) {
                //EX9E: Skip following instruction if key corresponding to hex value stored in VX is pressed
                case 0x9e: printf("%-10s V%01X", "SKIPKEY_Y\n", code[0] & 0xf); break;

                //EXA1: Skip following instruction if key corresponding to hex value stored in VX isn't pressed  
                case 0xa1: printf("%-10s V%01X", "SKIPKEY_N\n", code[0] & 0xf); break;

                default: printf("UNKNOWN E\n"); break;
            }
            break;
        
        case 0xf:
            switch (code[1]) {
                //FX07: Store value of delay timer in VX
                case 0x07: printf("%-10s V%01X,DELAY", "MOV\n", code[0] & 0xf); break;
                
                //FX0A: Wait for keypress and store result in VX
                case 0x0a: printf("%-10s V%01X", "KEY\n", code[0] & 0xf); break;

                //FX15: Set delay timer to value of VX
                case 0x15: printf("%-10s DELAY,V%01X", "MOV\n", code[0] & 0xf); break;

                //FX18: Set sound timer to value of VX
                case 0x18: printf("%-10s SOUND,V%01X", "MOV\n", code[0] & 0xf); break;

                //FX1E: Add value of VX to I  
                case 0x1e: printf("%-10s I,V%01X", "ADI\n", code[0] & 0xf); break;

                //FX29: Set I to memory address of sprite data corresponding to hex digit stored in VX
                case 0x29: printf("%-10s I,V%01X", "SPRITECHAR\n", code[0] & 0xf); break;

                //FX33: Store binary-coded decimal equivalent of value of VX at addresses I, I + 1, and I + 2
                case 0x33: printf("%-10s (I),V%01X", "MOVBCD\n", code[0] & 0xf); break;

                //FX55: Store values of V0 to VX inclusive in memory starting at address I, then set I to I + X + 1
                case 0x55: printf("%-10s (I),V0-V%01X", "MOVM\n", code[0] & 0xf); break;

                //FX65: Fill V0 to VX inclusive with values stored in memory starting at address I, then set I to I + X + 1
                case 0x65: printf("%-10s V0-V%01X,(I)", "MOVM\n", code[0] & 0xf); break;

                default: printf("UNKNOWN F\n"); break;
            }
            break;
    }
}

void unimplementedInstruction(CHIP8State *state) {
    decodeCHIP8(state -> memory, state -> pc);      //Program counter has advanced by 2, needs to be set back
    printf("Error: Unimplemented instruction.\n");
    exit(1);
}

void op00E0(CHIP8State *state, uint8_t *code) {
    //CLS
    //Copies 0 into display (64x32) bytes; originally 1 bit per pixel but the emulated screen is 8-bit 
    memset(state -> screen, 0, 64 * 32);
    state -> displayFlag = 1;   
}

void op00EE(CHIP8State *state, uint8_t *code) {
    //RTS
    uint16_t target = (state -> memory[state -> sp] << 8) | (state -> memory[(state -> sp) + 1]);   //logical OR
    state -> sp += 2;
    state -> pc = target;
}

void op1NNN(CHIP8State *state, uint8_t *code) {
    //JUMP
    uint16_t target = ((code[0] & 0xf) << 8) | code[1];
    
    if (target == (state -> pc) - 2) {
        state -> halt = 1;
        printf("Set a halt flag as an infinite loop was detected.\n");
    }

    state -> pc = target;
}

void op2NNN(CHIP8State *state, uint8_t *code) {
    //CALL
    state -> sp -= 2;
    state -> memory[state -> sp] = ((state -> pc) & 0xFF00) >> 8;
    state -> memory[(state -> sp) + 1] = (state -> pc) & 0xFF;
    state -> pc = ((code[0] & 0xf) << 8) | code[1]; 
}

void op3XNN(CHIP8State *state, uint8_t *code) {
    //SKIP_EQ NN
    uint8_t reg = code[0] & 0xf;
    
    if (state -> V[reg] == code[1]) {
        state -> pc += 2;
    }
}

void op4XNN(CHIP8State *state, uint8_t *code) {
    //SKIP_NE NN
    uint8_t reg = code[0] & 0xf;

    if (state -> V[reg] != code[1]) {
        state -> pc += 2;
    }
}

void op5XY0(CHIP8State *state, uint8_t *code) {
    //SKIP_EQ VY
    //0xf0 = 0b11110000
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;

    if (state -> V[regX] == state -> V[regY]) {
        state -> pc +=2;
    }
}

void op6XNN(CHIP8State *state, uint8_t *code) {
    //MVI NN
    uint8_t reg = code[0] & 0xf;
    state -> V[reg] = code[1];
}

void op7XNN(CHIP8State *state, uint8_t *code) {
    //ADI
    uint8_t reg = code[0] & 0xf;
    state -> V[reg] += code[1];
}

void op8XY0(CHIP8State *state, uint8_t *code) {
    //MOV
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;
    state -> V[regX] = state -> V[regY];
}

void op8XY1(CHIP8State *state, uint8_t *code) {
    //OR
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;
    state -> V[regX] |= state -> V[regY];

    //On the original CHIP-8, the flag register is reset, so this is purely to pass the quirks test
    state -> V[0xF] = 0;
}

void op8XY2(CHIP8State *state, uint8_t *code) {
    //AND
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;
    state -> V[regX] &= state -> V[regY];

    state -> V[0xF] = 0;
}

void op8XY3(CHIP8State *state, uint8_t *code) {
    //XOR
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;
    state -> V[regX] ^= state -> V[regY];

    state -> V[0xF] = 0;
}

void op8XY4(CHIP8State *state, uint8_t *code) {
    //ADD and set VF
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;
    uint16_t result = (state -> V[regX]) + (state -> V[regY]);

    //Registers are 8-bit, not 16-bit so we need to bitmask with 0xff = 0b11111111
    state -> V[regX] = result & 0xff;

    //Has carry occured? Bitmask here is 0xff00 = 0b111111100000000
    uint16_t carry = result & 0xff00;
    if (carry) {
        state -> V[0xF] = 1;
    }
    else {
        state -> V[0xF] = 0;
    }
}

void op8XY5(CHIP8State *state, uint8_t *code) {
    //SUB and set VF
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;

    //Has borrow occured?
    uint8_t borrow = (state -> V[regY]) > (state -> V[regX]);
    state -> V[regX] -= state -> V[regY];
    if (borrow) {
        state -> V[0xF] = 0;
    }
    else {
        state -> V[0xF] = 1;
    }
}

void op8XY6(CHIP8State *state, uint8_t *code) {
    //SHR and set VF to least significant bit
    //Original CHIP-8 interpreter sets VX to VY, modern ones shift VX in place, so this is purely to pass quirk test
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;
    state -> V[regX] = state -> V[regY];

    uint8_t lsb = (state -> V[regX]) & 0x1;

    state -> V[regX] = (state -> V[regX]) >> 1;
    state -> V[0xF] = lsb;
}

void op8XY7(CHIP8State *state, uint8_t *code) {
    //SUBB and set VF
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;

    //Has borrow occured?
    uint8_t borrow = (state -> V[regX]) > (state -> V[regY]);
    state -> V[regX] = (state -> V[regY]) - (state -> V[regX]);
    if (borrow) {
        state -> V[0xF] = 0;
    }
    else {
        state -> V[0xF] = 1;
    }
}

void op8XYE(CHIP8State *state, uint8_t *code) {
    //SHL and set VF to most significant bit
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;
    state -> V[regX] = state -> V[regY];

    //0x80 = 0b10000000
    uint8_t msb = (0x80 == ((state -> V[regX]) & 0x80));
    state -> V[regX] = (state -> V[regX]) << 1;
    state -> V[0xF] = msb;
}

void op9XY0(CHIP8State *state, uint8_t *code) {
    //SKIP_NE VY
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;

    if (state -> V[regX] != state -> V[regY]) {
        state -> pc +=2;
    }
}

void opANNN(CHIP8State *state, uint8_t *code) {
    //MVI NNN
    state -> I = ((code[0] & 0xf) << 8) | code[1];
}

void opBNNN(CHIP8State *state, uint8_t *code) {
    //JUMP +V0    
    uint16_t target = ((code[0] & 0xf) << 8) | code[1];
    target += state -> V[0];
    
    if (target == (state -> pc) - 2) {
        state -> halt = 1;
        printf("Set a halt flag as an infinite loop was detected.\n");
    }

    state -> pc = target;
}

void opCXNN(CHIP8State *state, uint8_t *code) {
    //RNDMSK
    uint8_t reg = code[0] & 0xf;
    //rand() is from the stdlib
    state -> V[reg] = rand() & code[1];   
}

void opDXYN(CHIP8State *state, uint8_t *code) {
    //SPRITE
    uint8_t regX = code[0] & 0xf;
    uint8_t regY = (code[1] & 0xf0) >> 4;

    //Set X and Y coordinates to values of VX & 63 and VY & 31 respectively, and VF to 0
    //Go through N rows and draw the 8 pixels in the row, stop entirely if you reach the bottom of the screen
    uint8_t x = (state -> V[regX]) & 0x3f;
    uint8_t y = (state -> V[regY]) & 0x1f;
    state -> V[0xF] = 0;
    int rows = code[1] & 0xf;

    for (int i = 0; i < rows; i++) {
        uint8_t *sprite = &(state -> memory[(state -> I) + i]);

        //For each of the 8 pixels/bits in the row, going from left to right i.e. most to least significant bit
        //0x80 = 0b10000000 i.e. the most significant bit, so below statement will be iterated for each bit
        for (int j = 0; j < 8; j++) {
            uint8_t spritePixel = *sprite & (0x80 >> j);

            //If current 1-bit sprite pixel = on and 8-bit screen pixel at (X,Y) = on, turn off screen pixel and set VF to 1
            //Otherwise, draw if current pixel = on and pixel at (X,Y) = off
            //This is an XOR operation
            if (spritePixel) {
                uint8_t *screenPixel = &(state -> screen[(y + i) * 64 + (x + j)]);
                if (*screenPixel) {
                    state -> V[0xF] = 1;
                } 
                *screenPixel ^= 0xF;  
            }
        }
    }
    state -> displayFlag = 1;
}

void opEX9E(CHIP8State *state, uint8_t *code) {
    //SKIPKEY_Y
    uint8_t reg = code[0] & 0xf;
    uint8_t ks = state -> V[reg];
    if (state -> keyState[ks]) {
        state -> pc += 2;
    }
}

void opEXA1(CHIP8State *state, uint8_t *code) {
    //SKIPKEY_N
    uint8_t reg = code[0] & 0xf;
    uint8_t ks = state -> V[reg];
    if (!state -> keyState[ks]) {
        state -> pc += 2;
    }
}

void opFX07(CHIP8State *state, uint8_t *code) {
    //MOV VX DELAY
    uint8_t reg = code[0] & 0xf;
    state -> V[reg] = state -> delay;
}

void opFX0A(CHIP8State *state, uint8_t *code) {
    //KEY
    uint8_t reg = code[0] & 0xf;

    if (!state -> keyWait) {
        memcpy(&(state -> savedKeyState), &(state -> keyState), 16);
        state -> keyWait = 1;
    }
    else {
        //Check that a key was pressed before AND now released
        for (int i = 0; i < 16; i++) {
            if (state -> savedKeyState[i] && !(state -> keyState[i])) {
                state -> V[reg] = i;
                state -> keyWait = 0;
            }
            state -> savedKeyState[i] = state -> keyState[i];
        }
    }
    
    //Don't proceed unless a key has been pressed and released
    if (state -> keyWait) {
        state -> pc -= 2;
    }
}

void opFX15(CHIP8State *state, uint8_t *code) {
    //MOV DELAY VX
    uint8_t reg = code[0] & 0xf;
    state -> delay = state -> V[reg];
}

void opFX18(CHIP8State *state, uint8_t *code) {
    //MOV SOUND
    uint8_t reg = code[0] & 0xf;
    state -> sound = state -> V[reg];
}

void opFX1E(CHIP8State *state, uint8_t *code) {
    //ADI
    uint8_t reg = code[0] & 0xf;
    state -> I += state -> V[reg];

    //Most interpreters check for an overflow
    if (state -> I > 0xFFF) {
        state -> V[0xF] = 1;
    }
    else {
        state -> V[0xF] = 0;
    }
}

void opFX29(CHIP8State *state, uint8_t *code) {
    //SPRITECHAR
    uint8_t reg = code[0] & 0xf;
    state -> I = FONT_BASE + ((state -> V[reg]) * 5);
}

void opFX33(CHIP8State *state, uint8_t *code) {
    //MOVBCD
    //Convert value of VX to 3 decimal digits and store these in memory at addresses I, I + 1, I + 2
    uint8_t reg = code[0] & 0xf;
    uint8_t regValue = state -> V[reg];

    uint8_t oneDigit = regValue % 10;
    uint8_t tenDigit = (regValue / 10) % 10;
    uint8_t hundredDigit = (regValue / 100) % 10;

    state -> memory[state -> I] = hundredDigit;
    state -> memory[(state -> I) + 1] = tenDigit;
    state -> memory[(state -> I) + 2] = oneDigit;
}

void opFX55(CHIP8State *state, uint8_t *code) {
    //MOVM STORE I
    uint8_t reg = code[0] & 0xf;

    for (int i = 0; i <= reg; i++) {
        state -> memory[(state -> I) + i] = state -> V[i];
    }

    //Original CHIP-8 interpreter sets I to new value I + X + 1
    //Modern interpreters leave I's value alone
    state -> I += reg + 1;
}

void opFX65(CHIP8State *state, uint8_t *code) {
    //MOVM FILL V0-VF
    uint8_t reg = code[0] & 0xf;

    for (int i = 0; i <= reg; i++) {
        state -> V[i] = state -> memory[(state -> I) + i];
    }

    state -> I += reg + 1;
}

void emulateCHIP8(CHIP8State *state) {
    //Fetch and decode instruction, also it's best to increment program counter here
    uint8_t *code = &(state -> memory[state -> pc]);
    //decodeCHIP8(state -> memory, state -> pc);
    state -> pc += 2;

    uint8_t firstNibble = (*code & 0xf0) >> 4;
    switch (firstNibble) {
        case 0x00: 
            switch (code[1]) {
                case 0xe0: op00E0(state, code); break;
                case 0xee: op00EE(state, code); break;
                default: unimplementedInstruction(state); break;
            }
            break;
        case 0x01: op1NNN(state, code); break;
        case 0x02: op2NNN(state, code); break;
        case 0x03: op3XNN(state, code); break;
        case 0x04: op4XNN(state, code); break;
        case 0x05: op5XY0(state, code); break;
        case 0x06: op6XNN(state, code); break;
        case 0x07: op7XNN(state, code); break;
        case 0x08:
            uint8_t fourthNibble = code[1] & 0xf;
            switch (fourthNibble) {
                case 0: op8XY0(state, code); break;
                case 1: op8XY1(state, code); break;
                case 2: op8XY2(state, code); break;
                case 3: op8XY3(state, code); break;
                case 4: op8XY4(state, code); break;
                case 5: op8XY5(state, code); break;
                case 6: op8XY6(state, code); break;
                case 7: op8XY7(state, code); break;
                case 0xe: op8XYE(state, code); break;
                default: unimplementedInstruction(state); break;
            }
            break;
        case 0x09: op9XY0(state, code); break;
        
        case 0x0a: opANNN(state, code); break;
        case 0x0b: opBNNN(state, code); break;
        case 0x0c: opCXNN(state, code); break;
        case 0x0d: opDXYN(state, code); break;
        case 0x0e:
            switch (code[1]) {
                case 0x9e: opEX9E(state, code); break;
                case 0xa1: opEXA1(state, code); break;
                default: unimplementedInstruction(state); break;
            }
            break; 
        case 0x0f:
            switch (code[1]) {
                case 0x07: opFX07(state, code); break;
                case 0x0a: opFX0A(state, code); break;
                case 0x15: opFX15(state, code); break;
                case 0x18: opFX18(state, code); break;
                case 0x1e: opFX1E(state, code); break;
                case 0x29: opFX29(state, code); break;
                case 0x33: opFX33(state, code); break;
                case 0x55: opFX55(state, code); break;
                case 0x65: opFX65(state, code); break;
            }
            break;
    }
}