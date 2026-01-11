#include "playlist.hpp"

#include <algorithm>

namespace AutosarMusicPlayer::Asw::Playlist {

Common::AppError Playlist::AddSong(Common::SongInfo song) {
    if (songs_.size() >= Common::kMaxPlaylistSize) {
        return Common::AppError::Busy;
    }

    if (song.id == 0U) {
        return Common::AppError::InvalidArgument;
    }

    const bool exists = std::any_of(songs_.begin(), songs_.end(), [&](const auto& ptr) {
        return ptr != nullptr && ptr->info.id == song.id;
    });
    if (exists) {
        return Common::AppError::InvalidArgument;
    }

    songs_.push_back(std::make_unique<Song>(std::move(song)));
    NotifyPlaylistChanged();

    if (!hasCurrent_) {
        hasCurrent_ = true;
        current_ = songs_.front()->info.id;
        NotifySongChanged(current_);
    }

    return Common::AppError::Ok;
}

Common::AppError Playlist::RemoveSong(Common::SongId id) {
    const auto it = std::remove_if(songs_.begin(), songs_.end(), [&](const auto& ptr) {
        return ptr != nullptr && ptr->info.id == id;
    });

    if (it == songs_.end()) {
        return Common::AppError::NotFound;
    }

    songs_.erase(it, songs_.end());
    NotifyPlaylistChanged();

    if (hasCurrent_ && current_ == id) {
        hasCurrent_ = false;
        current_ = 0U;
        if (!songs_.empty()) {
            hasCurrent_ = true;
            current_ = songs_.front()->info.id;
        }
        if (hasCurrent_) {
            NotifySongChanged(current_);
        }
    }

    return Common::AppError::Ok;
}

Common::AppError Playlist::Clear() {
    songs_.clear();
    hasCurrent_ = false;
    current_ = 0U;
    NotifyPlaylistChanged();
    return Common::AppError::Ok;
}

std::size_t Playlist::Size() const noexcept {
    return songs_.size();
}

Common::AppError Playlist::SetCurrentSong(Common::SongId id) {
    const auto it = std::find_if(songs_.begin(), songs_.end(), [&](const auto& ptr) {
        return ptr != nullptr && ptr->info.id == id;
    });

    if (it == songs_.end()) {
        return Common::AppError::NotFound;
    }

    hasCurrent_ = true;
    current_ = id;
    NotifySongChanged(id);
    return Common::AppError::Ok;
}

const Common::SongInfo* Playlist::GetCurrentSong() const noexcept {
    if (!hasCurrent_) {
        return nullptr;
    }

    const auto it = std::find_if(songs_.begin(), songs_.end(), [&](const auto& ptr) {
        return ptr != nullptr && ptr->info.id == current_;
    });

    if (it == songs_.end() || *it == nullptr) {
        return nullptr;
    }

    return &(*it)->info;
}

void Playlist::RegisterObserver(IPlaylistObserver* observer) {
    if (observer == nullptr) {
        return;
    }

    const bool exists = std::any_of(observers_.begin(), observers_.end(), [&](auto* obs) { return obs == observer; });
    if (!exists) {
        observers_.push_back(observer);
    }
}

void Playlist::UnregisterObserver(IPlaylistObserver* observer) {
    observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
}

void Playlist::NotifyPlaylistChanged() {
    for (auto* obs : observers_) {
        if (obs != nullptr) {
            obs->OnPlaylistChanged();
        }
    }
}

void Playlist::NotifySongChanged(Common::SongId id) {
    for (auto* obs : observers_) {
        if (obs != nullptr) {
            obs->OnSongChanged(id);
        }
    }
}

} // namespace AutosarMusicPlayer::Asw::Playlist
