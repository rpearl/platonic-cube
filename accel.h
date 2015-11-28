/* vim: set ts=8 sts=4 et sw=4 tw=99: */
#ifndef _ACCEL_H_
#define _ACCEL_H_
#include "vector.h"

void initAccelerometer();
void sampleAccelerometer();
Vector3f accelerometerDirection();

#endif
