#pragma once
#include <cstdint>
#include "LedOutput.h"

class LedDriver {
public:
    LedDriver(std::uint8_t redPin, std::uint8_t yellowPin, std::uint8_t greenPin)
        : redPin_(redPin), yellowPin_(yellowPin), greenPin_(greenPin) {}
    void begin();
    void apply(LedOutput output);
private:
    const std::uint8_t redPin_, yellowPin_, greenPin_;
};
