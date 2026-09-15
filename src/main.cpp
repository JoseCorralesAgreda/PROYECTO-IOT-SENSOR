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