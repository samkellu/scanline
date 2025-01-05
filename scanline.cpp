#include <SDL2/SDL.h>
#include "scanline.hpp"
#include <iostream>

// Use slr to fill frame buffer
void getFrame(FrameBuffer* fb, SLRenderer* slr)
{
    for (int x = 0; x < fb->width; x++)
    {
        for (int y = 0; y < fb->height; y++)
        {
            // Project ray for each pixel, return color of first mesh object hit to display, scale alpha by distance ig
            fb->buf[x][y] = { (uint8_t) (x % 256), (uint8_t) (y % 256), (uint8_t) (x % 256), 200};
        }
    }

}

// goto clean on fail
FrameBuffer* getFrameBuffer(int width, int height)
{
    FrameBuffer* fb = (FrameBuffer*) malloc(sizeof(FrameBuffer));
    if (!fb) return NULL;

    fb->width = width;
    fb->height = height;
    fb->buf = (Color**) malloc(sizeof(Color*) * width);
    if (!fb->buf) return NULL;

    for (int i = 0; i < width; i++)
    {
        fb->buf[i] = (Color*) malloc(sizeof(Color) * height);
        if (!fb->buf[i]) return NULL;
    }

    return fb;
}

// goto clean on fail
SLRenderer* getScanlineRenderer(int width, int height)
{
    SLRenderer* slr = (SLRenderer*) malloc(sizeof(SLRenderer));
    if (!slr) return NULL;

    slr->width = width;
    slr->height = height;
    slr->rays = (Ray**) malloc(sizeof(Ray*) * width);
    if (!slr->rays) return NULL;

    for (int x = 0; x < width; x++)
    {
        slr->rays[x] = (Ray*) malloc(sizeof(Ray) * height);
        if (!slr->rays[x]) return NULL;

        for (int y = 0; y < slr->height; y++)
        {
            // Do some lin alg here
            slr->rays[x][y].unitVec = {};
        }
    }

    return slr;
}

void drawFrame(SDL_Renderer* renderer, FrameBuffer* fb)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    for (int x = 0; x < fb->width; x++)
    {
        for (int y = 0; y < fb->height; y++)
        {
            Color px = fb->buf[x][y];
            SDL_SetRenderDrawColor(renderer, px.r, px.g, px.b, px.a);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_RenderPresent(renderer);
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
    FrameBuffer* fb;
    SLRenderer* slr;

    if (!initSDL(&window, &renderer)) 
    {
        printf("Failed to initialise SDL!\n");
        goto clean;
    }

    fb = getFrameBuffer(SCR_WIDTH, SCR_HEIGHT);
    if (!fb)
    {
        printf("Failed to initialise frame buffer!\n");
        goto clean;
    }

    slr = getScanlineRenderer(SCR_WIDTH, SCR_HEIGHT);
    if (!slr)
    {
        printf("Failed to initialise renderer!\n");
        goto clean;
    }


    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) goto clean;
        }

        getFrame(fb, slr);
        drawFrame(renderer, fb);
        SDL_Delay(100);
    }   

clean:
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    if (fb && fb->buf)
    {
        for (int i = 0; i < fb->width; i++)
        {
            free(fb->buf[i]);
        }

        free(fb->buf);
        free(fb);
    }

    if (slr && slr->rays)
    {
        for (int i = 0; i < slr->width; i++)
        {
            free(slr->rays[i]);
        }

        free(slr->rays);
        free(slr);
    }

    return 0;
}