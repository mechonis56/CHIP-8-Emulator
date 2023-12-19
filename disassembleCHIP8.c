#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void disassembleCHIP8(uint8_t *buffer, int pc) {
    uint8_t *code = &buffer[pc];
    uint8_t firstNibble = code[0] >> 4;           //>> shifts code[0] to the right by 4 bits
    printf("%04x %02x %02x ", pc, code[0], code[1]);

    switch (firstNibble) {
        case 0x0:
            switch (code[1]) {
                case 0xe0: printf(""); break;
                case 0xee: printf(""); break;
                default: printf("UNKNOWN 0"); break;
            }
            break;
        case 0x01: printf("1 not handled yet."); break;
        case 0x02: printf("2 not handled yet."); break;
        case 0x03: printf("3 not handled yet."); break;
        case 0x04: printf("4 not handled yet."); break;
        case 0x05: printf("5 not handled yet."); break;
        case 0x06: printf("6 not handled yet."); break;
        case 0x07: printf("7 not handled yet."); break;
        case 0x08:
            uint8_t secondNibble = code[1] >> 4;
            switch (secondNibble) {
                case 0xe: printf(""); break;
                default: printf("UNKNOWN 8"); break;
            }
            break;
        case 0x09: printf("9 not handled yet."); break;

        case 0x0a: printf("a not handled yet."); break;
        case 0x0b: printf("b not handled yet."); break;
        case 0x0c: printf("c not handled yet."); break;
        case 0x0d: printf("d not handled yet."); break;
        case 0x0e:
            switch (code[1]) {
                default: printf("UNKNOWN E"); break;
            }
            break;
        case 0x0f:
            switch (code[1]) {
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