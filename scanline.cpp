#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "scanline.hpp"
#include <iostream>

#pragma region Initialise

void fbSizeCallback(GLFWwindow* window, int width, int height);

bool initGLFW(GLFWwindow* window)
{
    if (glfwInit() == GLFW_FALSE) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Scanline", NULL, NULL);
    if (!window) 
    {
        printf("Failed to initialise window...\n");
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, fbSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        printf("Failed to initialise GLAD...\n");
        return false;
    }

    // TODO: Load shaders from files...

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
            Vec3 ray = slr->rays[x][y].unitVec + heading;
            ray.normalize();

            for (int m = 0; m < mesh.n_tris; m++)
            {
                Tri tr = mesh.tris[m];
                Vec3 n = tr.normal();

                // Triangle and ray are parallel
                if (n.dot(ray) == 0) continue;

                double d = -n.dot(tr.p0);
                double t = -(n.dot(position) + d) / n.dot(ray);
                // Triangle is behind ray origin
                if (t < 0) continue;

                // Intersection of ray with triangle plane
                Vec3 p = position + ray * t;
                
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

                fb->buf[x][y] = tr.color;
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

    if (!initGLFW(&window, &renderer)) 
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


    Vec3 position; position = {0, 0, 0};
    Vec3 heading; heading = {1, 0, 0};
    int mxc; mxc = 0;
    int myc; myc = 0;
    const uint8_t* currentKeyStates; currentKeyStates = SDL_GetKeyboardState(NULL);
    while (1)
    {
        int enabled = SDL_CaptureMouse(SDL_TRUE);
        printf("%d\n", enabled);
        printf("%s\n", SDL_GetError());

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    goto clean;
            }
        }

        int mx, my;
        SDL_GetGlobalMouseState(&mx, &my);

        double mxa = PI * (mxc - mx) / (double) SCR_WIDTH;
        double mya = -PI * (myc - my) / (double) SCR_WIDTH;
        mxc = mx;
        myc = my;

        double smxa = sin(mxa);
        double smya = sin(mya);
        double cmxa = cos(mxa);
        double cmya = cos(mya);

        heading = (Vec3)
        {
            heading.x * cmxa + smxa * (heading.y + heading.z * cmya),
            heading.y * cmya - heading.z * smya,
            -heading.x * smxa + smya * cmxa * (heading.y + heading.z)
        };

        // heading.normalize();
        heading.print();

        int df = 0, ds = 0;
        if (currentKeyStates[SDL_SCANCODE_UP]) df++;
        if (currentKeyStates[SDL_SCANCODE_DOWN]) df--;
        if (currentKeyStates[SDL_SCANCODE_LEFT]) ds--;
        if (currentKeyStates[SDL_SCANCODE_RIGHT]) ds++;

        Vec3 dd = (Vec3) {heading.x, 0, heading.z} * ds;
        dd = dd + heading.cross((Vec3) {0, 1, 0}) * df;
        dd.normalize();
        position.add(dd * STEP_SIZE);
        getFrame(fb, slr, position, heading, worldMesh);
        drawFrame(renderer, fb);
        // SDL_Delay(10);
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