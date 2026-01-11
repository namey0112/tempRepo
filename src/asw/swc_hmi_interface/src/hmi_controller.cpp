#include "hmi_controller.hpp"

namespace AutosarMusicPlayer::Asw::Hmi {

HmiController::HmiController(Asw::Playlist::Playlist& playlist, Rte::IRteMusicPlayerApp* rte)
    : playlist_(playlist), rte_(rte) {
    playlist_.RegisterObserver(this);
}

HmiController::~HmiController() {
    playlist_.UnregisterObserver(this);
}

void HmiController::OnPlaylistChanged() {
    // In a real system this would update UI state.
}

void HmiController::OnSongChanged(Common::SongId newSongId) {
    if (rte_ != nullptr) {
        rte_->NotifySongChanged(static_cast<Rte_SongIdType>(newSongId));
    }
}

} // namespace AutosarMusicPlayer::Asw::Hmi
