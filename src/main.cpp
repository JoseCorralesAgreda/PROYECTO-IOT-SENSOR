#include <Arduino.h>
#include "DistanceIndicator.h"
#include "LedDriver.h"
#include "UltrasonicSensor.h"

namespace {
UltrasonicSensor sensor(18, 19);
DistanceIndicator indicator;
LedDriver leds(25, 26, 27);
}

void setup() {
    leds.begin();
    indicator = DistanceIndicator(millis());
    leds.apply(indicator.output(millis()));
    sensor.begin();
}

void loop() {
    Reading reading;
    if (sensor.takeReading(reading)) indicator.accept(reading.distanceCm, millis());
    sensor.update();
    if (sensor.takeReading(reading)) indicator.accept(reading.distanceCm, millis());
    leds.apply(indicator.output(millis()));
}
