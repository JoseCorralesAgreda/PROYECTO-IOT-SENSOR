#pragma once
#include <cmath>
#include <cstdint>
#include <optional>
#include "LedOutput.h"

class DistanceIndicator {
public:
    explicit DistanceIndicator(std::uint32_t nowMs = 0) : phaseStartMs_(nowMs) {}
    void accept(std::optional<float> distanceCm, std::uint32_t nowMs) {
        const auto next = classify(distanceCm);
        if (next == State::Invalid && state_ != State::Invalid) {
            ++invalidReadings_;
            if (invalidReadings_ < kInvalidReadingsBeforeError)
                return;
            phaseStartMs_ = nowMs;
            phaseOn_ = true;
        } else if (next != State::Invalid) {
            invalidReadings_ = 0;
        }
        state_ = next;
    }
    LedOutput output(std::uint32_t nowMs) {
        if (state_ != State::Invalid)
            return {state_ == State::Red, state_ == State::Yellow, state_ == State::Green};
        const std::uint32_t steps = (nowMs - phaseStartMs_) / 250;
        phaseStartMs_ += steps * 250;
        if (steps % 2 != 0) phaseOn_ = !phaseOn_;
        return {phaseOn_, phaseOn_, phaseOn_};
    }
private:
    static constexpr std::uint8_t kInvalidReadingsBeforeError = 2;
    enum class State { Invalid, Red, Yellow, Green };
    static State classify(std::optional<float> distanceCm) {
        if (!distanceCm || !std::isfinite(*distanceCm) || *distanceCm < 2.0f ||
            *distanceCm > 400.0f) return State::Invalid;
        if (*distanceCm < 10.0f) return State::Red;
        if (*distanceCm < 30.0f) return State::Yellow;
        return State::Green;
    }
    State state_ = State::Invalid;
    std::uint8_t invalidReadings_ = 0;
    std::uint32_t phaseStartMs_;
    bool phaseOn_ = true;
};
