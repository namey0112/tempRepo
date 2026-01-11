#pragma once

#include <cstdint>

#include "audio_codec.hpp"

namespace AutosarMusicPlayer::Test::Mocks {

class MockAudioCodec final : public Bsw::Hal::IAudioCodec {
public:
    Common::AppError Start() override {
        ++startCalls;
        started = true;
        return startResult;
    }

    Common::AppError Pause() override {
        ++pauseCalls;
        return pauseResult;
    }

    Common::AppError Stop() override {
        ++stopCalls;
        started = false;
        return stopResult;
    }

    bool IsStarted() const override { return started; }

    std::uint32_t startCalls{0};
    std::uint32_t pauseCalls{0};
    std::uint32_t stopCalls{0};

    bool started{false};

    Common::AppError startResult{Common::AppError::Ok};
    Common::AppError pauseResult{Common::AppError::Ok};
    Common::AppError stopResult{Common::AppError::Ok};
};

} // namespace AutosarMusicPlayer::Test::Mocks
