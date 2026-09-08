#include "UltrasonicSensor.h"

void UltrasonicSensor::begin() {
    pinMode(triggerPin_, OUTPUT);
    digitalWrite(triggerPin_, LOW);
    pinMode(echoPin_, INPUT);
    attachInterruptArg(echoPin_, handleInterrupt, this, CHANGE);
}

void IRAM_ATTR UltrasonicSensor::handleInterrupt(void* context) {
    auto& sensor = *static_cast<UltrasonicSensor*>(context);
    portENTER_CRITICAL_ISR(&sensor.mux_);
    sensor.capture_.onEdge(digitalRead(sensor.echoPin_) == HIGH, micros());
    portEXIT_CRITICAL_ISR(&sensor.mux_);
}

void UltrasonicSensor::update() {
    bool trigger = false;
    portENTER_CRITICAL(&mux_);
    const std::uint32_t nowUs = micros();
    capture_.expire(nowUs);
    if (!capture_.active() && !capture_.pending() && !invalidPending_ &&
        (!started_ || nowUs - lastStartUs_ >= 100000)) {
        started_ = true;
        lastStartUs_ = nowUs;
        if (digitalRead(echoPin_) == HIGH) invalidPending_ = true;
        else {
            const std::uint32_t originUs = micros();
            digitalWrite(triggerPin_, HIGH);
            lastStartUs_ = micros();
            capture_.start(originUs);
            trigger = true;
        }
    }
    portEXIT_CRITICAL(&mux_);
    if (trigger) {
        delayMicroseconds(10);
        digitalWrite(triggerPin_, LOW);
    }
}

bool UltrasonicSensor::takeReading(Reading& result) {
    std::optional<std::uint32_t> durationUs;
    portENTER_CRITICAL(&mux_);
    bool available = invalidPending_;
    if (invalidPending_) invalidPending_ = false;
    else available = capture_.takePulse(durationUs);
    portEXIT_CRITICAL(&mux_);
    if (!available) return false;
    result.distanceCm = durationUs ? std::optional<float>(*durationUs / 58.0f) : std::nullopt;
    return true;
}
