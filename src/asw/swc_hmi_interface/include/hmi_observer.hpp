#pragma once

#include "app_types.hpp"

namespace AutosarMusicPlayer::Asw::Hmi {

class IHmiObserver {
public:
    virtual ~IHmiObserver() = default;
    virtual void OnDisplayedSongChanged(Common::SongId id) = 0;
};

} // namespace AutosarMusicPlayer::Asw::Hmi
