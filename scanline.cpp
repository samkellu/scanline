#include <SDL2/SDL.h>
#include "scanline.hpp"
#include <iostream>

// Return the next frame to be drawn
void getFrame(Color** fb, int width, int height)
{
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            fb[x][y] = { (uint8_t) (x % 256), (uint8_t) (y % 256), (uint8_t) (x % 256), 200};
        }
    }

}

Color** getFrameBuffer(int width, int height)
{
    Color** fb = (Color**) malloc(sizeof(Color*) * width);
    if (!fb) return NULL;

    for (int i = 0; i < width; i++)
    {
        fb[i] = (Color*) malloc(sizeof(Color) * height);
        if (!fb[i])
        {
            for (int j = i - 1; j >= 0; j--)
            {
                free(fb[j]);
            }

            free(fb);
            return NULL;
        }
    }

    return fb;
}

bool initSDL(SDL_Window** window, SDL_Renderer** renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cout << "Failed to initialise: " << SDL_GetError();
        return false;
    }

    *window = SDL_CreateWindow("Scanline", 
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              SCR_WIDTH, SCR_HEIGHT, 0);

    if (!*window)
    {
        std::cout << "Failed to create window: " << SDL_GetError();
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, 0, 0);
    if (!*renderer)
    {
        std::cout << "Failed to create renderer: " << SDL_GetError();
        return false;
    }

    return true;
}

int main()
{
    SDL_Window* window;
    SDL_Event event;
    SDL_Renderer* renderer;

    if (!initSDL(&window, &renderer)) return -1;

    Color** fb = getFrameBuffer(SCR_WIDTH, SCR_HEIGHT);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) goto clean;
        }

        getFrame(fb, SCR_WIDTH, SCR_HEIGHT);
        for (int x = 0; x < SCR_WIDTH; x++)
        {
            for (int y = 0; y < SCR_HEIGHT; y++)
            {
                Color px = fb[x][y];
                SDL_SetRenderDrawColor(renderer, px.r, px.g, px.b, px.a);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }   

clean:
    for (int i = 0; i < SCR_WIDTH; i++)
    {
        free(fb[i]);
    }

    free(fb);
    return 0;
}