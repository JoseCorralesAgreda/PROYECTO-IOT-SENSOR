#pragma once
#include <Arduino.h>
#include "EchoCapture.h"
#include "Reading.h"

class UltrasonicSensor {
public:
    UltrasonicSensor(std::uint8_t triggerPin, std::uint8_t echoPin)
        : triggerPin_(triggerPin), echoPin_(echoPin) {}
    void begin();
    void update();
    bool takeReading(Reading& result);
private:
    static void IRAM_ATTR handleInterrupt(void* context);
    const std::uint8_t triggerPin_;
    const std::uint8_t echoPin_;
    portMUX_TYPE mux_ = portMUX_INITIALIZER_UNLOCKED;
    EchoCapture capture_;
    bool invalidPending_ = false;
    bool started_ = false;
    std::uint32_t lastStartUs_ = 0;
};
