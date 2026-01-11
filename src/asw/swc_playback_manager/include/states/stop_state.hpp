#pragma once

#include "states/playback_state.hpp"

namespace AutosarMusicPlayer::Asw::Playback {

class StopState final : public PlaybackState {
public:
    const char* Name() const override { return "Stopped"; }

    Common::AppError Play(PlaybackStateMachine& sm) override;
    Common::AppError Pause(PlaybackStateMachine& sm) override;
    Common::AppError Stop(PlaybackStateMachine& sm) override;
};

} // namespace AutosarMusicPlayer::Asw::Playback
