#include "scanline.hpp"

#pragma region Initialise

void fbSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

uint32_t createShader(const char* path, const uint32_t shaderType)
{
    FILE* fp = fopen(path, "r");
    if (fp == NULL) 
    {
        printf("Failed to read shader from file... %s\n", path);
        return 0;
    }

    char* shaderSource = (char*) malloc(128);
    char c;
    int nChars = 0;
    int capacity = 128;
    do
    {
        c = fgetc(fp);
        shaderSource[nChars] = c == EOF ? '\0' : c;
        if (++nChars == capacity)
        {
            capacity += 128;
            shaderSource = (char*) realloc(shaderSource, sizeof(char) * capacity);
        }
    } 
    while (c != EOF);

    fclose(fp);

    int success;
    char infoLog[512];
    uint32_t shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    free(shaderSource);

    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Failed to compile shader %s: %s\n", path, infoLog);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
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
    uint32_t vertexShader, fragmentShader, shaderProgram;

    GLFWwindow* window;
    if (glfwInit() == GLFW_FALSE) return 0;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Scanline", NULL, NULL);
    if (window == NULL) 
    {
        printf("Failed to initialise window...\n");
        return 0;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, fbSizeCallback);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        printf("Failed to initialise GLAD...\n");
        goto clean;
    }

    vertexShader = createShader(VERT_PATH, GL_VERTEX_SHADER);
    if (!vertexShader) goto clean;

    fragmentShader = createShader(FRAG_PATH, GL_FRAGMENT_SHADER);
    if (!fragmentShader) goto clean;

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Failed link shader program: %s\n", infoLog);
        goto clean;
    }

    glUseProgram(shaderProgram);

    while (!glfwWindowShouldClose(window))
    {
        // processInput(window);

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
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}