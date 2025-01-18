#include "scanline.hpp"

#pragma region Initialise

void fbSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

char* readShaderFromFile(const char* path)
{
    FILE* fp = fopen(path, "r");
    if (fp == NULL) return NULL;

    char* output = (char*) malloc(128);
    char c;
    int nChars = 0;
    int capacity = 128;
    do
    {
        c = fgetc(fp);
        output[nChars] = c == EOF ? '\0' : c;
        if (++nChars == capacity)
        {
            capacity += 128;
            output = (char*) realloc(output, sizeof(char) * capacity);
        }
    } 
    while (c != EOF);

    fclose(fp);
    return output;
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
        return false;
    }

    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, fbSizeCallback);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        printf("Failed to initialise GLAD...\n");
        return false;
    }

    char* fragShader = readShaderFromFile(FRAG_PATH);
    if (fragShader == NULL)
    {
        printf("Failed to read fragment shader...");
        return false;
    }

    int success;
    char infoLog[512];
    uint64_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragShader, NULL);
    free(fragShader);

    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Failed to compile fragment shader: %s\n", infoLog);
        return false;
    }

    char* vertShader = readShaderFromFile(VERT_PATH);
    if (vertShader == NULL)
    {
        printf("Failed to read vertex shader...");
        return false;
    }

    uint64_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertShader, NULL);
    free(vertShader);

    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Failed to compile vertex shader: %s\n", infoLog);
        return false;
    }

    uint64_t shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader, 512, NULL, infoLog);
        printf("Failed link shader program: %s\n", infoLog);
        glDeleteProgram(shader);
        return false;
    }

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