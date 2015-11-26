#include <tuple>
#include <FastLED.h>
#include "cube.h"

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

void setPixel3d(uint8_t x, uint8_t y, uint8_t z, const CRGB &c) {
	if (x == 0) {
		leds[PIXEL_IN_PANEL(4, P(FLIP(y), z))] = c;
	} else if (x == LEDS_PER_ROW-1) { // panel 2
		leds[PIXEL_IN_PANEL(2, P(FLIP(y), FLIP(z)))] = c;
	}

	if (y == 0) {
		leds[PIXEL_IN_PANEL(5, P(FLIP(x), FLIP(z)))] = c;
	} else if (y == LEDS_PER_ROW-1) {
		leds[PIXEL_IN_PANEL(0, P(x, FLIP(z)))] = c;
	}

	if (z == 0) {
		leds[PIXEL_IN_PANEL(3, P(FLIP(y), FLIP(x)))] = c;
	} else if (z == LEDS_PER_ROW-1) {
		leds[PIXEL_IN_PANEL(1, P(FLIP(y), x))] = c;
	}
}

std::tuple<uint8_t, uint8_t, uint8_t> get3dCoord(uint8_t panel, uint8_t i, uint8_t j) {
	uint8_t x, y, z;
	switch (panel) {
		case 0:
			y = LEDS_PER_ROW-1;
			x = i;
			z = FLIP(j);
			break;
		case 1:
			z = LEDS_PER_ROW-1;
			y = FLIP(i);
			x = j;
			break;
		case 2:
			x = LEDS_PER_ROW-1;
			y = FLIP(i);
			z = FLIP(j);
			break;
		case 3:
			z = 0;
			y = FLIP(i);
			x = FLIP(j);
			break;
		case 4:
			x = 0;
			y = FLIP(i);
			z = j;
			break;
		case 5:
			y = 0;
			x = FLIP(i);
			z = FLIP(j);
			break;
	}
	return std::make_tuple(x,y,z);
}
