#include "display.hpp"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_mixer.h>

Display::Display(char const* title)
{
  screenWidth = 64;
  screenHeight = 32;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth * 10, screenHeight * 10, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  beep = Mix_LoadWAV("beep-02.wav");

}

Display::~Display()
{
  Mix_FreeChunk(beep);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Display::Update(uint8_t* buffer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
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

void Display::PlaySound()
{
  Mix_PlayChannel(-1, beep, 0);
}

bool Display::ProcessInput(uint8_t* keys)
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
      case SDL_KEYDOWN:
        {
          switch (e.key.keysym.sym)
          {
            case SDLK_ESCAPE: quit = true; break;
            case SDLK_x: keys[0x0] = 1; break;
            case SDLK_1: keys[0x1] = 1; break;
            case SDLK_2: keys[0x2] = 1; break;
            case SDLK_3: keys[0x3] = 1; break;
            case SDLK_q: keys[0x4] = 1; break;
            case SDLK_w: keys[0x5] = 1; break;
            case SDLK_e: keys[0x6] = 1; break;
            case SDLK_a: keys[0x7] = 1; break;
            case SDLK_s: keys[0x8] = 1; break;
            case SDLK_d: keys[0x9] = 1; break;
            case SDLK_z: keys[0xA] = 1; break;
            case SDLK_c: keys[0xB] = 1; break;
            case SDLK_4: keys[0xC] = 1; break;
            case SDLK_r: keys[0xD] = 1; break;
            case SDLK_f: keys[0xE] = 1; break;
            case SDLK_v: keys[0xF] = 1; break;
            default: break;
          }
        }
        break;
      case SDL_KEYUP:
        {
          switch (e.key.keysym.sym)
          {
            case SDLK_ESCAPE: quit = true; break;
            case SDLK_x: keys[0x0] = 0; break;
            case SDLK_1: keys[0x1] = 0; break;
            case SDLK_2: keys[0x2] = 0; break;
            case SDLK_3: keys[0x3] = 0; break;
            case SDLK_q: keys[0x4] = 0; break;
            case SDLK_w: keys[0x5] = 0; break;
            case SDLK_e: keys[0x6] = 0; break;
            case SDLK_a: keys[0x7] = 0; break;
            case SDLK_s: keys[0x8] = 0; break;
            case SDLK_d: keys[0x9] = 0; break;
            case SDLK_z: keys[0xA] = 0; break;
            case SDLK_c: keys[0xB] = 0; break;
            case SDLK_4: keys[0xC] = 0; break;
            case SDLK_r: keys[0xD] = 0; break;
            case SDLK_f: keys[0xE] = 0; break;
            case SDLK_v: keys[0xF] = 0; break;
            default: break;
          }
        }
        break;
    }
  }

  return quit;
}