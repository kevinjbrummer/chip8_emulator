#include <stdint.h>
#include <cstdlib>

class Chip8
{
  public:
    uint8_t V[16]{};
    uint8_t* memory;
    uint16_t opcode;
    uint16_t I;
    uint16_t PC;
    uint8_t gfx[64 * 32]{};
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint16_t Stack[16]{};
    uint16_t SP;
    uint8_t keys[16]{};
    bool halt;
    bool drawFlag;

    Chip8();
    bool LoadRom(char* file);
    void EmulateCycle();

    void Op00E0();
    void Op00EE();
    void Op1NNN();
    void Op2NNN();
    void Op3XNN();
    void Op4XNN();
    void Op5XY0();
    void Op6XNN();
    void Op7XNN();
    void Op8XY0();
    void Op8XY1();
    void Op8XY2();
    void Op8XY3();
    void Op8XY4();
    void Op8XY5();
    void Op8XY6();
    void Op8XY7();
    void Op8XYE();
    void Op9XY0();
    void OpANNN();
    void OpBNNN();
    void OpDXYN();
    void OpEX9E();
    void OpEXA1();
    void OpFX07();
    void OpFX0A();
    void OpFX15();
    void OpFX18();
    void OpFX1E();
    void OpFX29();
    void OpFX33();
    void OpFX55();
    void OpFX65();

    //For debugging
    void UnimplementedOpCode();
    void Draw();
};