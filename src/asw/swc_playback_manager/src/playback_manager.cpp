#include "playback_manager.hpp"

#include "playback_state_machine.hpp"
#include "states/stop_state.hpp"

namespace AutosarMusicPlayer::Asw::Playback {

PlaybackManager::PlaybackManager(Bsw::Hal::IAudioCodec& codec, Rte::IRteMusicPlayerApp* rte)
    : codec_(codec), rte_(rte), sm_(std::make_unique<PlaybackStateMachine>(codec_, rte_)) {}

PlaybackManager::~PlaybackManager() = default;

Common::AppError PlaybackManager::Play() {
    return sm_->Play();
}

Common::AppError PlaybackManager::Pause() {
    return sm_->Pause();
}

Common::AppError PlaybackManager::Stop() {
    return sm_->Stop();
}

const char* PlaybackManager::StateName() const {
    return sm_->StateName();
}

} // namespace AutosarMusicPlayer::Asw::Playback
