#include <stdint.h>

typedef struct CHIP8State {
    uint16_t pc;
    uint16_t sp;
    uint8_t V[16];
    uint16_t I;
    uint8_t delay;
    uint8_t sound;
    uint8_t *memory;
    uint8_t *screen;
    uint8_t halt;
    uint8_t keyState[16];
    uint8_t keyWait;
    uint8_t savedKeyState[16];
} CHIP8State;

CHIP8State* initCHIP8(void);
void decodeCHIP8(uint8_t *buffer, int pc);
void unimplementedInstruction(CHIP8State *state);
void emulateCHIP8(CHIP8State *state);

void op00E0(CHIP8State *state, uint8_t *code);
void op00EE(CHIP8State *state, uint8_t *code);
void op1NNN(CHIP8State *state, uint8_t *code);
void op2NNN(CHIP8State *state, uint8_t *code);
void op3XNN(CHIP8State *state, uint8_t *code);
void op4XNN(CHIP8State *state, uint8_t *code);
void op5XY0(CHIP8State *state, uint8_t *code);
void op6XNN(CHIP8State *state, uint8_t *code);
void op7XNN(CHIP8State *state, uint8_t *code);
void op8XY0(CHIP8State *state, uint8_t *code);
void op8XY1(CHIP8State *state, uint8_t *code);
void op8XY2(CHIP8State *state, uint8_t *code);
void op8XY3(CHIP8State *state, uint8_t *code);
void op8XY4(CHIP8State *state, uint8_t *code);
void op8XY5(CHIP8State *state, uint8_t *code);
void op8XY6(CHIP8State *state, uint8_t *code);
void op8XY7(CHIP8State *state, uint8_t *code);
void op8XYE(CHIP8State *state, uint8_t *code);
void op9XY0(CHIP8State *state, uint8_t *code);
void opANNN(CHIP8State *state, uint8_t *code);
void opBNNN(CHIP8State *state, uint8_t *code);
void opCXNN(CHIP8State *state, uint8_t *code);
void opDXYN(CHIP8State *state, uint8_t *code);
void opEX9E(CHIP8State *state, uint8_t *code);
void opEXA1(CHIP8State *state, uint8_t *code);
void opFX07(CHIP8State *state, uint8_t *code);
void opFX0A(CHIP8State *state, uint8_t *code);
void opFX15(CHIP8State *state, uint8_t *code);
void opFX18(CHIP8State *state, uint8_t *code);
void opFX1E(CHIP8State *state, uint8_t *code);
void opFX29(CHIP8State *state, uint8_t *code);
void opFX33(CHIP8State *state, uint8_t *code);
void opFX55(CHIP8State *state, uint8_t *code);
void opFX65(CHIP8State *state, uint8_t *code);
