#include "display.hpp"
#include <SDL2/SDL.h>
#include <stdint.h>

Display::Display(char const* title)
{
  screenWidth = 64;
  screenHeight = 32;
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth * 10, screenHeight * 10, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

Display::~Display()
{
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Display::Update(uint8_t* buffer)
{
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Rect* destRect = new SDL_Rect;
  destRect->x = 0;
  destRect->y = 0;
  destRect->w = 10;
  destRect->h = 10;

  for (int y = 0; y < 32; y++)
  {
    for (int x = 0; x < 64; x++)
    {
      if (buffer[(y*64) + x] == 1)
      {
        destRect->x = x * 10;
        destRect->y = y * 10;

        SDL_RenderFillRect(renderer, destRect);
      }
    }
  }

  delete destRect;

  SDL_RenderPresent(renderer);
}

bool Display::ProcessInput()
{
  bool quit = false;
  SDL_Event e;

  while (SDL_PollEvent(&e))
  {
    switch(e.type)
    {
      case SDL_QUIT:
        {
          quit = true;
        }
        break;
    }
  }

  return quit;
}