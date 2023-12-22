typedef struct CHIP8State {
    uint16_t pc;
    uint16_t sp;
    uint8_t V[16];
    uint16_t I;
    uint8_t delay;
    uint8_t sound;
    uint8_t *memory;
    uint8_t *screen;
} CHIP8State;

CHIP8State* initCHIP8(void);
void disassembleCHIP8opcodes(uint8_t *buffer, int pc);
void unimplementedInstruction(CHIP8State *state);
void emulateCHIP8opcodes(CHIP8State *state);