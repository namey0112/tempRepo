#pragma once

#include <cstdint>

#include "app_error_codes.hpp"

namespace AutosarMusicPlayer::Bsw::Hal {

class IAudioCodec {
public:
    virtual ~IAudioCodec() = default;

    virtual Common::AppError Start() = 0;
    virtual Common::AppError Pause() = 0;
    virtual Common::AppError Stop() = 0;

    virtual bool IsStarted() const = 0;
};

} // namespace AutosarMusicPlayer::Bsw::Hal
