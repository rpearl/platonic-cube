/* vim: set ts=8 sts=4 et sw=4 tw=99: */
#include <FastLED.h>
#include "cube_util.h"
#include "cube.h"

#include "quaternion.h"
/*
Vector3f normals[PANELS] = {
    //0 is the (x,z) plane at y=7
    Vector3f( 0.0f,  1.0f,  0.0f),
    //1 is the (x,y) plane at z=7
    Vector3f( 0.0f,  0.0f,  1.0f),
    //2 is the (y,z) plane at x=7
    Vector3f( 1.0f,  0.0f,  0.0f),
    //3 is the (x,y) plane at z=0
    Vector3f( 0.0f,  0.0f, -1.0f),
    //4 is the (y,z) plane at x=0
    Vector3f(-1.0f,  0.0f,  0.0f),
    //5 is the (x,z) plane at y=0
    Vector3f( 0.0f, -1.0f,  0.0f)
};
*/

#define NOISY_PIN A4

static unsigned int noisyBitOut(void)
{
    static unsigned long firstTime=1, prev=0;
    unsigned long bit1=0, bit0=0, x=0, limit=99;
    if (firstTime)
    {
        firstTime=0;
        prev=analogRead(NOISY_PIN);
    }
    while (limit--)
    {
        x=analogRead(NOISY_PIN);
        bit1=(prev!=x?1:0);
        prev=x;
        x=analogRead(NOISY_PIN);
        bit0=(prev!=x?1:0);
        prev=x;
        if (bit1!=bit0)
            break;
    }
    return bit1;
}

uint64_t seedOut(uint8_t noOfBits)
{
    // return value with 'noOfBits' random bits set
    uint64_t seed=0;
    for (uint8_t i=0;i<noOfBits;++i)
        seed = (seed<<1) | noisyBitOut();
    return seed;
}

void setPixel3d(int8_t x, int8_t y, int8_t z, const CRGB &c) {
    for (auto idx : getPixel3d(x,y,z)) {
        leds[idx] = c;
    }
}

static void getCorrectedCoord(const Vector3f &gravity,
                       int8_t x, int8_t y, int8_t z,
                       int8_t *xc, int8_t *yc, int8_t *zc) {
    Quaternion q = getRotationFromTo(Vector3f::UnitY, -gravity);

    Vector3f v(x-4,y-4,z-4);

    Vector3f ret = q*v;

    *xc = (int)ret.x+4;
    *yc = (int)ret.y+4;
    *zc = (int)ret.z+4;
}

CoordList getPixel3d(int8_t x, int8_t y, int8_t z) {
    CoordList out;
    if (x >= LEDS_PER_ROW || y >= LEDS_PER_ROW || z >= LEDS_PER_ROW)
        return out;

    if (x < 0 || y < 0 || z < 0)
        return out;


    if (x == 0) {
        out.push_back(PIXEL_IN_PANEL(4, P(FLIP(y), z)));
    } else if (x == LEDS_PER_ROW-1) { // panel 2
        out.push_back(PIXEL_IN_PANEL(2, P(FLIP(y), FLIP(z))));
    }

    if (y == 0) {
        out.push_back(PIXEL_IN_PANEL(5, P(FLIP(x), FLIP(z))));
    } else if (y == LEDS_PER_ROW-1) {
        out.push_back(PIXEL_IN_PANEL(0, P(x, FLIP(z))));
    }

    if (z == 0) {
        out.push_back(PIXEL_IN_PANEL(3, P(FLIP(y), FLIP(x))));
    } else if (z == LEDS_PER_ROW-1) {
        out.push_back(PIXEL_IN_PANEL(1, P(FLIP(y), x)));
    }

    return out;
}

CoordList getPixel3dCompensated(const Vector3f &gravity, int8_t x, int8_t y, int8_t z) {
    int8_t xc, yc, zc;
    getCorrectedCoord(gravity, x,y,z, &xc, &yc, &zc);
    return getPixel3d(xc,yc,zc);
}

void setPixel3dCompensated(const Vector3f &gravity, int8_t x, int8_t y, int8_t z, const CRGB &c) {
    int8_t xc, yc, zc;
    getCorrectedCoord(gravity, x,y,z, &xc, &yc, &zc);
    setPixel3d(xc,yc,zc,c);
}
