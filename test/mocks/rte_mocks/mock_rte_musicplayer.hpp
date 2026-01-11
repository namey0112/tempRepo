#pragma once

#include <string>
#include <vector>

#include "Rte_MusicPlayerApp.h"

namespace AutosarMusicPlayer::Test::Mocks {

class MockRteMusicPlayerApp final : public Rte::IRteMusicPlayerApp {
public:
    void NotifySongChanged(Rte_SongIdType newSongId) override {
        songChanged.push_back(newSongId);
    }

    void NotifyPlaybackStateChanged(const char* stateName) override {
        playbackStates.emplace_back(stateName != nullptr ? stateName : "");
    }

    std::vector<Rte_SongIdType> songChanged;
    std::vector<std::string> playbackStates;
};

} // namespace AutosarMusicPlayer::Test::Mocks
