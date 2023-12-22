#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void disassembleCHIP8(uint8_t *buffer, int pc) {
    uint8_t *code = &buffer[pc];
    uint8_t firstNibble = code[0] >> 4;                     //>> shifts code[0] to the right by 4 bits
    printf("%04x %02x %02x ", pc, code[0], code[1]);

    switch (firstNibble) {
        case 0x0:
            switch (code[1]) {
                case 0xe0: printf("%-10s", "CLS"); break;   //00E0: Clear the screen
                case 0xee: printf("%-10s", "RTS"); break;   //00EE: Return from a subroutine
                default: printf("UNKNOWN 0"); break;
            }
            break;
        case 0x1: printf("%-10s $%01x%02x", "JUMP", code[0] & 0xf, code[1]); break;              //1NNN: Jump to address NNN
        case 0x2: printf("%-10s $%01x%02x", "CALL", code[0] & 0xf, code[1]); break;              //2NNN: Execute subroutine starting at address NNN
        case 0x3: printf("%-10s V%01X,#$%02x", "SKIP_EQ", code[0] & 0xf, code[1]); break;        //3XNN: Skip following instruction if value of VX equals NN
        case 0x4: printf("%-10s V%01X,#$%02x", "SKIP_NE", code[0 ]& 0xf, code[1]); break;        //4XNN: Skip following instruction if value of VX doesn't equal NN
        case 0x5: printf("%-10s V%01X,V%01X", "SKIP_EQ", code[0] & 0xf, code[1] >> 4); break;    //5XY0: Skip following instruction if value of VX equals value of VY
        case 0x6: printf("%-10s V%01X,#$%02x", "MVI", code[0] & 0xf, code[1]); break;            //6XNN: Store NN in VX
        case 0x7: printf("%-10s V%01X,#$%02x", "ADI", code[0] & 0xf, code[1]); break;            //7XNN: Add NN to VX
        case 0x8:
            uint8_t secondNibble = code[1] >> 4;
            switch (secondNibble) {
                case 0: printf("%-10s V%01X,V%01X", "MOV", code[0] & 0xf, code[1] >> 4); break;   //8XY0: Store value of VY in VX
                case 1: printf("%-10s V%01X,V%01X", "OR", code[0] & 0xf, code[1] >> 4); break;    //8XY1: Set VX to (VX OR VY)
                case 2: printf("%-10s V%01X,V%01X", "AND", code[0] & 0xf, code[1] >> 4); break;   //8XY2: Set VX to (VX AND VY)
                case 3: printf("%-10s V%01X,V%01X", "XOR", code[0] & 0xf, code[1] >> 4); break;   //8XY3: Set VX to (VX XOR VY)

                //"." indicates instruction modifies VF

                //8XY4: Add value of VY to VX; set VF to 01 if a carry occurs, else set VF to 00
                case 4: printf("%-10s V%01X,V%01X", "ADD.", code[0] & 0xf, code[1] >> 4); break;

                //8XY5: Subtract value of VY from VX; set VF to 01 if a borrow occurs, else set VF to 00  
                case 5: printf("%-10s V%01X,V%01X,V%01X", "SUB.", code[0] & 0xf, code[0] & 0xf, code[1] >> 4); break;

                //8XY6: Store value of VY shifted right one bit in VX; set VF to least significant bit prior to shift; VY unchanged   
                case 6: printf("%-10s V%01X,V%01X", "SHR.", code[0] & 0xf, code[1] >> 4); break;

                //8XY7: Set VX to value of VY minus VX; set VF to 01 if a borrow occurs, else set VF to 00  
                case 7: printf("%-10s V%01X,V%01X,V%01X", "SUBB.", code[0] & 0xf, code[1] >> 4, code[1] >> 4); break;

                //8XYE: Store value of VY shifted left one bit in VX; set VF to most significant bit prior to shift; VY unchanged 
                case 0xe: printf("%-10s V%01X,V%01X", "SHL.", code[0] & 0xf, code[1] >> 4); break;

                default: printf("UNKNOWN 8"); break;
            }
            break;
        //9XY0: Skip following instruction if value of VX doesn't equal value of VY
        case 0x9: printf("%-10s V%01X,V%01X", "SKIP_NE", code[0] & 0xf, code[1] >> 4); break; 

        //ANNN: Store memory address NNN in I
        case 0xa: printf("%-10s I,#$%01x%02x", "MVI", code[0] & 0xf, code[1]); break;

        //BNNN: Jump to address NNN + V0 
        case 0xb: printf("%-10s $%01x%02x(V0)", "JUMP", code[0] & 0xf, code[1]); break;

        //CXNN: Set VX to random number with mask NN 
        case 0xc: printf("%-10s V%01X,#$%02x", "RNDMSK", code[0] & 0xf, code[1]); break;

        //DXYN: Draw sprite at (VX, VY) with N bytes of data starting at address stored in I  
        case 0xd: printf("%-10s V%01X,V%01X,#$%01x", "SPRITE", code[0] & 0xf, code[1] >> 4, code[1]&0xf); break; 

        case 0xe:
            switch (code[1]) {
                //EX9E: Skip following instruction if key corresponding to hex value stored in VX is pressed
                case 0x9e: printf("%-10s V%01X", "SKIPKEY_Y", code[0] & 0xf); break;

                //EXA1: Skip following instruction if key corresponding to hex value stored in VX isn't pressed  
                case 0xa1: printf("%-10s V%01X", "SKIPKEY_N", code[0] & 0xf); break;

                default: printf("UNKNOWN E"); break;
            }
            break;
        
        case 0xf:
            switch (code[1]) {
                //FX07: Store value of delay timer in VX
                case 0x07: printf("%-10s V%01X,DELAY", "MOV", code[0] & 0xf); break;
                
                //FX0A: Wait for keypress and store result in VX
                case 0x0a: printf("%-10s V%01X", "KEY", code[0] & 0xf); break;

                //FX15: Set delay timer to value of VX
                case 0x15: printf("%-10s DELAY,V%01X", "MOV", code[0] & 0xf); break;

                //FX18: Set sound timer to value of VX
                case 0x18: printf("%-10s SOUND,V%01X", "MOV", code[0] & 0xf); break;

                //FX1E: Add value of VX to I  
                case 0x1e: printf("%-10s I,V%01X", "ADI", code[0] & 0xf); break;

                //FX29: Set I to memory address of sprite data corresponding to hex digit stored in VX
                case 0x29: printf("%-10s I,V%01X", "SPRITECHAR", code[0] & 0xf); break;

                //FX33: Store binary-coded decimal equivalent of value of VX at addresses I, I + 1, and I + 2
                case 0x33: printf("%-10s (I),V%01X", "MOVBCD", code[0] & 0xf); break;

                //FX55: Store values of V0 to VX inclusive in memory starting at address I, then set I to I + X + 1
                case 0x55: printf("%-10s (I),V0-V%01X", "MOVM", code[0] & 0xf); break;

                //FX65: Fill V0 to VX inclusive with values stored in memory starting at address I, then set I to I + X + 1
                case 0x65: printf("%-10s V0-V%01X,(I)", "MOVM", code[0] & 0xf); break;

                default: printf("UNKNOWN F"); break;
            }
            break;
    }

}

int main(int argc, char **argv) {
        FILE *f = fopen(argv[1], "rb");
        if (f == NULL) {
            printf("Error: Couldn't open %s\n", argv[1]);
            exit(1);
        }

        //Get file size
        fseek(f, 0L, SEEK_END);
        int fsize = ftell(f);
        fseek(f, 0L, SEEK_SET);

        //CHIP-8 convention puts programs into memory at 0x200, with hardcoded addresses expecting this
        //Read file into memory buffer at 0x200 and close it
        unsigned char *buffer = malloc(fsize + 0x200);
        fread(buffer + 0x200, fsize, 1, f);
        fclose(f);

        int pc = 0x200;
        while (pc < (fsize + 0x200)) {
            disassembleCHIP8(buffer, pc);
            pc += 2;
            printf("\n");
        }

        return 0;
}