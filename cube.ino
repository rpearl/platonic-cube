#define USE_GET_MILLISECOND_TIMER
#include <FastLED.h>
#include "cube.h"
#include "Vector.h"

extern "C" {
   int _getpid(){ return -1;}
   int _kill(int pid, int sig){ return -1; }
   int _write_r() { return -1; }
}

uint32_t get_millisecond_timer() {
	return millis() / 2.5;
}

//like flicker but in order?
CRGB leds[NUM_LEDS];

typedef void (*SimplePatternList[])();
typedef void (*SimplePattern)();

//#define SINGLE_PATTERN_DEBUG

//TODO: generalize pattern list
SimplePatternList gTransitions = {rainbowSegments, pulse, fillSolid};

SimplePatternList gPatterns = {
	chaseThroughPanels,
	rainbow,
	pulseSegments,
	crawlWithHighlight,
	chaseRainbow,
	flickerSegments,
	chaseSolid,
	sweepTwoSolid,
	sweepTwoRainbow,
};

#define NUM_CUES 7
#define SECONDS_PER_CUE 30

#define TRANSITION_CHANGE_CUE 0
#define PATTERN_CHANGE_CUE 1

#define P(x,y) (x)*LEDS_PER_ROW + y

uint8_t gCurrentCue = 0;

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

// Hacky gross code for effects that understand the physical object

// Format:
// Index 0 - NUM_SEGMENTS
// index 1..NUM_SEGMENTS (inclusive): start index for each segment
// Remaining indices: pixel in panel

const PROGMEM int8_t segments[PANELS][LEDS_PER_PANEL + 11] = {
{ // side 11
8, 10, 18, 37, 50, 59, 68, 70, 72, 74,
P(2,2), P(2,1), P(2,0), P(3,0), P(4,0), P(5,0), P(6,0), P(7,0),
P(3,2), P(3,1), P(4,1), P(5,1), P(6,1), P(7,1), P(7,2), P(7,3), P(7,4), P(7,5), P(7,6), P(6,6), P(5,6), P(4,6), P(3,6), P(2,6), P(1,6), P(1,5), P(0,5),
P(4,2), P(5,2), P(6,2), P(6,3), P(6,4), P(6,5), P(5,5), P(4,5), P(3,5), P(2,5), P(2,4), P(1,4), P(0,4),
P(4,3), P(5,3), P(5,4), P(4,4), P(3,4), P(3,3), P(2,3), P(1,3), P(0,3),
P(7,7), P(6,7), P(5,7), P(4,7), P(3,7), P(2,7), P(1,7), P(0,7), P(0,6),
P(1,0), P(0,0),
P(1,1), P(0,1),
P(1,2), P(0,2),
-1
},
{ // side 14
7, 9, 16, 24, 31, 44, 54, 67, 73,
P(0,2), P(0,1), P(0,0), P(1,0), P(2,0), P(2,1), P(2,2),
P(1,1), P(1,2), P(1,3), P(0,3), P(0,4), P(1,4), P(1,5), P(1,6),
P(0,5), P(0,6), P(0,7), P(1,7), P(2,7), P(2,6), P(2,5),
P(3,0), P(3,1), P(4,1), P(4,0), P(5,0), P(5,1), P(5,2), P(6,2), P(6,1), P(6,0), P(7,0), P(7,1), P(7,2),
P(4,3), P(4,2), P(3,2), P(3,3), P(2,3), P(2,4), P(3,4), P(3,5), P(4,5), P(4,4),
P(3,7), P(3,6), P(4,6), P(4,7), P(5,7), P(5,6), P(5,5), P(6,5), P(6,6), P(6,7), P(7,7), P(7,6), P(7,5),
P(7,3), P(6,3), P(5,3), P(5,4), P(6,4), P(7,4),
-1, -1
},
{ // side 06
9, 11, 26, 32, 39, 45, 51, 57, 63, 71, 75,
P(0,0), P(1,0), P(2,0), P(3,0), P(4,0), P(4,1), P(3,1), P(3,2), P(3,3), P(2,3), P(2,4), P(2,5), P(1,5), P(1,6), P(1,7),
P(0,1), P(1,1), P(2,1), P(2,2), P(1,2), P(0,2),
P(0,3), P(1,3), P(1,4), P(0,4), P(0,5), P(0,6), P(0,7),
P(7,0), P(6,0), P(5,0), P(5,1), P(6,1), P(7,1),
P(6,2), P(5,2), P(4,2), P(4,3), P(5,3), P(6,3),
P(5,4), P(4,4), P(3,4), P(3,5), P(4,5), P(5,5),
P(4,6), P(3,6), P(2,6), P(2,7), P(3,7), P(4,7),
P(7,2), P(7,3), P(7,4), P(6,4), P(6,5), P(6,6), P(5,6), P(5,7),
P(7,5), P(7,6), P(7,7), P(6,7),

},
{ // side 13
8, 10, 17, 21, 25, 43, 47, 51, 58, 74,
P(0,0), P(0,1), P(0,2), P(1,2), P(1,1), P(1,0), P(2,0),
P(2,1), P(3,1), P(3,2), P(2,2),
P(4,1), P(5,1), P(5,2), P(4,2),
P(6,1), P(6,2), P(6,3), P(5,3), P(4,3), P(3,3), P(2,3), P(1,3), P(0,3), P(0,4), P(1,4), P(2,4), P(3,4), P(4,4), P(5,4), P(6,4), P(6,5), P(6,6),
P(4,5), P(5,5), P(5,6), P(4,6),
P(2,5), P(3,5), P(3,6), P(2,6),
P(0,7), P(0,6), P(0,5), P(1,5), P(1,6), P(1,7), P(2,7),
P(3,7), P(4,7), P(5,7), P(6,7), P(7,7), P(7,6), P(7,5), P(7,4), P(7,3), P(7,2), P(7,1), P(7,0), P(6,0), P(5,0), P(4,0), P(3,0),
-1
},
{ // side 08
6, 8, 16, 31, 38, 53, 61, 68,
P(5,6), P(5,7), P(6,7), P(6,6), P(7,6), P(7,5), P(6,5), P(5,5),
P(2,5), P(3,5), P(3,6), P(3,7), P(4,7), P(4,6), P(4,5), P(4,4), P(5,4), P(6,4), P(7,4), P(7,3), P(6,3), P(5,3), P(4,3),
P(6,2), P(7,2), P(7,1), P(6,1), P(6,0), P(5,0), P(5,1),
P(5,2), P(4,2), P(4,1), P(4,0), P(3,0), P(3,1), P(3,2), P(3,3), P(2,3), P(1,3), P(0,3), P(0,4), P(1,4), P(2,4), P(3,4),
P(2,2), P(1,2), P(0,2), P(0,1), P(1,1), P(1,0), P(2,0), P(2,1),
P(2,6), P(2,7), P(1,7), P(1,6), P(0,6), P(0,5), P(1,5),
-1, -1, -1, -1, -1, -1, -1
},
{ // side 04
7, 9, 27, 35, 47, 53, 61, 65, 73,
P(5,0), P(4,0), P(3,0), P(2,0), P(1,0), P(0,0), P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,7), P(2,7), P(3,7), P(4,7), P(5,7),
P(7,1), P(7,0), P(6,0), P(6,1), P(5,1), P(4,1), P(3,1), P(2,1),
P(1,1), P(1,2), P(2,2), P(3,2), P(4,2), P(4,3), P(4,4), P(4,5), P(3,5), P(2,5), P(1,5), P(1,6),
P(1,3), P(2,3), P(3,3), P(3,4), P(2,4), P(1,4),
P(7,2), P(6,2), P(5,2), P(5,3), P(5,4), P(5,5), P(6,5), P(7,5),
P(7,3), P(6,3), P(6,4), P(7,4),
P(7,6), P(7,7), P(6,7), P(6,6), P(5,6), P(4,6), P(3,6), P(2,6),
-1, -1
},
};

uint8_t gHue = 0;

unsigned int bitOut(void)
{
	static unsigned long firstTime=1, prev=0;
	unsigned long bit1=0, bit0=0, x=0, port=0, limit=99;
	if (firstTime)
	{
		firstTime=0;
		prev=analogRead(port);
	}
	while (limit--)
	{
		x=analogRead(port);
		bit1=(prev!=x?1:0);
		prev=x;
		x=analogRead(port);
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
		seed = (seed<<1) | bitOut();
	return seed;
}

void setup() {
	random16_set_seed(seedOut(16));
	nextCue();
	FastLED.addLeds<WS2811_PORTD, PANELS>(leds, LEDS_PER_PANEL);
	FastLED.setMaxRefreshRate(800);
	FastLED.setCorrection(TypicalSMD5050);
	FastLED.setBrightness(40);
}

enum { GETTING_DARKER = 0, GETTING_BRIGHTER = 1 };
uint8_t pulseSegmentDirections[(NUM_LEDS+7)/8];
bool getPixelDirection(uint16_t i) {
	uint16_t index = i / 8;
	uint8_t  bitNum = i & 0x07;
	// using Arduino 'bitRead' function; expanded code below
	return bitRead(pulseSegmentDirections[index], bitNum);
}
void setPixelDirection(uint16_t i, bool dir) {
	uint16_t index = i / 8;
	uint8_t  bitNum = i & 0x07;
	// using Arduino 'bitWrite' function; expanded code below
	bitWrite( pulseSegmentDirections[index], bitNum, dir);
}


CRGB makeBrighter( const CRGB& color, fract8 howMuchBrighter) {
	CRGB incrementalColor = color;
	incrementalColor.nscale8( howMuchBrighter);
	return color + incrementalColor;
}

CRGB makeDarker( const CRGB& color, fract8 howMuchDarker) {
	CRGB newcolor = color;
	newcolor.nscale8( 255 - howMuchDarker);
	return newcolor;
}

void brightenOrDarkenEachPixel( fract8 fadeUpAmount, fract8 fadeDownAmount) {
	for( uint16_t i = 0; i < NUM_LEDS; i++) {
		if( getPixelDirection(i) == GETTING_DARKER) {
			// This pixel is getting darker
			leds[i] = makeDarker( leds[i], fadeDownAmount);
		} else {
			// This pixel is getting brighter
			leds[i] = makeBrighter( leds[i], fadeUpAmount);
			// now check to see if we've maxxed out the brightness
			if( leds[i].r == 255 || leds[i].g == 255 || leds[i].b == 255) {
				// if so, turn around and start getting darker
				setPixelDirection(i, GETTING_DARKER);
			}
		}
	}
}
#define STARTING_BRIGHTNESS 64
#define FADE_IN_SPEED       28
#define FADE_OUT_SPEED      16

#define DENSITY 12 

void pulseSegments() {
	brightenOrDarkenEachPixel(FADE_IN_SPEED, FADE_OUT_SPEED);

	for (uint8_t panel = 0; panel < PANELS; panel++) {
		uint8_t num_segs = NUM_SEGS(panel);

		if (random8() < DENSITY) {
			uint8_t seg = random16(num_segs);
			FOREACH_IN_SEGMENT(panel, seg, idx) {
				uint16_t i = SEG_LED(panel, idx);
				if (   leds[i].r < (STARTING_BRIGHTNESS/2)
  				    || leds[i].g < (STARTING_BRIGHTNESS/2)
				    || leds[i].b < (STARTING_BRIGHTNESS/2)) {
					leds[i] = CHSV(OFFSET_HUE(panel, gHue), 255, STARTING_BRIGHTNESS);
					setPixelDirection(i, GETTING_BRIGHTER);
				}
			}
		}
	}
}

void crawlWithHighlight() {
	uint8_t offset = 5*gHue;
	uint16_t pos = beatsin16(BPM, 0, LEDS_PER_PANEL);
	fadeToBlackBy(leds, NUM_LEDS, 25);
	for (uint8_t panel = 0; panel < PANELS; panel++) {
		uint8_t hue = OFFSET_HUE(panel, offset);
		uint8_t num_segs = NUM_SEGS(panel);

		uint16_t offset_pos = pos + num_segs+2;

		uint16_t seg;
		for (seg = 0; seg < num_segs; seg++) {
			if (offset_pos <= SEG_LAST(panel, seg))
				break;
		}

		FOREACH_IN_SEGMENT(panel, seg, idx) {
			uint8_t bright = 192;
			if (offset_pos == idx)
				bright = 255;
			CHSV c(OFFSET_HUE(panel, hue), 255, bright);
			leds[SEG_LED(panel, idx)] = c;
			hue += 15;
		}
	}
}

void chaseThroughPanels() {
	uint8_t panel_map[][3] = { {1,2,3} , {0,4,5} };

	uint8_t point = beatsin16(3, 0, LEDS_PER_PANEL*3);

	uint8_t pos = point % LEDS_PER_PANEL;
	uint8_t panel_point = point / LEDS_PER_PANEL;
  fadeToBlackBy( leds, NUM_LEDS, 1);
	for (uint8_t panel_idx = 0; panel_idx < 2; panel_idx++) {
	  uint8_t panel = panel_map[panel_idx][panel_point];
    uint8_t num_segs = NUM_SEGS(panel);
		uint16_t offset_pos = pos + num_segs+2;

	  uint16_t pixel = SEG_LED(panel, offset_pos);
	  leds[pixel] = CHSV(gHue * 5, 255, 255);
  }
}


void rainbow() {
	uint8_t offset = 5*gHue;
	int16_t pos = beatsin16(BPM, 0, LEDS_PER_PANEL);
	int16_t direction = beatsin16(BPM, -LEDS_PER_PANEL, LEDS_PER_PANEL, 0, 16384);

	for (uint8_t panel = 0; panel < PANELS; panel++) {
		uint8_t hue = OFFSET_HUE(panel, offset);
		uint8_t num_segs = NUM_SEGS(panel);

		uint16_t offset_pos = pos + num_segs+2;

		uint16_t seg;
		uint8_t bright = 0;

		uint16_t chosen_seg = -1;
		for (seg = 0; seg < num_segs; seg++) {
			if (offset_pos <= SEG_LAST(panel, seg) && chosen_seg == -1)
				break;
		}

		FOREACH_IN_SEGMENT(panel, seg, idx) {
			uint8_t bright = 0;

			if (direction >= 0 && offset_pos > idx) {
				bright = 192;
			} else if (direction <= 0 && offset_pos < idx) {
				bright = 192;
			}

			if (offset_pos == idx) {
				bright = 255;
			}

			if (bright) {
				leds[SEG_LED(panel, idx)] = CHSV(OFFSET_HUE(panel, hue), 255, bright);
			}
			hue += 15;
		}
	}
}

void rainbowSegments() {
	uint8_t offset = gHue;

	for (uint8_t panel = 0; panel < PANELS; panel++) {
		uint8_t hue = OFFSET_HUE(panel, offset);
		uint8_t num_segs = NUM_SEGS(panel);

		for (int seg = 0; seg < num_segs; seg++) {
			FOREACH_IN_SEGMENT(panel, seg, idx) {
				leds[SEG_LED(panel, idx)].setHue(hue);
			}
			hue += 15;
		}
	}
}

void chase(uint8_t hueDelta) {
	uint8_t hue = gHue;
	uint16_t pos = beatsin16(BPM, 0, LEDS_PER_PANEL-1);
 	fadeToBlackBy( leds, NUM_LEDS, 40);

	for (uint8_t panel = 0; panel < PANELS; panel++) {
		uint16_t idx = SEG_LED(panel, pos+NUM_SEGS(panel)+2);
		leds[idx] += CHSV(OFFSET_HUE(panel, hue), 255, 255);

		hue += hueDelta;
	}
}

void chaseSolid() {
	chase(0);
}

void chaseRainbow() {
	chase(15);
}

void sweepTwo(bool solid) { // sweep simultaneously across two Us
	//1,2,3 5,4,0

 	fadeToBlackBy( leds, NUM_LEDS, 40);

 	uint8_t hue = gHue;

	uint8_t panel_map[][3] = { {1,2,3} , {0,4,5} };
	uint8_t flip[][3] = { {1,1,1} , {0,0,1} };

	uint8_t row_position = beatsin16(15, 0, ROWS_PER_PANEL * 3);
	uint8_t row = row_position % ROWS_PER_PANEL;

	uint8_t panel_point = row_position / ROWS_PER_PANEL;

	for (uint8_t panel_idx = 0; panel_idx < 2; panel_idx++) {
		uint8_t panel = panel_map[panel_idx][panel_point];

		uint16_t pixel, delta;

		if (!flip[panel_idx][panel_point]) {
			pixel = PIXEL_IN_PANEL(panel, row*LEDS_PER_ROW);
			delta = 1;
		} else {
			pixel = PIXEL_IN_PANEL(panel, row);
			delta = 8;
		}

		uint8_t count = 0;

		while (count < LEDS_PER_ROW) {
			leds[pixel] = CHSV(hue + 128*panel_idx, 255, 255);
			pixel += delta;
			count++;
			if (!solid) {
				hue += 10;
			}
		}
	}
}

void sweepTwoSolid() {
	sweepTwo(true);
}

void sweepTwoRainbow() {
	sweepTwo(false);
}

void pulse() {
	uint8_t beat = beatsin16(12, 64, 255);
	uint8_t offset = 5*gHue;

	for (uint8_t panel = 0; panel < PANELS; panel++) {
		uint8_t hue = OFFSET_HUE(panel, offset);
		uint8_t num_segs = NUM_SEGS(panel);

		for (int seg = 0; seg < num_segs; seg++) {
			FOREACH_IN_SEGMENT(panel, seg, idx) {
				CHSV c(OFFSET_HUE(panel, hue), 255, beat);
				leds[SEG_LED(panel, idx)] = c;
				hue += 15;
			}
		}
	}
}

void flickerSegments() {
	uint8_t offset = gHue;

	if (random16(2)) {
		fadeToBlackBy(leds, NUM_LEDS, 25);
		return;
	}


	for (uint8_t i = 0; i < PANELS/3; i++) {

		uint8_t panel = random16(PANELS);

		uint8_t hue = OFFSET_HUE(panel, offset);
		uint8_t num_segs = NUM_SEGS(panel);

		int seg = random16(num_segs);

		FOREACH_IN_SEGMENT(panel, seg, idx) {
			leds[SEG_LED(panel, idx)].setHue(hue);
			hue += 15;
		}
	}
 	fadeToBlackBy( leds, NUM_LEDS, 25);
}

void fillSolid() {
	uint8_t bright = beatsin16(BPM, 128, 255);
	for (uint8_t panel = 0; panel < PANELS; panel++) {
		fill_solid(
			&leds[PIXEL_IN_PANEL(panel, 0)],
			LEDS_PER_PANEL,
			CHSV(OFFSET_HUE(panel, gHue), 255, bright)
		);
	}
}

void nextCue() {
#ifndef SINGLE_PATTERN_DEBUG
	gCurrentCue = (gCurrentCue + 1) % NUM_CUES;

	if (gCurrentCue == TRANSITION_CHANGE_CUE) {
		gCurrentPatternNumber = random16(ARRAY_SIZE(gTransitions));
	} else if (gCurrentCue == PATTERN_CHANGE_CUE) {
		gCurrentPatternNumber = random16(ARRAY_SIZE(gPatterns));
	}
#else
	gCurrentCue = PATTERN_CHANGE_CUE;
	gCurrentPatternNumber = 0;
#endif
}

void showCurrentPattern() {
	uint64_t start = get_millisecond_timer();
	if (gCurrentCue == TRANSITION_CHANGE_CUE) {
		gTransitions[gCurrentPatternNumber]();
	} else {
		gPatterns[gCurrentPatternNumber]();
	}
	FastLED.show();
	uint64_t end = get_millisecond_timer();
	uint64_t duration = 0;
	if (end > start) {
		duration = end - start;
	}

	FastLED.delay(WAIT-duration);
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

void sweepPlane() {
	fadeToBlackBy(leds, NUM_LEDS, 50);
	uint8_t z = beatsin16(15, 0, LEDS_PER_ROW);

	uint8_t hue = gHue;

	for (uint8_t x = 0; x < LEDS_PER_ROW; x++) {
		for (uint8_t y = 0; y < LEDS_PER_ROW; y++) {
			if (x == 0 || x == LEDS_PER_ROW-1 || y == 0 || y == LEDS_PER_ROW-1) {
				setPixel3d(x,y,z, CHSV(hue, 255, 255));
				setPixel3d(x,y,z, CHSV(hue, 255, 255));
				setPixel3d(x,y,LEDS_PER_ROW-z-1, CHSV(hue+128, 255, 255));

				uint8_t x1 = z;
				uint8_t y1 = x;
				uint8_t z1 = y;

				setPixel3d(x1,y1,z1, CHSV(hue, 255, 255));
				setPixel3d(LEDS_PER_ROW-x1-1, y1, z1, CHSV(hue+128, 255, 255));

/*
				uint8_t y2 = z;
				uint8_t x2 = y;
				uint8_t z2 = x;
				setPixel3d(x2,y2,z2, CHSV(hue, 255, 255));
				setPixel3d(x2,LEDS_PER_ROW-y2-1,z2, CHSV(hue+128, 255, 255));
*/
				hue+=5;
			}
		}
	}
}

void sweepOnePlane() {
	fadeToBlackBy(leds, NUM_LEDS, 75);
	uint8_t base = scale16(beat16(20), LEDS_PER_ROW+8);


	for (uint8_t i = 0; i < LEDS_PER_ROW+8; i+= 6) {
		shootRing((base + i) % (LEDS_PER_ROW+8), gHue+(30*i));
	}
}

void shootRing(uint8_t z, uint8_t hue) {
	if (z < 4) {
		uint8_t width = z*2;
		for (uint8_t x = 0; x < width; x++) {
			for (uint8_t y = 0; y < width; y++) {
				if (x == 0 || x == width-1 || y == 0 || y == width-1) {
					uint8_t start = 4-z;
					setPixel3d(start+x, start+y, 0, CHSV(hue, 255, 255));
					hue += 5;
				}
			}
		}
	} else if (z >= LEDS_PER_ROW + 4) {
		z = (LEDS_PER_ROW+8) - z - 1;
		uint8_t width = z*2;
		for (uint8_t x = 0; x < width; x++) {
			for (uint8_t y = 0; y < width; y++) {
				if (x == 0 || x == width-1 || y == 0 || y == width-1) {
					uint8_t start = 4-z;
					setPixel3d(start+x, start+y, LEDS_PER_ROW-1, CHSV(hue, 255, 255));
					hue += 5;
				}
			}
		}
	} else {
		z -= 4;
		for (uint8_t x = 0; x < LEDS_PER_ROW; x++) {
			for (uint8_t y = 0; y < LEDS_PER_ROW; y++) {
				if (x == 0 || x == LEDS_PER_ROW-1 || y == 0 || y == LEDS_PER_ROW-1) {
					setPixel3d(x,y,z, CHSV(hue, 255, 255));
					hue += 5;
				}
			}
		}
	}
}

void loop() {
	showCurrentPattern();

	EVERY_N_MILLISECONDS(20) {
		gHue++;
	}

	EVERY_N_SECONDS(SECONDS_PER_CUE) {
		nextCue();
	}
}
