#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_mixer.h>

class Display
{
  public:
    int screenWidth;
    int screenHeight;
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    Mix_Chunk* beep{};

    Display(char const* title);
    ~Display();
    void Update(uint8_t* buffer);
    void PlaySound();
    bool ProcessInput(uint8_t* keys);
};