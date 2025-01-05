#pragma once
#include <cstdint>

#define SCR_WIDTH 680
#define SCR_HEIGHT 480

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct Vec3
{
    float x;
    float y;
    float z;
};

struct Tri
{
    Vec3 p1;
    Vec3 p2;
    Vec3 p3;
    Color c;
};

struct Mesh
{
    Tri* tris;
    int n_tries;
};

struct FrameBuffer
{
    int width;
    int height;
    Color** buf;
};

class Ray
{
    public:
        Vec3 source;
        Vec3 unitVec;
        Vec3 hitPoint;
};