#pragma once

#include <memory>

#include "Rte_MusicPlayerApp.h"
#include "audio_codec.hpp"
#include "app_error_codes.hpp"

namespace AutosarMusicPlayer::Asw::Playback {

class PlaybackState;

class PlaybackStateMachine {
public:
    PlaybackStateMachine(Bsw::Hal::IAudioCodec& codec, Rte::IRteMusicPlayerApp* rte);

    [[nodiscard]] Common::AppError Play();
    [[nodiscard]] Common::AppError Pause();
    [[nodiscard]] Common::AppError Stop();

    [[nodiscard]] const char* StateName() const;

    void TransitionTo(std::unique_ptr<PlaybackState> next);

    [[nodiscard]] Bsw::Hal::IAudioCodec& Codec();
    [[nodiscard]] Rte::IRteMusicPlayerApp* RteIf();

private:
    Bsw::Hal::IAudioCodec& codec_;
    Rte::IRteMusicPlayerApp* rte_;
    std::unique_ptr<PlaybackState> state_;
};

} // namespace AutosarMusicPlayer::Asw::Playback
