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

    Vec3 cross(Vec3 v)
    {
        return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
    };

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

    inline Vec3 operator-(Vec3 v)
    {
        return {x - v.x, y - v.y, z - v.z};
    }

    inline Vec3 operator+(Vec3 v)
    {
        return {x + v.x, y + v.y, z + v.z};
    }

    inline Vec3 operator*(double d)
    {
        return {x * d, y * d, z * d};
    }

    void print()
    {
        printf("(%lf, %lf, %lf)\n", x, y, z);
    }
};

struct Tri
{
    Vec3 p0;
    Vec3 p1;
    Vec3 p2;
    Color color;

    Vec3 normal()
    {
        Vec3 e0 = p1 - p0;
        Vec3 e1 = p2 - p0;
        return e0.cross(e1);
    }
};

struct Mesh
{
    Tri* tris;
    int n_tris;
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
    {
        {1.1, 0, 10},
        {0, 1.1, 10},
        {0, 0, 10},
        {255, 255, 255, 255}
    }
};

Mesh worldMesh = 
{
    triangles,
    1
};