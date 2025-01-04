#include <SDL2/SDL.h>
#include "scanline.h"
#include <iostream>

int main()
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cout << "Failed to initialise: " << SDL_GetError();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Scanline", 
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCR_WIDTH,
                                          SCR_HEIGHT,
                                          0);

    if (!window)
    {
        std::cout << "Failed to create window: " << SDL_GetError();
        return -1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if (!surface)
    {
        std::cout << "Failed to extract surface: " << SDL_GetError();
        return -1; 
    }

    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) > 0)
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
            }
            
            SDL_UpdateWindowSurface(window);
        }
    }   

    return 1;
}