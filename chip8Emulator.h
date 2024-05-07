#include <stdint.h>

class Chip8Emulator
{
  public:
    uint8_t V[16]; //registers
    uint16_t I; //memory address
    uint16_t SP; //stack pointer
    uint16_t PC; //program counter
    uint8_t delay; //delay timer;
    uint8_t sound; //sound timer;
    uint8_t *memory;
    uint8_t *screen; //memory[0xf00];
    uint8_t keyState[16];
    uint8_t saveKeyState[16];
    int waitingForKey;
    int halt;

    Chip8Emulator();
    void EmulateChip8Op();
    void LoadRom(char* file);
    void DisassembleChip8Op();
    void UnimplementedInstruction();
    void Op0(uint8_t* code);
    void Op1(uint8_t* code);
    void Op2(uint8_t* code);
    void Op3(uint8_t* code);
    void Op4(uint8_t* code);
    void Op5(uint8_t* code);
    void Op6(uint8_t* code);
    void Op7(uint8_t* code);
    void Op8(uint8_t* code);
    void Op9(uint8_t* code);
    void OpA(uint8_t* code);
    void OpB(uint8_t* code);
    void OpC(uint8_t* code);
    void OpD(uint8_t* code);
    void OpE(uint8_t* code);
    void OpF(uint8_t* code);
};