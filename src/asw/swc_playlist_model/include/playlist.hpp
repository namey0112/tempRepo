#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "app_error_codes.hpp"
#include "app_types.hpp"

namespace AutosarMusicPlayer::Asw::Playlist {

class IPlaylistObserver;

class Playlist {
public:
    Playlist() = default;

    [[nodiscard]] Common::AppError AddSong(Common::SongInfo song);
    [[nodiscard]] Common::AppError RemoveSong(Common::SongId id);
    [[nodiscard]] Common::AppError Clear();

    [[nodiscard]] std::size_t Size() const noexcept;

    [[nodiscard]] Common::AppError SetCurrentSong(Common::SongId id);
    [[nodiscard]] const Common::SongInfo* GetCurrentSong() const noexcept;

    void RegisterObserver(IPlaylistObserver* observer);
    void UnregisterObserver(IPlaylistObserver* observer);

private:
    struct Song {
        explicit Song(Common::SongInfo i) : info(std::move(i)) {}
        Common::SongInfo info;
    };

    void NotifyPlaylistChanged();
    void NotifySongChanged(Common::SongId id);

    std::vector<std::unique_ptr<Song>> songs_;
    Common::SongId current_{0U};
    bool hasCurrent_{false};

    std::vector<IPlaylistObserver*> observers_;
};

class IPlaylistObserver {
public:
    virtual ~IPlaylistObserver() = default;
    virtual void OnPlaylistChanged() = 0;
    virtual void OnSongChanged(Common::SongId newSongId) = 0;
};

} // namespace AutosarMusicPlayer::Asw::Playlist
