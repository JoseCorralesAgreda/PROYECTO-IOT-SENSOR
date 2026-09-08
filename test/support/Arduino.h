#pragma once
#include <array>
#include <cstdint>
#include <vector>
#define IRAM_ATTR
constexpr int HIGH = 1, LOW = 0, INPUT = 0, OUTPUT = 1, RISING = 1, FALLING = 2, CHANGE = 3;
struct portMUX_TYPE {};
#define portMUX_INITIALIZER_UNLOCKED {}
namespace fake {
inline std::uint64_t nowUs = 0;
inline std::array<int, 40> levels{}, modes{};
inline void (*callback)(void*) = nullptr;
inline void* context = nullptr;
inline int lockDepth = 0, triggerCount = 0;
inline bool delayLocked = false;
inline int interruptPin = -1, interruptMode = 0;
inline std::uint32_t writeCostUs = 0;
inline std::vector<std::uint64_t> pulses;
inline std::vector<std::array<int, 3>> ledWrites;
inline std::vector<portMUX_TYPE*> taskMutexes, isrMutexes;
inline void reset() {
    nowUs = 0; levels.fill(0); modes.fill(0); callback = nullptr; context = nullptr;
    lockDepth = 0; triggerCount = 0; delayLocked = false; pulses.clear();
    interruptPin = -1; interruptMode = 0; writeCostUs = 0;
    ledWrites.clear(); taskMutexes.clear(); isrMutexes.clear();
}
inline void edge(bool high, std::uint64_t atUs, int pin = 19) {
    nowUs = atUs;
    const bool changed = levels[pin] != high;
    levels[pin] = high;
    if (callback && pin == interruptPin && changed &&
        (interruptMode == CHANGE || interruptMode == (high ? RISING : FALLING)))
        callback(context);
}
}
inline void portENTER_CRITICAL(portMUX_TYPE* mux) {
    fake::taskMutexes.push_back(mux); ++fake::lockDepth;
}
inline void portEXIT_CRITICAL(portMUX_TYPE*) { --fake::lockDepth; }
inline void portENTER_CRITICAL_ISR(portMUX_TYPE* mux) {
    fake::isrMutexes.push_back(mux); ++fake::lockDepth;
}
inline void portEXIT_CRITICAL_ISR(portMUX_TYPE* mux) { portEXIT_CRITICAL(mux); }
inline void pinMode(int pin, int mode) { fake::modes[pin] = mode; }
inline void digitalWrite(int pin, int level) {
    fake::nowUs += fake::writeCostUs;
    fake::levels[pin] = level;
    if (pin == 18 && level == HIGH) { ++fake::triggerCount; fake::pulses.push_back(fake::nowUs); }
    if (pin >= 25 && pin <= 27)
        fake::ledWrites.push_back({fake::levels[25], fake::levels[26], fake::levels[27]});
}
inline int digitalRead(int pin) { return fake::levels[pin]; }
inline std::uint32_t micros() { return static_cast<std::uint32_t>(fake::nowUs); }
inline std::uint32_t millis() { return static_cast<std::uint32_t>(fake::nowUs / 1000); }
inline void delayMicroseconds(unsigned durationUs) {
    fake::delayLocked |= fake::lockDepth != 0;
    fake::nowUs += durationUs;
}
inline void attachInterruptArg(int pin, void (*callback)(void*), void* context, int mode) {
    fake::callback = callback; fake::context = context;
    fake::interruptPin = pin; fake::interruptMode = mode;
}
