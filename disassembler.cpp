#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <fstream>
#include <cstdlib>

using namespace std;

void DisassembleChip8Op(uint8_t *codebuffer, int pc)
{
  uint8_t *code = &codebuffer[pc];
  uint8_t firstnib = (code[0] >> 4);
  printf("%04x %02x %02x ", pc, code[0], code[1]);

  switch (firstnib)
  {
    case 0x00:
      {
        switch (code[1])
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
        uint8_t reg = (code[0] & 0x0f);
        printf("%-10s $%01x%02x", "JUMP", reg, code[1]);
      }
      break;
    case 0x02:
      {
        uint8_t reg = (code[0] & 0x0f);
        printf("%-10s %01x%02x", "CALL", reg, code[1]);
      }
      break;
    case 0x03:
      {
        uint8_t reg = (code[0] & 0x0f);
        printf("%-10s V%01x, #$%02x", "SKIP.EQ", reg, code[1]);
      }
      break;
    case 0x04:
      {
        uint8_t reg = (code[0] & 0x0f);
        printf("%-10s V%01x, #$%02x", "SKIP.NE", reg, code[1]);
      }
      break;
    case 0x05:
      printf("%-10s V%01x, V%01x", "SKIP.EQ", (code[0] & 0x0f), (code[1] >> 1));
      break;
    case 0x06:
      {
        uint8_t reg = (code[0] & 0x0f);
        printf("%-10s V%01x,#$%02x", "M0V", reg, code[1]);
      }
      break;
    case 0x07:
      {
        uint8_t reg = (code[0] & 0x0f);
        printf("%-10s V%01x,#$%02x", "ADI", reg, code[1]);
      }
      break;
    case 0x08:
      {
        switch (code[1] & 0x0f)
        {
        case 0x00:
          printf("%-10s V%01x, V%01x", "MOV", (code[0] & 0x0f), (code[1] >> 1));
          break;
        case 0x01:
          printf("%-10s V%01x, V%01x", "OR", (code[0] & 0x0f), (code[1] >> 1));
          break;
        case 0x02:
          printf("%-10s V%01x, V%01x", "AND", (code[0] & 0x0f), (code[1] >> 1));
          break;
        case 0x03:
          printf("%-10s V%01x, V%01x", "XOR", (code[0] & 0x0f), (code[1] >> 1));
          break;
        case 0x04:
          printf("%-10x V%01x, V%01x", "ADD.", (code[0] & 0x0f), (code[1] >> 1));
          break;
        case 0x05:
          printf("%-10x V%01x, V%01x", "SUB.", (code[0] & 0x0f), (code[1] >> 1));
          break;
        case 0x06:
          printf("%-10x V%01x", "SHR.", (code[0] & 0x0f));
          break;
        case 0x07:
          printf("%-10x V%01x, V%01x", "SUBB.", (code[0] & 0x0f), (code[1] >> 1));
          break;
        case 0x0e:
          printf("%-10x V%01x", "SHL.", (code[0] & 0x0f));
          break;
        default:
          printf("UNKOWN 8");
          break;
        }
      }
      break;
    case 0x09:
      printf("%-10s V%01x, V%01x", "SKIP.NE", (code[0] & 0x0f), (code[1] >> 1));
      break;
    case 0x0a:
      {
        uint8_t addresshi = code[0] & 0x0f;
        printf("%-10s I, #$%01x%02x", "MVI", addresshi, code[1]);
      }
      break;
    case 0x0b:
      printf("%-10s $%01x%02x(V0)", "MVI", (code[0] & 0x0f), code[1]);
      break;
    case 0x0c:
      printf("%-10s V%01x, #$%02x", "RAND", (code[0] & 0x0f), code[1]);
      break;
    case 0x0d:
      printf("%-10s V%01x, V%01x, #$%01x", "SPRITE", (code[0] & 0x0f), (code[1] >> 4), (code[1] & 0x0f));
      break;
    case 0x0e:
      {
        switch(code[1])
        {
          case 0x9e:
            printf("%-10s V%01x", "SKIP.KEY", (code[0] & 0x0f));
            break;
          case 0xa1:
            printf("%-10s V%01x", "SKIP.NOKEY", (code[0] & 0x0f));
            break;
          default:
            printf("UNKNOWN E");
            break;
        }
      }
      break;
    case 0x0f:
      {
        switch(code[1])
        {
          case 0x07:
            printf("%-10s V%01x, DELAY", "MOV", (code[0] & 0x0f));
            break;
          case 0x0a:
            printf("%-10s V%01x", "WAIT.KEY", (code[0] & 0x0f));
            break;
          case 0x15:
            printf("%-10s DELAY, V%01x", "MOV", (code[0] & 0x0f));
            break;
          case 0x18:
            printf("%-10s SOUND, V%01x", "MOV", (code[0] & 0x0f));
            break;
          case 0x1e:
            printf("%-10s I, V%01x", "ADD", (code[0] & 0x0f));
            break;
          case 0x29:
            printf("%-10s V%01x", "SPRITECHAR", (code[0] & 0x0f));
            break;
          case 0x33:
            printf("%-10s V%01x", "MOVBCD", (code[0] & 0x0f));
            break;
          case 0x55:
            printf("%-10s (I), V0~V%01x", "MOVM", (code[0] & 0x0f));
            break;
          case 0x65:
            printf("%-10s V0~V%01x, (I)", "MOVM", (code[0] & 0x0f));
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

int main (int argc, char* argv[])
{
  FILE * infile;
  long lSize;
  unsigned char * buffer;
  size_t result;

  infile = fopen(argv[1], "rb");
  if (infile == NULL)
  {
    printf("File Error %s", argv[1]);
    exit(1);
  }

  fseek(infile, 0, SEEK_END);
  lSize = ftell(infile);
  rewind(infile);

  buffer = (unsigned char*)malloc(sizeof(char)*lSize);

  result = fread(buffer+0x200, 1, lSize, infile);
  fclose(infile);

  int pc = 0x200;

  while (pc < lSize + 0x200)
  {
    DisassembleChip8Op(buffer, pc);
    pc += 2;
    printf("\n");
  }

  free(buffer);
  return 0;
}