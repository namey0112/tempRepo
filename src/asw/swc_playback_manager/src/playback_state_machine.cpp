#include "playback_state_machine.hpp"

#include "states/pause_state.hpp"
#include "states/play_state.hpp"
#include "states/stop_state.hpp"

namespace AutosarMusicPlayer::Asw::Playback {

PlaybackStateMachine::PlaybackStateMachine(Bsw::Hal::IAudioCodec& codec, Rte::IRteMusicPlayerApp* rte)
    : codec_(codec), rte_(rte), state_(std::make_unique<StopState>()) {
    if (rte_ != nullptr) {
        rte_->NotifyPlaybackStateChanged(state_->Name());
    }
}

Common::AppError PlaybackStateMachine::Play() {
    const auto res = state_->Play(*this);
    if (rte_ != nullptr) {
        rte_->NotifyPlaybackStateChanged(state_->Name());
    }
    return res;
}

Common::AppError PlaybackStateMachine::Pause() {
    const auto res = state_->Pause(*this);
    if (rte_ != nullptr) {
        rte_->NotifyPlaybackStateChanged(state_->Name());
    }
    return res;
}

Common::AppError PlaybackStateMachine::Stop() {
    const auto res = state_->Stop(*this);
    if (rte_ != nullptr) {
        rte_->NotifyPlaybackStateChanged(state_->Name());
    }
    return res;
}

const char* PlaybackStateMachine::StateName() const {
    return state_->Name();
}

void PlaybackStateMachine::TransitionTo(std::unique_ptr<PlaybackState> next) {
    state_ = std::move(next);
}

Bsw::Hal::IAudioCodec& PlaybackStateMachine::Codec() {
    return codec_;
}

Rte::IRteMusicPlayerApp* PlaybackStateMachine::RteIf() {
    return rte_;
}

// --- State implementations ---

Common::AppError StopState::Play(PlaybackStateMachine& sm) {
    const auto res = sm.Codec().Start();
    if (res == Common::AppError::Ok) {
        sm.TransitionTo(std::make_unique<PlayState>());
    }
    return res;
}

Common::AppError StopState::Pause(PlaybackStateMachine&) {
    return Common::AppError::InvalidArgument;
}

Common::AppError StopState::Stop(PlaybackStateMachine&) {
    return Common::AppError::Ok;
}

Common::AppError PlayState::Play(PlaybackStateMachine&) {
    return Common::AppError::Ok;
}

Common::AppError PlayState::Pause(PlaybackStateMachine& sm) {
    const auto res = sm.Codec().Pause();
    if (res == Common::AppError::Ok) {
        sm.TransitionTo(std::make_unique<PauseState>());
    }
    return res;
}

Common::AppError PlayState::Stop(PlaybackStateMachine& sm) {
    const auto res = sm.Codec().Stop();
    if (res == Common::AppError::Ok) {
        sm.TransitionTo(std::make_unique<StopState>());
    }
    return res;
}

Common::AppError PauseState::Play(PlaybackStateMachine& sm) {
    const auto res = sm.Codec().Start();
    if (res == Common::AppError::Ok) {
        sm.TransitionTo(std::make_unique<PlayState>());
    }
    return res;
}

Common::AppError PauseState::Pause(PlaybackStateMachine&) {
    return Common::AppError::Ok;
}

Common::AppError PauseState::Stop(PlaybackStateMachine& sm) {
    const auto res = sm.Codec().Stop();
    if (res == Common::AppError::Ok) {
        sm.TransitionTo(std::make_unique<StopState>());
    }
    return res;
}

} // namespace AutosarMusicPlayer::Asw::Playback
