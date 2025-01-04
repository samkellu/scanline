#include <SDL2/SDL.h>
#include "scanline.h"
#include <iostream>

int main()
{
    SDL_Window* window;
    SDL_Event event;
    SDL_Renderer* renderer;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cout << "Failed to initialise: " << SDL_GetError();
        return -1;
    }

    window = SDL_CreateWindow("Scanline", 
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              SCR_WIDTH, SCR_HEIGHT, 0);

    if (!window)
    {
        std::cout << "Failed to create window: " << SDL_GetError();
        return -1;
    }

    renderer = SDL_CreateRenderer(window, 0, 0);
    if (!renderer)
    {
        std::cout << "Failed to create renderer: " << SDL_GetError();
        return -1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    int i = 0;
    while (1)
    {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) break;

        SDL_RenderDrawPoint(renderer, i % SCR_WIDTH, i / SCR_WIDTH);
        SDL_RenderPresent(renderer);
        i++;
        SDL_Delay(1);
    }   

    return 0;
}