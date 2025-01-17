#include "scanline.hpp"

#pragma region Initialise

void fbSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool initGLFW(GLFWwindow** window)
{
    if (glfwInit() == GLFW_FALSE) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Scanline", NULL, NULL);
    if (*window == NULL) 
    {
        printf("Failed to initialise window...\n");
        fflush(stdout);
        return false;
    }

    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, fbSizeCallback);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        printf("Failed to initialise GLAD...\n");
        return false;
    }

    // TODO: Load shaders from files...

    return true;
}

#pragma endregion

#pragma region Inputs

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

#pragma endregion

int main()
{
    Vec3 position = {0, 0, 0};
    Vec3 heading = {1, 0, 0};
    int mxc = 0;
    int myc = 0;

    GLFWwindow* window;
    if (!initGLFW(&window)) 
    {
        printf("Failed to initialise GLFW!\n");
        goto clean;
    }

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // double mxa = PI * (mxc - mx) / (double) SCR_WIDTH;
        // double mya = -PI * (myc - my) / (double) SCR_WIDTH;
        // mxc = mx;
        // myc = my;

        // double smxa = sin(mxa);
        // double smya = sin(mya);
        // double cmxa = cos(mxa);
        // double cmya = cos(mya);

        // heading = (Vec3)
        // {
        //     heading.x * cmxa + smxa * (heading.y + heading.z * cmya),
        //     heading.y * cmya - heading.z * smya,
        //     -heading.x * smxa + smya * cmxa * (heading.y + heading.z)
        // };

        // // heading.normalize();
        // heading.print();

        // int df = 0, ds = 0;
        // if (currentKeyStates[SDL_SCANCODE_UP]) df++;
        // if (currentKeyStates[SDL_SCANCODE_DOWN]) df--;
        // if (currentKeyStates[SDL_SCANCODE_LEFT]) ds--;
        // if (currentKeyStates[SDL_SCANCODE_RIGHT]) ds++;

        // Vec3 dd = (Vec3) {heading.x, 0, heading.z} * ds;
        // dd = dd + heading.cross((Vec3) {0, 1, 0}) * df;
        // dd.normalize();
        // position.add(dd * STEP_SIZE);

        // getFrame(fb, slr, position, heading, worldMesh);
        // drawFrame(renderer, fb);
        // SDL_Delay(10);
    }   

clean:
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}