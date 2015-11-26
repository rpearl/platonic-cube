/* vim: set ts=8 sts=4 et sw=4 tw=99: */
#ifndef _UTIL_H_
#define _UTIL_H_

#include "cube.h"
#include "Vector.h"

//Randomness
uint64_t seedOut(uint8_t noOfBits);

//3d coordinate system
extern Vector3f normals[PANELS];

void setPixel3d(uint8_t x, uint8_t y, uint8_t z, const CRGB &c);
std::tuple<uint8_t, uint8_t, uint8_t> get3dCoord(uint8_t panel, uint8_t i, uint8_t j);

#endif


