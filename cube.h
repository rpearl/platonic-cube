/* vim: set ts=8 sts=4 et sw=4 tw=99: */
#ifndef _CUBE_H_
#define _CUBE_H_

#include <FastLED.h>

#include <avr/pgmspace.h>

#define LEDS_PER_ROW 8
#define ROWS_PER_PANEL 8

#define LEDS_PER_PANEL (LEDS_PER_ROW * ROWS_PER_PANEL)
#define PANELS 6

#define FLIP(coord) (LEDS_PER_ROW - ( (coord) + 1 ))

#define NUM_SEGS(panel) (pgm_read_byte(&segments[panel][0]))

#define SEG_FIRST(panel, seg) (pgm_read_byte(&segments[panel][(seg)+1]))
#define SEG_LAST(panel, seg) (pgm_read_byte(&segments[panel][(seg)+2]))

#define PIXEL_IN_PANEL(panel, pixel) (((panel)*LEDS_PER_PANEL) + (pixel))

#define SEG_LED(panel, idx) PIXEL_IN_PANEL(panel, pgm_read_byte(&segments[panel][idx]))

#define FOREACH_IN_SEGMENT(panel, seg, idx) \
    for (uint8_t idx = SEG_FIRST(panel, seg); idx < SEG_LAST(panel, seg); idx++)
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define WAIT 30


#define OFFSET_HUE(panel, hue) ( (hue) + (256/PANELS)*panel )

#define BPM  8

#define NUM_LEDS (LEDS_PER_PANEL * PANELS)

extern CRGB leds[NUM_LEDS];
extern uint8_t gHue;

void init_liquid();
#endif
