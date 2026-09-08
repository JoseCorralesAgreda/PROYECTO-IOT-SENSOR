#include <Arduino.h>
#include "LedDriver.h"

void LedDriver::begin() {
    pinMode(redPin_, OUTPUT);
    pinMode(yellowPin_, OUTPUT);
    pinMode(greenPin_, OUTPUT);
}
void LedDriver::apply(LedOutput output) {
    if (!output.red) digitalWrite(redPin_, LOW);
    if (!output.yellow) digitalWrite(yellowPin_, LOW);
    if (!output.green) digitalWrite(greenPin_, LOW);
    if (output.red) digitalWrite(redPin_, HIGH);
    if (output.yellow) digitalWrite(yellowPin_, HIGH);
    if (output.green) digitalWrite(greenPin_, HIGH);
}
