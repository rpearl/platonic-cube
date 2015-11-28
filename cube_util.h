/* vim: set ts=8 sts=4 et sw=4 tw=99: */
#ifndef _UTIL_H_
#define _UTIL_H_

#include "cube.h"
#include "Vector.h"

typedef std::tuple<int8_t, int8_t, int8_t> coord_t;

//Randomness
uint64_t seedOut(uint8_t noOfBits);

//3d coordinate system
extern Vector3f normals[PANELS];

void setPixel3d(int8_t x, int8_t y, int8_t z, const CRGB &c);
int16_t getPixel3d(int8_t x, int8_t y, int8_t z);
coord_t get3dCoord(int8_t panel, int8_t i, int8_t j);
coord_t getCorrectedCoord(Vector3f &gravity, int8_t x, int8_t y, int8_t z);
void setPixel3dCompensated(const Vector3f &gravity, int8_t x, int8_t y, int8_t z, const CRGB &c);
int16_t getPixel3dCompensated(const Vector3f &gravity, int8_t x, int8_t y, int8_t z);


#endif


