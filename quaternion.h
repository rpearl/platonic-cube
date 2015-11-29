/* vim: set ts=8 sts=4 et sw=4 tw=99: */
/**
 * @file quaternion.hpp
 * @brief A quaternion class.
 *
 * @author Eric Butler (edbutler)
 * @author Zeyang Li (zeyangl)
 */

#ifndef _QUATERNION_HPP_
#define _QUATERNION_HPP_

#include "vector3.h"

/*
   This code is loosely based on quaternion code from Ogre3d (www.ogre3d.org).
 */

/**
 * A basic quaternion class.
 * Quaternions represent 3D rotations and can be used to concatenate
 * rotations efficiently. The class provides functions to convert
 * to and from rotation matrices and axis-angle representations.
 *
 * Quaternions can be concatenated right-to-left, just like matrices.
 * That is, to apply rotation A, then B, then C to vector V, do C*B*A*V.
 *
 * The axis-angle representation of a quaterion is a unit vector denoting
 * the axis (starting from the origin) about which the rotation occurs.
 * The angle represents the amount of counter-clockwise rotation in radians.
 * For example, an axis of (1,0,0) and an angle of PI/2 represents the rotation
 * that leaves the x-axis the same, takes the y-axis to the z-axis, and takes
 * the z-axis to the negative y-axis.
 */
class Quaternion {
    public:

        /**
         * The quaternion filled with zero.
         */
        static const Quaternion Zero;

        /**
         * The quaternion representing the identity rotation.
         */
        static const Quaternion Identity;

        float w, x, y, z;

        /**
         * Default constructor. Leaves values uninitialized.
         */
        Quaternion() {}

        /**
         * Construct a quaternion with the given values.
         */
        Quaternion( float w, float x, float y, float z )
            : w( w ), x( x ), y( y ), z( z ) { }

        /**
         * Constructs a quaternion representing a rotation about the given axis
         * by the given angle.
         */
        Quaternion( const Vector3f& axis, float radians );

        Quaternion operator*( const Quaternion& rhs ) const;

        /**
         * Rotate a vector by this quaternion.
         */
        Vector3f operator*( const Vector3f& rhs ) const;

        Quaternion operator*( float s ) const {
            return Quaternion( w * s, x * s, y * s, z * s );
        }

        Quaternion& operator*=( float s ) {
            w *= s;
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        bool operator==( const Quaternion& rhs ) const {
            return rhs.x == x && rhs.y == y &&
                rhs.z == z && rhs.w == w;
        }

        bool operator!=( const Quaternion& rhs ) const {
            return !operator==( rhs );
        }

        /**
         * Convert this quaternion into an angle and axis.
         * Returns the rotation in radians about an axis.
         */
        void to_axis_angle( Vector3f* axis, float* angle ) const;

        /**
         * Returns the X,Y,Z axes rotated by this quaternion.
         */
        void to_axes( Vector3f axes[3] ) const;
};

inline float norm( const Quaternion& q ) {
    return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

inline Quaternion operator*( float s, const Quaternion& rhs ) {
    return rhs * s;
}

Quaternion normalize( const Quaternion& q );

Quaternion conjugate( const Quaternion& q );

Quaternion getRotationFromTo(const Vector3f &source, const Vector3f &dest);

#endif /* _QUATERNION_HPP_ */

