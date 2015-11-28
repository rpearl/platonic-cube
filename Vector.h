/* vim: set ts=8 sts=4 et sw=4 tw=99: */
#ifndef _VECTOR3F_H_
#define _VECTOR3F_H_

#include <cmath>


struct Vector3f
{
    static const Vector3f UnitX;
    static const Vector3f UnitY;
    static const Vector3f UnitZ;
    float	x, y, z;

    inline Vector3f()
    {
        x = y = z = 0.0f;
    }

    inline Vector3f(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    inline Vector3f(float xyz)
    {
        x = y = z = xyz;
    }

    inline Vector3f(const float *xyzArr)
    {
        x = xyzArr[0];
        y = xyzArr[1];
        z = xyzArr[2];
    }

    inline operator const float *() const
    {
        return ((const float *)&x);
    }

    inline float &operator[](unsigned int idx)
    {
        return (*(((float *)&x) + idx));
    }

    inline void operator +=(float s)
    {
        x += s;
        y += s;
        z += s;
    }

    inline void operator +=(const Vector3f &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    inline void operator -=(float s)
    {
        x -= s;
        y -= s;
        z -= s;
    }

    inline void operator -=(const Vector3f &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    inline void operator *=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
    }

    inline void operator *=(const Vector3f &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
    }

    inline void operator /=(float s)
    {
        float	inv = 1.0f / s;

        x *= inv;
        y *= inv;
        z *= inv;
    }

    inline void operator /=(const Vector3f &v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
    }
};
Vector3f cross(const Vector3f &u, const Vector3f &v);
Vector3f normalize(const Vector3f &v);
Vector3f operator *(const Vector3f &u, const Vector3f &v);
Vector3f operator *(const Vector3f &v, float s);
Vector3f operator *(float s, const Vector3f &v);
Vector3f operator +(const Vector3f &u, const Vector3f &v);
Vector3f operator +(const Vector3f &v, float s);
Vector3f operator +(float s, const Vector3f &v);
Vector3f operator -(const Vector3f &u, const Vector3f &v);
Vector3f operator -(const Vector3f &v);
Vector3f operator -(const Vector3f &v, float s);
Vector3f operator -(float s, const Vector3f &v);
Vector3f operator /(const Vector3f &u, const Vector3f &v);
Vector3f operator /(const Vector3f &v, float s);
Vector3f operator /(float s, const Vector3f &v);
bool isZeroLength(const Vector3f &v);
float dot(const Vector3f &u, const Vector3f &v);
float length(const Vector3f &v);


#endif /* _VECTOR3F_H_ */
