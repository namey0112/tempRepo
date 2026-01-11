#include "media_source_strategy.hpp"

#include "playlist.hpp"
#include "strategies/usb_source.hpp"

#include <utility>

namespace AutosarMusicPlayer::Asw::MediaSource {

void MediaSourceHandler::SetStrategy(std::unique_ptr<IMediaSourceStrategy> strategy) {
    if (strategy_ != nullptr) {
        (void)strategy_->Deactivate();
    }

    strategy_ = std::move(strategy);

    if (strategy_ != nullptr) {
        (void)strategy_->Activate();
    }
}

const char* MediaSourceHandler::ActiveSourceName() const {
    return (strategy_ != nullptr) ? strategy_->Name() : "None";
}

Common::AppError MediaSourceHandler::RefreshPlaylist(Asw::Playlist::Playlist& playlist) {
    if (strategy_ == nullptr) {
        return Common::AppError::NotReady;
    }

    std::vector<Common::SongInfo> tracks;
    const auto res = strategy_->GetAvailableTracks(tracks);
    if (res != Common::AppError::Ok) {
        return res;
    }

    (void)playlist.Clear();

    for (auto& track : tracks) {
        const auto addRes = playlist.AddSong(std::move(track));
        if (addRes != Common::AppError::Ok) {
            return addRes;
        }
    }

    return Common::AppError::Ok;
}

Common::AppError UsbSource::GetAvailableTracks(std::vector<Common::SongInfo>& outTracks) {
    std::vector<Bsw::Cdd::UsbMassStorage::FileEntry> files;
    const auto res = storage_.ListMusicFiles(files);
    if (res != Common::AppError::Ok) {
        return res;
    }

    outTracks.clear();
    outTracks.reserve(files.size());

    Common::SongId nextId = 1U;
    for (const auto& f : files) {
        Common::SongInfo info;
        info.id = nextId++;
        info.title = f.name;
        info.durationSeconds = 180U;
        outTracks.push_back(std::move(info));
    }

    return Common::AppError::Ok;
}

} // namespace AutosarMusicPlayer::Asw::MediaSource
