#pragma once

#include <memory>

#include "Rte_MusicPlayerApp.h"
#include "audio_codec.hpp"
#include "app_error_codes.hpp"

namespace AutosarMusicPlayer::Asw::Playback {

class PlaybackStateMachine;

class PlaybackManager {
public:
    PlaybackManager(Bsw::Hal::IAudioCodec& codec, Rte::IRteMusicPlayerApp* rte);
    ~PlaybackManager();

    [[nodiscard]] Common::AppError Play();
    [[nodiscard]] Common::AppError Pause();
    [[nodiscard]] Common::AppError Stop();

    [[nodiscard]] const char* StateName() const;

private:
    Bsw::Hal::IAudioCodec& codec_;
    Rte::IRteMusicPlayerApp* rte_;
    std::unique_ptr<PlaybackStateMachine> sm_;
};

} // namespace AutosarMusicPlayer::Asw::Playback
