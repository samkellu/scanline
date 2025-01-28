#pragma once
#include <cstdint>
#include <math.h>
#include <cstdio>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <unistd.h>

#define PI          3.14159
#define SCR_WIDTH   680
#define SCR_HEIGHT  480
#define HFOV        90 * PI / 180
#define DOV         SCR_WIDTH * tan(HFOV/2) / 2 
#define VFOV        2 * atan(SCR_HEIGHT * DOV / 2)
#define STEP_SIZE   0.2f
#define FRAG_PATH   "./fragmentShader.glsl"
#define VERT_PATH   "./vertexShader.glsl"

struct Ray
{
    glm::vec3 source;
    glm::vec3 unitVec;
    glm::vec3 hitPoint;
};

struct ControlState
{
    bool up;
    bool down;
    bool left;
    bool right;
};