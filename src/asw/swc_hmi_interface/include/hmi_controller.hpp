#pragma once

#include "Rte_MusicPlayerApp.h"
#include "playlist.hpp"

namespace AutosarMusicPlayer::Asw::Hmi {

class HmiController final : public Asw::Playlist::IPlaylistObserver {
public:
    HmiController(Asw::Playlist::Playlist& playlist, Rte::IRteMusicPlayerApp* rte);
    ~HmiController() override;

    void OnPlaylistChanged() override;
    void OnSongChanged(Common::SongId newSongId) override;

private:
    Asw::Playlist::Playlist& playlist_;
    Rte::IRteMusicPlayerApp* rte_;
};

} // namespace AutosarMusicPlayer::Asw::Hmi
