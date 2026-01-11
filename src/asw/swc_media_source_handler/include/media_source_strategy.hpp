#pragma once

#include <memory>
#include <vector>

#include "app_error_codes.hpp"
#include "app_types.hpp"

namespace AutosarMusicPlayer::Asw::Playlist {
class Playlist;
}

namespace AutosarMusicPlayer::Asw::MediaSource {

class IMediaSourceStrategy {
public:
    virtual ~IMediaSourceStrategy() = default;

    virtual const char* Name() const = 0;
    virtual Common::AppError Activate() = 0;
    virtual Common::AppError Deactivate() = 0;

    virtual Common::AppError GetAvailableTracks(std::vector<Common::SongInfo>& outTracks) = 0;
};

class MediaSourceHandler {
public:
    MediaSourceHandler() = default;

    void SetStrategy(std::unique_ptr<IMediaSourceStrategy> strategy);

    [[nodiscard]] const char* ActiveSourceName() const;

    [[nodiscard]] Common::AppError RefreshPlaylist(Asw::Playlist::Playlist& playlist);

private:
    std::unique_ptr<IMediaSourceStrategy> strategy_;
};

} // namespace AutosarMusicPlayer::Asw::MediaSource
