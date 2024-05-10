#include "cpu.hpp"
#include "display.hpp"
#include <stdio.h>
#include <cstdlib>
#include <chrono>

int main(int argc, char* argv[])
{

  Chip8 chip8;
  if (!chip8.LoadRom(argv[1]))
  {
    printf("Error Loading Rom.");
    std::exit(1);
  }

  Display display("Chip 8 Emulator");

  int cycleDelay = 3;
  auto lastCycleTime = std::chrono::high_resolution_clock::now();

  bool quit = false;

  while (!quit)
  {
    quit = display.ProcessInput(chip8.keys);
    auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

    if (dt > cycleDelay)
    {

      lastCycleTime = currentTime;
      if (!chip8.halt)
      {
        chip8.EmulateCycle();
      }

      if (chip8.drawFlag)
      {
        display.Update(chip8.gfx);
        chip8.drawFlag = false;
      }
    }


  }
  return 0;
}