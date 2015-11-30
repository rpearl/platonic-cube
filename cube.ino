/* vim: set ts=8 sts=4 et sw=4 tw=99: */
#define USE_GET_MILLISECOND_TIMER
#include <FastLED.h>
#include "cube.h"
#include "vector3.h"
#include "cube_util.h"
#include "accel.h"
#include "pattern.h"

extern "C" {
    int _getpid(){ return -1;}
    int _kill(int pid, int sig){ return -1; }
    int _write_r() { return -1; }
    void __cxa_pure_virtual() { abort(); }
}

namespace __gnu_cxx {
    void __verbose_terminate_handler() {
        abort();
    }
}

uint32_t get_millisecond_timer() {
    return millis() / 2.5;
}

CRGB leds[NUM_LEDS];

#define SINGLE_PATTERN_DEBUG


// Hacky gross code for effects that understand the physical object

// Format:
// Index 0 - NUM_SEGMENTS
// index 1..NUM_SEGMENTS (inclusive): start index for each segment
// Remaining indices: pixel in panel

#define P(x,y) (x)*LEDS_PER_ROW + y
static const PROGMEM int8_t segments[PANELS][LEDS_PER_PANEL + 11] = {
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
        6, 8, 15, 30, 37, 52, 60, 68,
        P(2,1), P(2,0), P(1,0), P(1,1), P(0,1), P(0,2), P(1,2),
        P(2,2), P(3,2), P(3,1), P(3,0), P(4,0), P(4,1), P(4,2), P(4,3), P(5,3), P(6,3), P(7,3), P(7,4), P(6,4), P(5,4), P(4,4),
        P(6,5), P(7,5), P(7,6), P(6,6), P(6,7), P(5,7), P(5,6),
        P(5,5), P(4,5), P(4,6), P(4,7), P(3,7), P(3,6), P(3,5), P(3,4), P(2,4), P(1,4), P(0,4), P(0,3), P(1,3), P(2,3), P(3,3),
        P(5,2), P(6,2), P(7,2), P(7,1), P(6,1), P(6,0), P(5,0), P(5,1),
        P(2,6), P(2,7), P(1,7), P(1,6), P(0,6), P(0,5), P(1,5), P(2,5),
        -1,-1,-1,-1,-1,-1,-1,
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

class Rain : public Pattern {
    private:
        static const int NUM_DROPLETS = 80;
        /// This class maintains the state and calculates the animations to render a falling water droplet
        /// Objects of this class can have three states:
        ///    - inactive: this object does nothing
        ///    - swelling: the droplet is at the top of the led strip and swells in intensity
        ///    - falling: the droplet falls downwards and accelerates
        ///    - bouncing: the droplet has bounced of the ground. A smaller, less intensive droplet bounces up
        ///      while a part of the drop remains on the ground.
        /// After going through the swelling, falling and bouncing phases, the droplet automatically returns to the
        /// inactive state.
        class Droplet {
            public:
                Droplet()
                    : x(0), z(0), color(CRGB::Black), gravity(5),
                    position(0), speed(0), state(inactive)
            {}

                void init(int8_t x, int8_t z) {
                    this->x = x;
                    this->z = z;
                    reinit();
                }

                void reinit() {
                    this->position = 0;
                    this->speed = 0;
                    this->color = CHSV(144+random(32), 255, 255);
                    this->start = get_millisecond_timer()+1000*random8(5);
                    state = swelling;
                }

                /// perform one step and draw.
                void step(CRGB *leds) {
                    if (get_millisecond_timer() >= start) {
                        step();
                        draw(leds);
                    }
                }

            private:
                /// calculate the next step in the animation for this droplet
                void step() {
                    if (state == falling || state == bouncing) {
                        position += speed;
                        speed += gravity;

                        // if we hit the bottom...
                        const uint16_t maxpos16 = (ROWS_PER_PANEL-1) << 8;
                        if (position > maxpos16) {
                            if (state == bouncing) {
                                // this is the second collision,
                                // deactivate.
                                state = inactive;
                                reinit();
                            } else {
                                // reverse direction and dampen the speed
                                position = maxpos16 - (position - maxpos16);
                                speed = -speed/4;
                                color.nscale8_video(collision_scaling);
                                state = bouncing;
                            }
                        }
                    } else if (state == swelling) {
                        ++position;
                        if (color.blue <= 10 || color.blue - position <= 10) {
                            state = falling;
                            position = 0;
                        }
                    }
                }

                /// Draw the droplet on the led string
                /// This will "smear" the light of this droplet between two leds. The closer
                /// the droplets position is to that of a particular led, the brighter that
                /// led will be
                void draw(CRGB *leds) {
                    Vector3f gravity = accelerometerDirection();
                    if (state == falling || state == bouncing) {
                        uint8_t position8 = position >> 8;
                        uint8_t remainder = position; // get the lower bits

                        CRGB tc = color;
                        for (auto lidx : getPixel3dCompensated(gravity, x, position8,z)) {
                            leds[lidx] += tc.nscale8_video(256 - remainder);
                        }

                        if (state == bouncing) {
                            for (auto lidx : getPixel3dCompensated(gravity, x,ROWS_PER_PANEL-1,z)) {
                                leds[lidx] = color;
                            }
                        }
                    } else if (state == swelling) {
                        CRGB tc = color;
                        for (auto lidx : getPixel3dCompensated(gravity,x,0,z)) {
                            leds[lidx] = tc.nscale8_video(position);
                        }
                    }
                }

                // how much of a color is left when colliding with the floor, value
                // between 0 and 256 where 256 means no loss.
                static const uint16_t collision_scaling = 40;
                int8_t x, z;
                CRGB color;
                uint16_t gravity;
                uint16_t position;
                int16_t  speed;
                enum stateval {
                    inactive,
                    swelling,
                    falling,
                    bouncing
                };

                stateval state;
                unsigned long start;
        };
        Droplet *droplets;
    public:
        void setup() {
            droplets = new Droplet[NUM_DROPLETS];
            uint8_t idx = 0;
            while (true) {
                for (int8_t x = -1; x < LEDS_PER_ROW+1; x+=2) {
                    for (int8_t z = -1; z < LEDS_PER_ROW+1; z+=2) {
                        if (abs(x) <= 1 || abs(x-LEDS_PER_ROW) <= 1 ||
                            abs(z) <= 1 || abs(z-LEDS_PER_ROW) <= 1) {
                            droplets[idx++].init(x,z);

                            if (idx == NUM_DROPLETS)
                                return;
                        }
                    }
                }
            }
        }

        void show() override {
            fill_solid(leds, NUM_LEDS, CRGB::Black);
            for (uint8_t idx = 0; idx < NUM_DROPLETS; ++idx) {
                droplets[idx].step(leds);
            }
        }

        void teardown() {
            delete[] droplets;
        }
};
void setup() {
    initAccelerometer();
    random16_set_seed(seedOut(16));
    nextCue();
    FastLED.addLeds<WS2811_PORTD, PANELS>(leds, LEDS_PER_PANEL);
    FastLED.setMaxRefreshRate(800);
    FastLED.setCorrection(TypicalSMD5050);
    FastLED.setBrightness(40);
}

class PulseSegments : public Pattern {
    private:
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
        static const uint8_t STARTING_BRIGHTNESS = 64;
        static const uint8_t FADE_IN_SPEED       = 28;
        static const uint8_t FADE_OUT_SPEED      = 16;

        static const uint8_t DENSITY             = 12;

    public:
        void show() override {
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
};

class CrawlWithHighlight : public Pattern {
    public:
        void show() override {
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
};

class ChaseThroughPanels : public Pattern {
    public:
        void show() override {
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
};


class Rainbow : public Pattern {
    public:
        void show() override {
            uint8_t offset = 5*gHue;
            int16_t pos = beatsin16(BPM, 0, LEDS_PER_PANEL);
            int16_t direction = beatsin16(BPM, -LEDS_PER_PANEL, LEDS_PER_PANEL, 0, 16384);

            for (uint8_t panel = 0; panel < PANELS; panel++) {
                uint8_t hue = OFFSET_HUE(panel, offset);
                uint8_t num_segs = NUM_SEGS(panel);

                uint16_t offset_pos = pos + num_segs+2;

                uint16_t seg;

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
};

class RainbowSegments : public Pattern {
    public:
        void show() override {
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
};

static void chase(uint8_t hueDelta) {
    uint8_t hue = gHue;
    uint16_t pos = beatsin16(BPM, 0, LEDS_PER_PANEL-1);
    fadeToBlackBy( leds, NUM_LEDS, 40);

    for (uint8_t panel = 0; panel < PANELS; panel++) {
        uint16_t idx = SEG_LED(panel, pos+NUM_SEGS(panel)+2);
        leds[idx] += CHSV(OFFSET_HUE(panel, hue), 255, 255);

        hue += hueDelta;
    }
}

class ChaseSolid : public Pattern {
    public:
        void show() override {
            chase(0);
        }
};

class ChaseRainbow : public Pattern {
    public:
        void show() override {
            chase(15);
        }
};

static void sweepTwo(bool solid) { // sweep simultaneously across two Us
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

class SweepTwoSolid : public Pattern {
    public:
        void show() override {
            sweepTwo(true);
        }
};

class SweepTwoRainbow : public Pattern {
    public:
        void show() override {
            sweepTwo(false);
        }
};

class Pulse : public Pattern {
    public:
        void show() override {
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
};

class FlickerSegments : public Pattern {
    public:
        void show() override {
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
};

class FillSolid : public Pattern {
    public:
        void show() override {
            uint8_t bright = beatsin16(BPM, 128, 255);
            for (uint8_t panel = 0; panel < PANELS; panel++) {
                fill_solid(
                        &leds[PIXEL_IN_PANEL(panel, 0)],
                        LEDS_PER_PANEL,
                        CHSV(OFFSET_HUE(panel, gHue), 255, bright)
                        );
            }
        }
};


class SweepPlane : public Pattern {
    public:
        void show() override {
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

                        hue+=5;
                    }
                }
            }
        }
};

class MakeWaves : public Pattern {
    private:
        void beginWave(uint8_t z, uint8_t hue, uint8_t bright) {
            uint8_t width = z*2;
            for (uint8_t x = 0; x < width; x++) {
                for (uint8_t y = 0; y < width; y++) {
                    if (x == 0 || x == width-1 || y == 0 || y == width-1) {
                        uint8_t start = 4-z;
                        setPixel3d(start+x, start+y, 0, CHSV(hue, 255, bright));
                        hue += 5;
                    }
                }
            }
        }

        void endWave(uint8_t z, uint8_t hue, uint8_t bright) {
            uint8_t width = z*2;
            for (uint8_t x = 0; x < width; x++) {
                for (uint8_t y = 0; y < width; y++) {
                    if (x == 0 || x == width-1 || y == 0 || y == width-1) {
                        uint8_t start = 4-z;
                        setPixel3d(start+x, start+y, LEDS_PER_ROW-1, CHSV(hue, 255, bright));
                        hue += 5;
                    }
                }
            }
        }

        void midWave(uint8_t z, uint8_t hue, uint8_t bright) {
            for (uint8_t x = 0; x < LEDS_PER_ROW; x++) {
                for (uint8_t y = 0; y < LEDS_PER_ROW; y++) {
                    if (x == 0 || x == LEDS_PER_ROW-1 || y == 0 || y == LEDS_PER_ROW-1) {
                        setPixel3d(x,y,z, CHSV(hue, 255, bright));
                        hue += 5;
                    }
                }
            }
        }

        void makeWave(uint8_t z, uint8_t hue, uint8_t bright) {
            if (z < 4) {
                beginWave(z, hue, bright);
            } else if (z >= LEDS_PER_ROW + 4) {
                endWave((LEDS_PER_ROW+8) - z - 1, hue, bright);
            } else {
                midWave(z-4, hue, bright);
            }
        }
    public:

        void show() override {
            fadeToBlackBy(leds, NUM_LEDS, 25);
            uint16_t base = scale16(beat16(25), (LEDS_PER_ROW+8));

            for (uint8_t i = 0; i < LEDS_PER_ROW+8; i+= 6) {
                makeWave((base + i) % (LEDS_PER_ROW+8), gHue+(30*i), 255);
                makeWave((base + 1+ i) % (LEDS_PER_ROW+8), gHue+(30*i), 128);
                makeWave((base + 2+ i) % (LEDS_PER_ROW+8), gHue+(30*i), 32);
            }
        }
};


//void rain() {
//}
//
//void testPattern() {
//    rain();
//}
//
#define BUFFER 2
template <int8_t Y, int16_t S>
class PlanesWithGravity : public Pattern {
    private:
        uint16_t pos;
        int16_t speed;
        const uint16_t gravity = 3;
    public:
        void setup() {
            this->pos = (Y + BUFFER) << 8;
            this->speed = S;
        }
        void show() override {
            fadeToBlackBy(leds, NUM_LEDS, 25);
            Vector3f accelDir = accelerometerDirection();
            //TODO Falling with gravity.
            //
            pos += speed;
            speed += gravity;

            int8_t y = pos >> 8;
            y -= BUFFER;


            for (int8_t x = -BUFFER; x < LEDS_PER_ROW+BUFFER; x++) {
                for (int8_t z = -BUFFER; z < LEDS_PER_ROW+BUFFER; z++) {
                    setPixel3dCompensated(accelDir, x, y, z, CHSV(gHue,255,255));
                }
            }
            if (y >= LEDS_PER_ROW + (BUFFER*2)) {
                setup();
            }
        }
};


typedef PlanesWithGravity<-BUFFER, 0> FallingPlanes;
typedef PlanesWithGravity<LEDS_PER_ROW + BUFFER-1, -130> LaunchingPlanes;

// pattern ideas:
// rainbowify segments with bright crawlers
//
// rainbow from top to bottom gravity-compensated
//
//
class RainbowGravity : public Pattern {
    public:
        void show() override {
            fill_solid(leds, NUM_LEDS, CRGB::Black);
            uint8_t hue = gHue;
            Vector3f accelDir = accelerometerDirection();
            for (int8_t y = -3; y < LEDS_PER_ROW+3; y++) {
                for (int8_t x = -3; x < LEDS_PER_ROW+3; x++) {
                    for (int8_t z = -3; z < LEDS_PER_ROW+3; z++) {
                        setPixel3dCompensated(accelDir, x, y, z, CHSV(hue,255,255));
                    }
                }
                hue+=15;
            }
        }
};

//TODO: generalize pattern list
static Pattern * const gTransitions[] = {
    new RainbowSegments(),
    new Pulse(),
    new FillSolid()
};
static Pattern * const gPatterns[] = {
    new RainbowGravity(),
    new LaunchingPlanes(),
    new FallingPlanes(),
    new Rain(),
    new MakeWaves(),
    new ChaseThroughPanels(),
    new Rainbow(),
    new PulseSegments(),
    new CrawlWithHighlight(),
    new ChaseRainbow(),
    new FlickerSegments(),
    new ChaseSolid(),
    new SweepTwoSolid(),
    new SweepTwoRainbow(),
};

#define NUM_CUES 7
#define SECONDS_PER_CUE 30

#define TRANSITION_CHANGE_CUE 0
#define PATTERN_CHANGE_CUE 1

#define RANDOM_FROM_ARRAY(arr) arr[random16(ARRAY_SIZE(arr))]


static uint8_t gCurrentCue = 0;

static Pattern *currentPattern = NULL;

void nextCue() {
#ifndef SINGLE_PATTERN_DEBUG
    gCurrentCue = (gCurrentCue + 1) % NUM_CUES;
    Pattern *oldPattern = currentPattern;

    if (gCurrentCue == TRANSITION_CHANGE_CUE) {
        currentPattern = RANDOM_FROM_ARRAY(gTransitions);
    } else if (gCurrentCue == PATTERN_CHANGE_CUE) {
        currentPattern = RANDOM_FROM_ARRAY(gPatterns);
    }

    if (oldPattern != currentPattern) {
        if (oldPattern)
            oldPattern->teardown();
        currentPattern->setup();
    }
#else
    (void)gCurrentCue;
    if (!currentPattern) {
        currentPattern = gPatterns[0];
        currentPattern->setup();
    }
#endif

}

void showCurrentPattern() {
    int64_t start = get_millisecond_timer();
    currentPattern->show();
    FastLED.show();
    int64_t end = get_millisecond_timer();
    int64_t duration = 0;
    if (end > start) {
        duration = end - start;
    }
    int64_t wait = WAIT-duration;
    if (wait < 0) {
        wait = 0;
    }
    if (wait > WAIT) {
        wait = wait;
    }
    FastLED.delay(wait);
}

void loop() {

    EVERY_N_MILLISECONDS(20) {
        gHue++;
        sampleAccelerometer();
    }

    EVERY_N_SECONDS(SECONDS_PER_CUE) {
        nextCue();
    }

    showCurrentPattern();
}
