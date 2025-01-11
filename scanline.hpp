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

    void normalize()
    {
        double mag = magnitude();
        x /= mag;
        y /= mag;
        z /= mag;
    }

    double dot(Vec3 v)
    {
        return v.x * x + v.y * y + v.z * z;
    }

    void add(Vec3 v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    void subtract(Vec3 v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    void print()
    {
        printf("(%lf, %lf, %lf)\n", x, y, z);
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

Tri triangles[] =
{
    {50, 0, 100},
    {50, 50, 100},
    {0, 0, 100},
    {255, 255, 255, 255}
};

Mesh worldMesh = 
{
    triangles,
    1
};