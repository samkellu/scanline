#pragma once
#include <cstdint>
#include <math.h>

#define PI          3.14159
#define SCR_WIDTH   680
#define SCR_HEIGHT  480
#define HFOV        70 * PI / 180
#define DOV         SCR_WIDTH * tan(HFOV/2) / 2 
#define VFOV        2 * atan(SCR_HEIGHT * DOV / 2)

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct Vec3
{
    double x;
    double y;
    double z;
    double magnitude()
    {
        return sqrt(x*x + y*y + z*z);
    }
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

struct Ray
{
    Vec3 source;
    Vec3 unitVec;
    Vec3 hitPoint;
};

struct SLRenderer
{
    int width;
    int height;
    Ray** rays;
};