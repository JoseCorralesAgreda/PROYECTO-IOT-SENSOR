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
    Serial.begin(115200);
    delay(500);

    leds.begin();

    indicator = DistanceIndicator(millis());

    leds.apply(indicator.output(millis()));

    sensor.begin();
}

void loop() {
    Reading reading;

    // First read consumes a result left pending by the previous iteration
    // before a new acquisition is allowed to start.
    if (sensor.takeReading(reading)) {

        if (reading.distanceCm.has_value()) {
            Serial.print("Distance: ");
            Serial.print(*reading.distanceCm);
            Serial.println(" cm");
        } else {
            Serial.println("Distance: INVALID");
        }

        indicator.accept(reading.distanceCm, millis());
    }

    sensor.update();

    // update() can complete or expire a capture immediately (for example an
    // already-high echo or a reached timeout), so the second read applies that
    // result in this same iteration instead of waiting one cycle.
    if (sensor.takeReading(reading)) {

        if (reading.distanceCm.has_value()) {
            Serial.print("Distance: ");
            Serial.print(*reading.distanceCm);
            Serial.println(" cm");
        }
        indicator.accept(reading.distanceCm, millis());
    }

    leds.apply(indicator.output(millis()));
}