#pragma once
#include <cstdint>

#define SCR_WIDTH 680
#define SCR_HEIGHT 480

struct Vec3 {
    float x;
    float y;
    float z;
};

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class Ray
{
    public:
        Vec3 source;
        Vec3 unitVec;
        Vec3 hitPoint;
};