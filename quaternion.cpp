/* vim: set ts=8 sts=4 et sw=4 tw=99: */
/**
 * @file quaternion.cpp
 * @brief A quaternion class.
 *
 * @author Eric Butler (edbutler)
 * @author Zeyang Li (zeyangl)
 */
#include <iostream>
#include <math.h>

#include "quaternion.h"

const Quaternion Quaternion::Zero( 0.0, 0.0, 0.0, 0.0 );

const Quaternion Quaternion::Identity( 1.0, 0.0, 0.0, 0.0 );

static void make_unit( Quaternion& q )
{
    float maginv = 1.0 / sqrt( norm( q ) );
    q.x *= maginv;
    q.y *= maginv;
    q.z *= maginv;
    q.w *= maginv;
}

Quaternion::Quaternion( const Vector3f& axis, float radians )
{
    radians *= 0.5;
    Vector3f naxis = normalize( axis );
    float sine = sin( radians );

    w = cos( radians );
    x = sine * naxis.x;
    y = sine * naxis.y;
    z = sine * naxis.z;

    make_unit( *this );
}

Quaternion Quaternion::operator*( const Quaternion& rhs ) const
{
    return Quaternion(
            w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
            w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x
            );
}

Vector3f Quaternion::operator*( const Vector3f& v ) const
{
    // nVidia SDK implementation
    Vector3f qvec( x, y, z );
    Vector3f uv = cross( qvec, v );
    Vector3f uuv = cross( qvec, uv );
    uv *= ( 2.0 * w );
    uuv *= 2.0;

    return v + uv + uuv;
}

void Quaternion::to_axis_angle( Vector3f* axis, float* angle ) const
{
    // The quaternion representing the rotation is
    // q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
    float norm = x * x + y * y + z * z;
    if ( norm > 0.0 ) {
        *angle = 2.0 * acos( w );
        float inverse_length = 1 / sqrt( norm );
        axis->x = x * inverse_length;
        axis->y = y * inverse_length;
        axis->z = z * inverse_length;
    } else {
        // angle is 0 (mod 2*pi), so any axis will do
        *angle = 0.0;
        *axis = Vector3f::UnitX;
    }
}

static void rotate_axes( const Quaternion& quat,
        Vector3f &ax, Vector3f &ay, Vector3f &az)
{
    float x2  = 2.0 * quat.x;
    float y2  = 2.0 * quat.y;
    float z2  = 2.0 * quat.z;
    float xw2 = x2 * quat.w;
    float yw2 = y2 * quat.w;
    float zw2 = z2 * quat.w;
    float xx2 = x2 * quat.x;
    float xy2 = y2 * quat.x;
    float xz2 = z2 * quat.x;
    float yy2 = y2 * quat.y;
    float yz2 = z2 * quat.y;
    float zz2 = z2 * quat.z;

    ax.x = 1.0 - ( yy2 + zz2 );
    ax.y = xy2 + zw2;
    ax.z = xz2 - yw2;

    ay.x = xy2 - zw2;
    ay.y = 1.0 - ( xx2 + zz2 );
    ay.z = yz2 + xw2;

    az.x = xz2 + yw2;
    az.y = yz2 - xw2;
    az.z = 1.0 - ( xx2 + yy2 );
}

void Quaternion::to_axes( Vector3f axes[3] ) const
{
    rotate_axes( *this, axes[0], axes[1], axes[2]);
}

Quaternion normalize( const Quaternion& q )
{
    Quaternion rv( q );
    make_unit( rv );
    return rv;
}

Quaternion conjugate( const Quaternion& q )
{
    return Quaternion( q.w, -q.x, -q.y, -q.z );
}

Quaternion getRotationFromTo(const Vector3f &source, const Vector3f &dest) {
    Vector3f v0 = normalize(source);
    Vector3f v1 = normalize(dest);

    float d = dot(v0, v1);

    if (d >= 1.0f) {
         return Quaternion::Identity;
    }

    if (d < (1e-6f - 1.0f)) {
        // Arbitrary axis of rotation as these vectors are inverse
        Vector3f axis = cross(Vector3f::UnitX, source);
        if (isZeroLength(axis)) {
            axis = cross(Vector3f::UnitY, source);
        }
        axis = normalize(axis);

        return Quaternion(axis, M_PI);
    } else {
        float s = sqrt((1+d)*2);
        float invs = 1 / s;
        Vector3f c = cross(v0, v1);
        Quaternion q(s / 2, c.x * invs, c.y * invs, c.z * invs);

        return normalize(q);
    }
}

std::ostream& operator <<( std::ostream& o, const Quaternion& q )
{
    o << "Quaternion(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
    return o;
}

