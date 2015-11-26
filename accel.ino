/* vim: set ts=8 sts=4 et sw=4 tw=99: */
#define NUM_READINGS 10
#include "accel.h"

struct AccelAxis {
    int readings[NUM_READINGS];

    int axisPin;
    int rawMin, rawMax;
    int currentReading;

    AccelAxis(int axisPin, int rawMin, int rawMax) :
        axisPin(axisPin),
        rawMin(rawMin),
        rawMax(rawMax),
        currentReading(0)
    {
        for (size_t i = 0; i < NUM_READINGS; i++) {
            readSample();
        }
    }

    void readSample() {
        readings[currentReading] = analogRead(axisPin);
        currentReading = (currentReading + 1) % NUM_READINGS;
    }

    int getRawValue() {
        long value = 0;
        for (int i = 0; i < NUM_READINGS; i++) {
            value += readings[i];
        }
        return value / NUM_READINGS;
    }

    int getScaledValue() {
        return map(getRawValue(), rawMin, rawMax, -1000, 1000);
    }
};

static AccelAxis xAxis(A3, 419, 619);
static AccelAxis yAxis(A2, 403, 612);
static AccelAxis zAxis(A1, 405, 609);

void sampleAccelerometer() {
    xAxis.readSample();
    yAxis.readSample();
    zAxis.readSample();
}

Vector3f accelerometerDirection() {
    return normalize(Vector3f(
                xAxis.getScaledValue(),
                yAxis.getScaledValue(),
                zAxis.getScaledValue()
                ));
}
