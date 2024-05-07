#include "chip8Emulator.h"
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <fstream>
#include <cstdlib>
#include <memory.h>
#include <stdlib.h>

Chip8Emulator::Chip8Emulator()
{
  memory = (uint8_t *)calloc(1024 * 4, 1); //Init memory with 4K
  screen = &memory[0xf00];
  SP = 0xfa0;
  PC = 0x200;
  waitingForKey = 0;
  halt = 0;
}

void Chip8Emulator::DisassembleChip8Op()
{
  uint8_t *op = &memory[PC];
  uint8_t firstnib = (op[0] >> 4);
  printf("%04x %02x %02x ", PC, op[0], op[1]);

  switch (firstnib)
  {
    case 0x00:
      {
        switch (op[1])
        {
          case 0xe0:
            printf("CLS");
            break;
          case 0xee:
            printf("RTN");
            break;
          default:
            printf("UNKOWN 0");
            break;
        }
      }
      break;
    case 0x01:
      {
        uint8_t x = (op[0] & 0x0f);
        printf("%-10s $%01x%02x", "JUMP", x, op[1]);
      }
      break;
    case 0x02:
      {
        uint8_t x = (op[0] & 0x0f);
        printf("%-10s $%01x%02x", "CALL", x, op[1]);
      }
      break;
    case 0x03:
      {
        uint8_t x = (op[0] & 0x0f);
        printf("%-10s V%01x, #$%02x", "SKIP.EQ", x, op[1]);
      }
      break;
    case 0x04:
      {
        uint8_t x = (op[0] & 0x0f);
        printf("%-10s V%01x, #$%02x", "SKIP.NE", x, op[1]);
      }
      break;
    case 0x05:
      printf("%-10s V%01x, V%01x", "SKIP.EQ", (op[0] & 0x0f), (op[1] >> 1));
      break;
    case 0x06:
      {
        uint8_t x = (op[0] & 0x0f);
        printf("%-10s V%01x,#$%02x", "M0V", x, op[1]);
      }
      break;
    case 0x07:
      {
        uint8_t x = (op[0] & 0x0f);
        printf("%-10s V%01x,#$%02x", "ADI", x, op[1]);
      }
      break;
    case 0x08:
      {
        switch (op[1] & 0x0f)
        {
        case 0x00:
          printf("%-10s V%01x, V%01x", "MOV", (op[0] & 0x0f), (op[1] >> 1));
          break;
        case 0x01:
          printf("%-10s V%01x, V%01x", "OR", (op[0] & 0x0f), (op[1] >> 1));
          break;
        case 0x02:
          printf("%-10s V%01x, V%01x", "AND", (op[0] & 0x0f), (op[1] >> 1));
          break;
        case 0x03:
          printf("%-10s V%01x, V%01x", "XOR", (op[0] & 0x0f), (op[1] >> 1));
          break;
        case 0x04:
          printf("%-10x V%01x, V%01x", "ADD.", (op[0] & 0x0f), (op[1] >> 1));
          break;
        case 0x05:
          printf("%-10x V%01x, V%01x", "SUB.", (op[0] & 0x0f), (op[1] >> 1));
          break;
        case 0x06:
          printf("%-10x V%01x", "SHR.", (op[0] & 0x0f));
          break;
        case 0x07:
          printf("%-10x V%01x, V%01x", "SUBB.", (op[0] & 0x0f), (op[1] >> 1));
          break;
        case 0x0e:
          printf("%-10x V%01x", "SHL.", (op[0] & 0x0f));
          break;
        default:
          printf("UNKOWN 8");
          break;
        }
      }
      break;
    case 0x09:
      printf("%-10s V%01x, V%01x", "SKIP.NE", (op[0] & 0x0f), (op[1] >> 1));
      break;
    case 0x0a:
      {
        uint8_t addresshi = op[0] & 0x0f;
        printf("%-10s I, #$%01x%02x", "MVI", addresshi, op[1]);
      }
      break;
    case 0x0b:
      printf("%-10s $%01x%02x(V0)", "MVI", (op[0] & 0x0f), op[1]);
      break;
    case 0x0c:
      printf("%-10s V%01x, #$%02x", "RAND", (op[0] & 0x0f), op[1]);
      break;
    case 0x0d:
      printf("%-10s V%01x, V%01x, #$%01x", "SPRITE", (op[0] & 0x0f), (op[1] >> 4), (op[1] & 0x0f));
      break;
    case 0x0e:
      {
        switch(op[1])
        {
          case 0x9e:
            printf("%-10s V%01x", "SKIP.KEY", (op[0] & 0x0f));
            break;
          case 0xa1:
            printf("%-10s V%01x", "SKIP.NOKEY", (op[0] & 0x0f));
            break;
          default:
            printf("UNKNOWN E");
            break;
        }
      }
      break;
    case 0x0f:
      {
        switch(op[1])
        {
          case 0x07:
            printf("%-10s V%01x, DELAY", "MOV", (op[0] & 0x0f));
            break;
          case 0x0a:
            printf("%-10s V%01x", "WAIT.KEY", (op[0] & 0x0f));
            break;
          case 0x15:
            printf("%-10s DELAY, V%01x", "MOV", (op[0] & 0x0f));
            break;
          case 0x18:
            printf("%-10s SOUND, V%01x", "MOV", (op[0] & 0x0f));
            break;
          case 0x1e:
            printf("%-10s I, V%01x", "ADD", (op[0] & 0x0f));
            break;
          case 0x29:
            printf("%-10s V%01x", "SPRITECHAR", (op[0] & 0x0f));
            break;
          case 0x33:
            printf("%-10s V%01x", "MOVBCD", (op[0] & 0x0f));
            break;
          case 0x55:
            printf("%-10s (I), V0~V%01x", "MOVM", (op[0] & 0x0f));
            break;
          case 0x65:
            printf("%-10s V0~V%01x, (I)", "MOVM", (op[0] & 0x0f));
            break;
          default:
            printf("UNKNOWN F");
            break;
        }
      }
      break;
    default:
      break;
  }
}

void Chip8Emulator::UnimplementedInstruction()
{
  printf("Error: UnimplementedInstruction\n");
  DisassembleChip8Op();
  halt = 1;
}

void Chip8Emulator::EmulateChip8Op()
{
  uint8_t *op = &memory[PC];
  DisassembleChip8Op();
  printf("\n");
  switch (op[0] >> 4)
  {
  case 0x00: Op0(op); break;
  case 0x01: Op1(op); break;
  case 0x02: Op2(op); break;
  case 0x03: Op3(op); break;
  case 0x04: Op4(op); break;
  case 0x05: Op5(op); break;
  case 0x06: Op6(op); break;
  case 0x07: Op7(op); break;
  case 0x08: Op8(op); break;
  case 0x09: Op9(op); break;
  case 0x0a: OpA(op); break;
  case 0x0b: OpB(op); break;
  case 0x0c: OpC(op); break;
  case 0x0d: UnimplementedInstruction(); break;
  case 0x0e: OpE(op); break;
  case 0x0f: OpF(op); break;
  default: UnimplementedInstruction(); break;
  }
}

void Chip8Emulator::Op0(uint8_t *code)
{
  switch (code[1])
    {
    case 0xe0:
      memset(screen, 0, 64*32/8);
      PC += 2;
      break;
    case 0xee:
      {
        uint16_t target = (memory[SP] << 8 | memory[SP+ 1]);
        SP += 2;
        PC = target;
        break;
      }
    default:
      UnimplementedInstruction();
      break;
    }
}

void Chip8Emulator::Op1(uint8_t *code)
{
  uint16_t target = (code[0] << 8 | code[1]);
  if (PC == target)
  {
    printf("Infinite loop detected. Stopping program.\n");
    halt = 1;
    return;
  }
  PC = target;
}

void Chip8Emulator::Op2(uint8_t *code)
{
  SP -= 2;
  memory[SP] = ((PC + 2) & 0xff00) >> 8;
  memory[SP + 1] = (PC+2) & 0xff;
  PC = ((code[0] & 0x0f) << 8) | code[1];
}

void Chip8Emulator::Op3(uint8_t *code)
{
  uint8_t x = code[0] & 0x0f;
  if (V[x] == code[1])
  {
    PC += 2;
  }
  PC += 2;
}

void Chip8Emulator::Op4(uint8_t *code)
{
  uint8_t x = code[0] & 0x0f;
  if (V[x] != code[1])
  {
    PC += 2;
  }
  PC += 2;
}

void Chip8Emulator::Op5(uint8_t *code)
{
  uint8_t x = code[0] & 0x0f;
  uint8_t y = code[1] >> 4;
  if (V[x] == V[y])
  {
    PC += 2;
  }
  PC += 2;
}

void Chip8Emulator::Op6(uint8_t *code)
{
  uint8_t x = code[0] & 0x0f;
  V[x] = code[1];
  PC += 2;
}

void Chip8Emulator::Op7(uint8_t *code)
{
  uint8_t x = code[0] & 0x0f;
  V[x] += code[1];
  PC += 2;
}

void Chip8Emulator::Op8(uint8_t *code)
{
  uint8_t x = code[0] & 0x0f;
  uint8_t y = code[1] >> 4;
  switch(code[1] & 0x0f)
  {
    case 0x00: V[x] = V[y]; break;
    case 0x01: V[x] |= V[y]; break;
    case 0x02: V[x] &= V[y]; break;
    case 0x03: V[x] ^= V[y]; break;
    case 0x04:
      {
        uint16_t result = V[x] + V[y];
        V[0x0f] = (result & 0xff00);
        V[x] = result & 0xff;
      }
      break;
    case 0x05:
      V[0x0f] = (V[x] >= V[y]);
      V[x] -= V[y];
      break;
    case 0x06:
      V[0x0f] = V[x] & 0x1;
      V[x] = V[x] >> 1;
      break;
    case 0x07:
      V[0x0f] = (V[y] >= V[x]);
      V[x] = V[y] - V[x];
      break;
    case 0x0e:
      V[0x0f] = (0x80 == (V[x] & 0x80));
      V[x] = V[x] << 1;
      break;
    default: UnimplementedInstruction(); break;
  }
  PC += 2;
}

void Chip8Emulator::Op9(uint8_t *code)
{
  uint8_t x = code[0] & 0x0f;
  uint8_t y = code[1] >> 4;
  if (V[x] != V[y])
  {
    PC += 2;
  }
  PC += 2;
}

void Chip8Emulator::OpA(uint8_t *code)
{
  uint16_t value = ((code[0] & 0x0f) << 8) | code[1];
  I = value;
  PC += 2;
}

void Chip8Emulator::OpB(uint8_t *code)
{
  uint16_t value = ((code[0] & 0x0f) << 8) | code[1];
  PC = V[0] + value;
}

void Chip8Emulator::OpC(uint8_t *code)
{
  uint8_t x = code[0] & 0x0f;
  int rand = std::rand() % 256;
  PC = V[x] = rand & code[1];
}

void Chip8Emulator::OpE(uint8_t *code)
{
  uint8_t x = code[0] & 0x0f;
  switch (code[1])
  {
    case 0x9e:
      if (keyState[V[x]] != 0)
      {
        PC += 2;
      }
      break;
    case 0xa1:
      if (keyState[V[x]] == 0)
      {
        PC += 2;
      }
      break;
    default: UnimplementedInstruction(); break;
  }
  PC += 2;
}

void Chip8Emulator::OpF(uint8_t *code)
{
  uint8_t x = code[0] & 0x0f;
  switch (code[1])
  {
    case 0x07: V[x] = delay; break;
    case 0x0a:
      if (!waitingForKey)
      {
        memcpy(saveKeyState, keyState, 16);
        waitingForKey = 1;
        return;
      }
      else
      {
        for (int i = 0; i < 16; ++i)
        {
          if ((saveKeyState[i] == 0) && (keyState[i] == 1))
          {
            waitingForKey = 0;
            V[x] = i;
            PC += 2;
            return;
          }
        }
        return;
      }
    case 0x15: delay = V[x]; break;
    case 0x18: sound = V[x]; break;
    case 0x1e: I += V[x]; break;
    case 0x29: UnimplementedInstruction(); break;
    case 0x33:
      {
        uint8_t tens, ones, hundreds;
        uint8_t value = V[x];
        ones = value % 10;
        value = value / 10;
        tens = value % 10;
        hundreds = value / 10;

        memory[I] = hundreds;
        memory[I + 1] = tens;
        memory[I + 2] = ones;
      }
      break;
    case 0x55:
      for (int i = 0; i <= x; ++i)
      {
        memory[I + i] = V[i];
      }
      I += (x + 1);
      break;
    case 0x65:
      for (int i = 0; i <= x; ++i)
      {
        V[i] = memory[I + i];
      }
      I += (x + 1);
      break;
    default: UnimplementedInstruction(); break;
  }

  PC += 2;
}

void Chip8Emulator::LoadRom(char* file)
{
  FILE* infile = fopen(file, "rb");
  if (infile == NULL)
  {
    printf("File Error %s", file);
    std::exit(1);
  }

  fseek(infile, 0, SEEK_END);
  long lSize = ftell(infile);
  rewind(infile);

  fread(memory+0x200, 1, lSize, infile);
  fclose(infile);
}

int main(int argc, char* argv[])
{
  Chip8Emulator* chip8 = new Chip8Emulator();
  chip8->LoadRom(argv[1]);

  while (chip8->halt == 0)
  {
    chip8->EmulateChip8Op();
  }
  delete chip8;
  return 0;
}