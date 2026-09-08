#pragma once
#include <cstdint>
#include <optional>

class EchoCapture {
public:
    static constexpr std::uint32_t kTimeoutUs = 30000;
    void start(std::uint32_t nowUs) {
        if (active() || pending()) return;
        originUs_ = nowUs;
        state_ = State::WaitingRise;
    }
    void onEdge(bool high, std::uint32_t nowUs) {
        if (!active()) return;
        const std::uint32_t elapsedUs = nowUs - originUs_;
        // Reject timestamps from before this cycle, including across wrap.
        if (elapsedUs > UINT32_MAX / 2) return;
        if (elapsedUs >= kTimeoutUs) finish(std::nullopt);
        else if (state_ == State::WaitingRise && high) {
            riseUs_ = nowUs;
            state_ = State::WaitingFall;
        } else if (state_ == State::WaitingFall && !high) {
            const std::uint32_t durationUs = nowUs - riseUs_;
            if (durationUs > 0 && durationUs <= elapsedUs) finish(durationUs);
        }
    }
    void expire(std::uint32_t nowUs) {
        if (active() && nowUs - originUs_ >= kTimeoutUs) finish(std::nullopt);
    }
    bool takePulse(std::optional<std::uint32_t>& durationUs) {
        if (!pending()) return false;
        durationUs = durationUs_;
        state_ = State::Idle;
        return true;
    }
    bool active() const { return state_ == State::WaitingRise || state_ == State::WaitingFall; }
    bool pending() const { return state_ == State::Ready; }
private:
    enum class State { Idle, WaitingRise, WaitingFall, Ready };
    void finish(std::optional<std::uint32_t> durationUs) {
        durationUs_ = durationUs;
        state_ = State::Ready;
    }
    State state_ = State::Idle;
    std::uint32_t originUs_ = 0;
    std::uint32_t riseUs_ = 0;
    std::optional<std::uint32_t> durationUs_;
};
