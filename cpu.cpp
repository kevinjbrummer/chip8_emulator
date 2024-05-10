#include "cpu.hpp"
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <ctime>
#include <chrono>
#include <random>


uint8_t FONT_SET[80] = 
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8():randGen(std::chrono::system_clock::now().time_since_epoch().count())

{
  memory   = (uint8_t*)calloc(4096, 1);
  PC       = 0x200;
  opcode   = 0;
  I        = 0;
  SP       = 0;
  halt     = false;
  drawFlag = false;
  waitingForKey = false;
  delayTimer = 0;
  soundTimer = 0;
  randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

  memcpy(&memory[0], FONT_SET, 80);
}

bool Chip8::LoadRom(char* file)
{
  FILE* rom;
  rom = fopen(file, "rb");
  if (rom == NULL)
  {
    printf("Error opening rom file\n");
    return false;
  }

  fseek(rom, 0, SEEK_END);
  int size = ftell(rom);
  rewind(rom);

  fread(memory + 0x200, 1, size, rom);
  fclose(rom);

  return true;
}

void Chip8::EmulateCycle()
{
  uint8_t* code = &memory[PC];
  opcode = (code[0] << 8) | code[1];
  switch (opcode & 0xF000)
  {
  case 0x0000:
    {
      switch(opcode & 0x00FF)
      {
        case 0x00E0: Op00E0(); break;
        case 0x00EE: Op00EE(); break;
        default: UnimplementedOpCode(); break;
      }
    }
    break;
  case 0x1000: Op1NNN(); break;
  case 0x2000: Op2NNN(); break;
  case 0x3000: Op3XNN(); break;
  case 0x4000: Op4XNN(); break;
  case 0x5000: Op5XY0(); break;
  case 0x6000: Op6XNN(); break;
  case 0x7000: Op7XNN(); break;
  case 0x8000:
    {
      switch (opcode & 0x000F)
      {
        case 0x0000: Op8XY0(); break;
        case 0x0001: Op8XY1(); break;
        case 0x0002: Op8XY2(); break;
        case 0x0003: Op8XY3(); break;
        case 0x0004: Op8XY4(); break;
        case 0x0005: Op8XY5(); break;
        case 0x0006: Op8XY6(); break;
        case 0x0007: Op8XY7(); break;
        case 0x000E: Op8XYE(); break;
        default: UnimplementedOpCode(); break;
      }
    }
    break;
  case 0x9000: Op9XY0(); break;
  case 0xA000: OpANNN(); break;
  case 0xB000: OpBNNN(); break;
  case 0xC000: OpCXNN(); break;
  case 0xD000: OpDXYN(); break;
  case 0xE000:
    {
      switch(opcode & 0x00FF)
      {
        case 0x009E: OpEX9E(); break;
        case 0x00A1: OpEXA1(); break;
        default: UnimplementedOpCode(); break;
      }
    }
    break;
  case 0xF000:
    {
      switch (opcode & 0x00FF)
      {
        case 0x0007: OpFX07(); break;
        case 0x000A: OpFX0A(); break;
        case 0x0015: OpFX15(); break;
        case 0x0018: OpFX18(); break;
        case 0x001E: OpFX1E(); break;
        case 0x0029: OpFX29(); break;
        case 0x0033: OpFX33(); break;
        case 0x0055: OpFX55(); break;
        case 0x0065: OpFX65(); break;
        default: UnimplementedOpCode(); break;
      }
    }
    break;
  default: UnimplementedOpCode(); break;
  }

  if (delayTimer > 0)
  {
    --delayTimer;
  }

  if (soundTimer > 0)
  {
    if(soundTimer == 1)
    {
      soundFlag = true;
    }
    --soundTimer;
  }
}

void Chip8::Op00E0()
{
  memset(gfx, 0, 64 * 32);
  drawFlag = true;
  PC += 2;
}

void Chip8::Op00EE()
{
  --SP;
  PC = Stack[SP];
  PC += 2;
}

void Chip8::Op1NNN()
{
  uint16_t address = opcode & 0x0FFF;
  if (address == PC)
  {
    printf("Infinate loop detected. Ending Program");
    halt = true;
  }
  else
  {
    PC = address;
  }
}

void Chip8::Op2NNN()
{
  uint16_t address = opcode & 0x0FFF;
  Stack[SP] = PC;
  ++SP;

  PC = address;
}

void Chip8::Op3XNN()
{
  uint8_t x  = (opcode & 0x0F00) >> 8;
  if (V[x] == (opcode & 0x00FF))
  {
    PC += 2;
  }
  PC += 2;
}

void Chip8::Op4XNN()
{
  uint8_t x  = (opcode & 0x0F00) >> 8;
  if (V[x] != (opcode & 0x00FF))
  {
    PC += 2;
  }
  PC += 2;
}

void Chip8::Op5XY0()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  if (V[x] == V[y])
  {
    PC += 2;
  }
  PC += 2;
}

void Chip8::Op6XNN()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  V[x] = (opcode & 0x00FF);
  PC += 2;
}

void Chip8::Op7XNN()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  V[x] += (opcode & 0x00FF);
  PC += 2;
}

void Chip8::Op8XY0()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  V[x] = V[y];
  PC += 2;
}

void Chip8::Op8XY1()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  V[x] |= V[y];
  V[0xF] = 0;
  PC += 2;
}

void Chip8::Op8XY2()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  V[x] &= V[y];
  V[0xF] = 0;
  PC += 2;
}

void Chip8::Op8XY3()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  
  V[x] ^= V[y];
  V[0xF] = 0;
  PC += 2;
}

void Chip8::Op8XY4()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  uint16_t result = V[x] + V[y];

  V[x] = (result & 0x00FF);
  if (result & 0xFF00)
  {
    V[0xF] = 1;
  }
  else
  {
    V[0xF] = 0;
  }

  PC += 2;
}

void Chip8::Op8XY5()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  int borrow = (V[x] >= V[y]);
  V[x] -= V[y];
  V[0xF] = borrow;

  PC += 2;
}

void Chip8::Op8XY6()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  V[x] = V[y];
  uint8_t lsb = (V[x] & 0x01);
  V[x] = V[x] >>  1;
  V[0xF] = lsb;
  PC += 2;
}

void Chip8::Op8XY7()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  int borrow = (V[y] >= V[x]);
  V[x] = V[y] - V[x];
  V[0xF] = borrow;

  PC += 2;
}
void Chip8::Op8XYE()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  V[x] = V[y];
  uint8_t msb = 0x80 == (V[x] & 0x80);
  V[x] <<= 1;
  V[0xF] = msb;
  PC += 2;
}

void Chip8::Op9XY0()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  if (V[x] != V[y])
  {
    PC += 2;
  }

  PC += 2;
}

void Chip8::OpANNN()
{
  I = opcode & 0x0FFF;
  PC += 2;
}

void Chip8::OpBNNN()
{
  uint16_t address = (opcode & 0x0FFF);
  PC = V[0] + address;
}

void Chip8::OpCXNN()
{
  uint16_t x = (opcode & 0x0F00) >> 8;
  V[x] = randByte(randGen) & (opcode & 0x00FF);
  PC += 2;
}

void Chip8::OpDXYN()
{
  uint16_t x = V[(opcode & 0x0F00) >> 8] % 64;
  uint16_t y = V[(opcode & 0x00F0) >> 4] % 32;
  uint16_t height = opcode & 0x000F;
  uint16_t pixel;

  V[0xF] = 0;
  for (int yline = 0; yline < height; yline++)
  {
    pixel = memory[I + yline];
    for (int xline = 0; xline < 8; xline++)
    {
      if ((pixel & (0x80 >> xline)) != 0)
      {
        if ((x + xline) > 63)
        {
          break;
        }
        if (gfx[(x + xline + ((y + yline) * 64))] == 1)
        {
          V[0xF] = 1;
        }
        gfx[x + xline + ((y + yline) * 64)] ^= 1;
      }
    }
  }
  drawFlag = true;
  PC += 2;
}

void Chip8::OpEX9E()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  if (keys[V[x]] != 0)
  {
    PC += 2;
  }
  PC += 2;
}

void Chip8::OpEXA1()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  if (keys[V[x]] == 0)
  {
    PC += 2;
  }
  PC += 2;
}

void Chip8::OpFX07()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  V[x] = delayTimer;
  PC += 2;
}

void Chip8::OpFX0A()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  if (!waitingForKey)
  {
    waitingForKey = true;
    memcpy(savedKeys, keys, 16);
  }
  else
  {
    for (int i = 0; i < 16; i++)
    {
      if ((savedKeys[i] == 0) && keys[i] == 1)
      {
        waitingForKey = false;
        V[x] = i;
        PC += 2;
      }
    }
  }
}

void Chip8::OpFX15()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  delayTimer = V[x];
  PC += 2;
}

void Chip8::OpFX18()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  soundTimer = V[x];
  PC += 2;
}

void Chip8::OpFX1E()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  I += V[x];
  PC += 2;
}

void Chip8::OpFX29()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  I = (V[x] * 5);
  PC += 2;
}

void Chip8::OpFX33()
{
  uint8_t x = (opcode & 0x0F00) >> 8;

  memory[I] = V[x] / 100;
  memory[I + 1] = (V[x] / 10) % 10;
  memory[I + 2] = (V[x] % 100) % 10;
  PC += 2;
}

void Chip8::OpFX55()
{
  uint8_t x = (opcode & 0x0F00) >> 8;

  for (int i = 0; i <= x; i++)
  {
    memory[I + i] = V[i];
  }

  PC += 2;
}

void Chip8::OpFX65()
{
  uint8_t x = (opcode & 0x0F00) >> 8;

  for (int i = 0; i <= x; i++)
  {
    V[i] = memory[I + i];
  }

  PC += 2;
}

void Chip8::UnimplementedOpCode()
{
  printf("Unknown opcode: %04x\n", opcode);
  PC += 2;
}