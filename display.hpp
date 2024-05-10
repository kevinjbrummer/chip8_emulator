#include <SDL2/SDL.h>
#include <stdint.h>

class Display
{
  public:
    int screenWidth;
    int screenHeight;
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};

    Display(char const* title);
    ~Display();
    void Update(uint8_t* buffer);
    bool ProcessInput();
};