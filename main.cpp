#include "cpu.hpp"
#include "display.hpp"
#include <stdio.h>
#include <cstdlib>

int main(int argc, char* argv[])
{

  Chip8 chip8;
  if (!chip8.LoadRom(argv[1]))
  {
    printf("Error Loading Rom.");
    std::exit(1);
  }

  Display display("Chip 8 Emulator");

  bool quit = false;

  while (!quit)
  {
    if (!chip8.halt)
    {
      chip8.EmulateCycle();
    }

    if (chip8.drawFlag)
    {
      display.Update(chip8.gfx);
      chip8.drawFlag = false;
    }

    quit = display.ProcessInput();

  }
  return 0;
}