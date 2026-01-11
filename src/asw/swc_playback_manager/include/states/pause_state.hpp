#pragma once

#include "states/playback_state.hpp"

namespace AutosarMusicPlayer::Asw::Playback {

class PauseState final : public PlaybackState {
public:
    const char* Name() const override { return "Paused"; }

    Common::AppError Play(PlaybackStateMachine& sm) override;
    Common::AppError Pause(PlaybackStateMachine& sm) override;
    Common::AppError Stop(PlaybackStateMachine& sm) override;
};

} // namespace AutosarMusicPlayer::Asw::Playback
