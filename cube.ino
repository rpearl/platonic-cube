#include <FastLED.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

#define LEDS_PER_ROW 8
#define ROWS_PER_PANEL 8

#define LEDS_PER_PANEL (LEDS_PER_ROW * ROWS_PER_PANEL)
#define PANELS 6
#define DATA_PIN 6

#define PATTERN_IDX 1

#define NUM_LEDS (LEDS_PER_PANEL * PANELS)
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

CRGB leds[NUM_LEDS];

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {
	chaseRainbow,
	flickerSegments,
	pulse,
	chaseSolid,
	sweepTwoSolid,
	rainbowSegments,
	sweepTwoRainbow,
};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

const PROGMEM uint8_t segments[PANELS][LEDS_PER_PANEL + 11] = {
  {
    8,  10,  18,  37,  50,  59,  68,  70,  72,  74,
   45,  46,  47,  39,  31,  23,  15,   7,
   37,  38,  30,  22,  14,   6,   5,   4,   3,   2,   1,   9,  17,  25,  33,  41,  49,  50,  58,
   29,  21,  13,  12,  11,  10,  18,  26,  34,  42,  43,  51,  59,
   28,  20,  19,  27,  35,  36,  44,  52,  60,
    0,   8,  16,  24,  32,  40,  48,  56,  57,
   55,  63,
   54,  62,
   53,  61,
   -1,
  },
  {
    7,   9,  16,  23,  30,  43,  53,  66,  72,
    2,   1,   0,   8,  16,  17,  18,
    9,  10,  11,   3,   4,  12,  14,
    5,   6,   7,  15,  23,  22,  21,
   24,  25,  33,  32,  40,  41,  42,  50,  49,  48,  56,  57,  58,
   35,  34,  26,  27,  19,  20,  28,  29,  37,  36,
   31,  30,  38,  39,  47,  46,  45,  53,  54,  55,  63,  62,  61,
   59,  51,  43,  44,  52,  60,
   -1,  -1,  -1,
  },
  {
    9,  11,  26,  32,  39,  45,  51,  57,  63,  71,  75,
    0,   8,  16,  24,  32,  33,  25,  26,  27,  19,  20,  21,  13,  14,  15,
    1,   9,  17,  18,  10,   2,
    3,  11,  12,   4,   5,   6,   7,
   56,  48,  40,  41,  49,  57,
   50,  42,  34,  35,  43,  51,
   44,  36,  28,  29,  37,  45,
   38,  30,  22,  23,  31,  39,
   58,  59,  60,  52,  53,  54,  46,  47,
   61,  62,  63,  55,

  },
  {
    8,  10,  17,  21,  25,  43,  47,  51,  58,  74,
    0,   1,   2,  10,   9,   8,  16,
   17,  25,  26,  18,
   33,  41,  42,  34,
   49,  50,  51,  43,  35,  27,  19,  11,   3,   4,  12,  20,  28,  36,  44,  52,  53,  54,
   37,  45,  46,  38,
   21,  29,  30,  22,
    7,   6,   5,  13,  14,  15,  23,
   31,  39,  47,  55,  63,  62,  61,  60,  59,  58,  57,  56,  48,  40,  32,  24,
   -1,
  },
  {
    6,   8,  16,  31,  38,  53,  61,  68,
   17,  16,   8,   9,   1,   2,  10,  18,
   42,  34,  33,  32,  24,  25,  26,  27,  19,  11,   3,   4,  12,  20,  28,
   13,   5,   6,  14,  15,  23,  22,
   21,  29,  30,  31,  39,  38,  37,  36,  44,  52,  60,  59,  51,  43,  35,
   45,  53,  61,  62,  54,  55,  47,  46,
   41,  40,  48,  49,  57,  58,  50,
   -1,  -1,  -1,  -1,  -1,  -1,  -1,
  },
  {
    7,   9,  27,  35,  47,  53,  61,  65,  73,
   40,  32,  24,  16,   8,   0,   1,   2,   3,   4,   5,   6,   7,  15,  23,  31,  39,  47,
   57,  56,  48,  49,  41,  33,  25,  17,
    9,  10,  18,  26,  34,  35,  36,  37,  29,  21,  13,  14,
   11,  19,  27,  28,  20,  12,
   58,  50,  42,  43,  44,  45,  53,  61,
   59,  51,  52,  60,
   62,  63,  55,  54,  46,  38,  30,  22,
   -1,  -1,
  },

};

#define NUM_SEGS(panel) (pgm_read_byte(&segments[panel][0]))

#define SEG_FIRST(panel, seg) (pgm_read_byte(&segments[panel][(seg)+1]))
#define SEG_LAST(panel, seg) (pgm_read_byte(&segments[panel][(seg)+2]))

#define PIXEL_IN_PANEL(panel, pixel) (((panel)*LEDS_PER_PANEL) + (pixel))

#define SEG_LED(panel, idx) PIXEL_IN_PANEL(panel, pgm_read_byte(&segments[panel][idx]))

#define FOREACH_IN_SEGMENT(panel, seg, idx) \
	for (uint8_t idx = SEG_FIRST(panel, seg); idx < SEG_LAST(panel, seg); idx++)

#define WAIT 30

#define BPM  8
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void setup() {
	nextPattern();
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
	FastLED.setCorrection(TypicalSMD5050);
	FastLED.setBrightness(40);
	//gCurrentPatternNumber = EEPROM.read(PATTERN_IDX);
	//if (gCurrentPatternNumber >= ARRAY_SIZE(gPatterns)) {
	//	gCurrentPatternNumber = 0;
	//}

	//EEPROM.write(PATTERN_IDX, (gCurrentPatternNumber+1) % ARRAY_SIZE(gPatterns));

}

void rainbowSegments() {
	uint8_t offset = gHue;

	for (uint8_t panel = 0; panel < PANELS; panel++) {
		uint8_t hue = 42*panel + offset;
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
		leds[idx] += CHSV(hue + 42*panel, 255, 255);

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
		uint8_t hue = 42*panel + offset;
		uint8_t num_segs = NUM_SEGS(panel);

		for (int seg = 0; seg < num_segs; seg++) {
			FOREACH_IN_SEGMENT(panel, seg, idx) {
				CHSV c(hue + panel*42, 255, beat);
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

		uint8_t hue = 42*panel + offset;
		uint8_t num_segs = NUM_SEGS(panel);

		int seg = random16(num_segs);

		FOREACH_IN_SEGMENT(panel, seg, idx) {
			leds[SEG_LED(panel, idx)].setHue(hue);
			if (idx % 3 == 0) {
				FastLED.show();
			}
			hue += 15;
		}
	}
 	fadeToBlackBy( leds, NUM_LEDS, 25);
}

void nextPattern() {
	gCurrentPatternNumber = random16(ARRAY_SIZE(gPatterns));
}

void loop() {
	//flickerSegments();
	//pulse();
	//chase();
	gPatterns[gCurrentPatternNumber]();
	FastLED.show();
	FastLED.delay(WAIT);

	EVERY_N_MILLISECONDS( 20 ) { gHue++; }

	EVERY_N_SECONDS( 60*5 ) { nextPattern(); }

}
