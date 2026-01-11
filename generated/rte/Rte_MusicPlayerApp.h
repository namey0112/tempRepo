#pragma once

#include "Rte_Type.h"

namespace AutosarMusicPlayer::Rte {

class IRteMusicPlayerApp {
public:
    virtual ~IRteMusicPlayerApp() = default;

    virtual void NotifySongChanged(Rte_SongIdType newSongId) = 0;
    virtual void NotifyPlaybackStateChanged(const char* stateName) = 0;
};

} // namespace AutosarMusicPlayer::Rte
