/* vim: set ts=8 sts=4 et sw=4 tw=99: */
#ifndef _UTIL_H_
#define _UTIL_H_
#include "cube.h"

#include "vector3.h"

class CoordList {
    int16_t arr[3];
    int8_t pos;
    public:
    CoordList() : pos(0) {
        for (uint8_t i = 0; i < 3; i++)
            arr[i] = -1;
    }

    void push_back(int16_t val) {
        arr[pos++] = val;
    }

    int16_t *begin() {
        return &arr[0];
    }

    int16_t *end() {
        return &arr[pos];
    }
};

//Randomness
uint64_t seedOut(uint8_t noOfBits);

//extern Vector3f normals[PANELS];

void setPixel3d(int8_t x, int8_t y, int8_t z, const CRGB &c);

CoordList getPixel3d(int8_t x, int8_t y, int8_t z);
void setPixel3dCompensated(const Vector3f &gravity, int8_t x, int8_t y, int8_t z, const CRGB &c);
CoordList getPixel3dCompensated(const Vector3f &gravity, int8_t x, int8_t y, int8_t z);


#endif


