#include "vector.h"

const Vector3f Vector3f::UnitX(1.0, 0.0, 0.0);
const Vector3f Vector3f::UnitY(0.0, 1.0, 0.0);
const Vector3f Vector3f::UnitZ(0.0, 0.0, 1.0);

Vector3f operator +(const Vector3f &v, float s)
{
    return (Vector3f(v.x + s, v.y + s, v.z + s));
}

Vector3f operator +(float s, const Vector3f &v)
{
    return (Vector3f(s + v.x, s + v.y, s + v.z));
}

Vector3f operator +(const Vector3f &u, const Vector3f &v)
{
    return (Vector3f(u.x + v.x, u.y + v.y, u.z + v.z));
}

Vector3f operator -(const Vector3f &v, float s)
{
    return (Vector3f(v.x - s, v.y - s, v.z - s));
}

Vector3f operator -(float s, const Vector3f &v)
{
    return (Vector3f(s - v.x, s - v.y, s - v.z));
}

Vector3f operator -(const Vector3f &u, const Vector3f &v)
{
    return (Vector3f(u.x - v.x, u.y - v.y, u.z - v.z));
}

Vector3f operator *(const Vector3f &v, float s)
{
    return (Vector3f(v.x * s, v.y * s, v.z * s));
}

Vector3f operator *(float s, const Vector3f &v)
{
    return (Vector3f(s * v.x, s * v.y, s * v.z));
}

Vector3f operator *(const Vector3f &u, const Vector3f &v)
{
    return (Vector3f(u.x * v.x, u.y * v.y, u.z * v.z));
}

Vector3f operator /(const Vector3f &v, float s)
{
    float	inv = 1.0f / s;

    return (Vector3f(v.x * inv, v.y * inv, v.z * inv));
}

Vector3f operator /(float s, const Vector3f &v)
{
    return (Vector3f(s / v.x, s / v.y, s / v.z));
}

Vector3f operator /(const Vector3f &u, const Vector3f &v)
{
    return (Vector3f(u.x / v.x, u.y / v.y, u.z / v.z));
}

Vector3f operator -(const Vector3f &v)
{
    return (Vector3f(-v.x, -v.y, -v.z));
}

float dot(const Vector3f &u, const Vector3f &v)
{
    return (u.x * v.x + u.y * v.y + u.z * v.z);
}

Vector3f cross(const Vector3f &u, const Vector3f &v)
{
    return (Vector3f(u.y * v.z - v.y * u.z,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x));
}

float length(const Vector3f &v)
{
    return ((float)sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

bool isZeroLength(const Vector3f &v) {
    float sqlen = v.x*v.x + v.y*v.y + v.z*v.z;
    return sqlen < (1e-06 * 1e-06);
}

Vector3f normalize(const Vector3f &v)
{
    return (v / length(v));
}
