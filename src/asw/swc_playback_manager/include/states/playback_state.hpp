#pragma once

#include "app_error_codes.hpp"

namespace AutosarMusicPlayer::Asw::Playback {

class PlaybackStateMachine;

class PlaybackState {
public:
    virtual ~PlaybackState() = default;

    virtual const char* Name() const = 0;

    virtual Common::AppError Play(PlaybackStateMachine& sm) = 0;
    virtual Common::AppError Pause(PlaybackStateMachine& sm) = 0;
    virtual Common::AppError Stop(PlaybackStateMachine& sm) = 0;
};

} // namespace AutosarMusicPlayer::Asw::Playback
