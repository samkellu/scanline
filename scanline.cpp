#include <SDL2/SDL.h>
#include "scanline.hpp"
#include <iostream>

#pragma region Initialise

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

    double viewportHalfWidth = DOV * tan(HFOV / 2);
    double viewportHalfHeight = DOV * tan(VFOV / 2);

    // Get all ray unit vectors in screenspace frame of ref
    for (int x = 0; x < width; x++)
    {
        slr->rays[x] = (Ray*) malloc(sizeof(Ray) * height);
        if (!slr->rays[x]) return NULL;

        double vx = (x / (double) width - 0.5) * viewportHalfWidth;
        for (int y = 0; y < slr->height; y++)
        {
            double vy = (y / (double) width - 0.5) * viewportHalfWidth;
            slr->rays[x][y].unitVec = {vx, vy, DOV};
            slr->rays[x][y].unitVec.normalize();
        }
    }

    return slr;
}

#pragma endregion


// Use slr to fill frame buffer
void getFrame(FrameBuffer* fb, SLRenderer* slr, Vec3 position, Vec3 heading, Mesh mesh)
{
    // Multiply by matrix to convert to worldspace
    for (int x = 0; x < fb->width; x++)
    {
        for (int y = 0; y < fb->height; y++)
        {
            fb->buf[x][y] = {0, 0, 0, 0};
            Vec3 newVec = heading;

// https://stackoverflow.com/questions/14607640/rotating-a-vector-in-3d-space
//             In 3D rotating around the Z-axis would be

//     |cos θ   −sin θ   0| |x|   |x cos θ − y sin θ|   |x'|
//     |sin θ    cos θ   0| |y| = |x sin θ + y cos θ| = |y'|
//     |  0       0      1| |z|   |        z        |   |z'|
// around the Y-axis would be

//     | cos θ    0   sin θ| |x|   | x cos θ + z sin θ|   |x'|
//     |   0      1       0| |y| = |         y        | = |y'|
//     |−sin θ    0   cos θ| |z|   |−x sin θ + z cos θ|   |z'|
// around the X-axis would be

//     |1     0           0| |x|   |        x        |   |x'|
//     |0   cos θ    −sin θ| |y| = |y cos θ − z sin θ| = |y'|
//     |0   sin θ     cos θ| |z|   |y sin θ + z cos θ|   |z'|
            bool hit = false;
            for (int m = 0; m < mesh.n_tris; m++)
            {
                Tri tr = mesh.tris[m];
                Vec3 n = tr.normal();

                // Triangle and ray are parallel
                if (n.dot(heading) == 0) continue;

                double d = -n.dot(tr.p0);
                double t = -(n.dot(position) + d) / n.dot(heading);
                // Triangle is behind ray origin
                if (t < 0) continue;

                // Intersection of ray with triangle plane
                Vec3 p = position + heading * t;
                
                // Check p is on the correct side of each edge
                Vec3 pVertex = p - tr.p0;
                Vec3 nEdge = (tr.p1 - tr.p0).cross(pVertex);
                if (n.dot(nEdge) < 0) continue;

                pVertex = p - tr.p1;
                nEdge = (tr.p2 - tr.p1).cross(pVertex);
                if (n.dot(nEdge) < 0) continue;

                pVertex = p - tr.p2;
                nEdge = (tr.p0 - tr.p2).cross(pVertex);
                if (n.dot(nEdge) < 0) continue;

                bool hit = true;

                fb->buf[x][y] = tr.color;
            }

            if (hit){
                printf("hell yeh\n");
                printf("(%d, %d) %d %d %d\n", x, y, fb->buf[x][y].r, fb->buf[x][y].g, fb->buf[x][y].b);
            }
        }
    }

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

        getFrame(fb, slr, {1,1,1}, {1,1,1}, worldMesh);
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