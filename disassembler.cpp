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
    case 0x03: printf("3 not handled yet"); break;
    case 0x04: printf("4 not handled yet"); break;
    case 0x05: printf("5 not handled yet"); break;
    case 0x06:
      {
        uint8_t reg = (code[0] & 0x0f);    
        printf("%-10s V%01x,#$%02x", "MVI", reg, code[1]);    
      }
      break;
    case 0x07: 
      {
        uint8_t reg = (code[0] & 0x0f);
        printf("%-10s V%01x,#$%02x", "ADI", reg, code[1]); 
      }
      break;
    case 0x08: printf("8 not handled yet"); break;
    case 0x09: printf("9 not handled yet"); break;
    case 0x0a:
      {
        uint8_t addresshi = code[0] & 0x0f;
        printf("%-10s I, #&%01x%02x", "MVI", addresshi, code[1]);
      } 
      break;
    case 0x0b: printf("b not handled yet"); break;
    case 0x0c: printf("c not handled yet"); break;
    case 0x0d: printf("d not handled yet"); break;
    case 0x0e: printf("e not handled yet"); break;
    case 0x0f: printf("f not handled yet"); break;
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