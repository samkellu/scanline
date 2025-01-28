#include "scanline.hpp"

ControlState controlState = {false, false, false, false};

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

void processKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    bool pressed = false;
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        pressed = true;
    }
    else if (action != GLFW_RELEASE) return;

    if (key == GLFW_KEY_ESCAPE && pressed)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_W)
        controlState.up = pressed;

    if (key == GLFW_KEY_A)
        controlState.left = pressed;

    if (key == GLFW_KEY_S)
        controlState.down = pressed;
    
    if (key == GLFW_KEY_D)
        controlState.right = pressed;
}

#pragma endregion

int main()
{
    glm::vec3 position = {0, 0, 0};
    glm::vec3 heading = {1, 0, 0};
    int mxc = 0;
    int myc = 0;
    uint32_t vertexShader, fragmentShader, shaderProgram;
    glm::vec3 triangle[3] = 
    {
        {1,0,0},
        {0,0,0},
        {0,1,0},
    };

    glm::vec3 color[3] = 
    {
        {255, 255, 0},
        {255, 255, 0},
        {255, 255, 0},
    };

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
    glfwSetKeyCallback(window, processKeyboard);

    uint32_t vao;
    uint32_t vbo[2];
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(2, &vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 3, triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 3, triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window))
    {

        glClearColor(0, 0, 0, 0);
        glClear(GL_CLEAR_BUFFER);

        glDrawArrays(GL_LINE_LOOP, 0, 3);
        glfwSwapBuffers(window);

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