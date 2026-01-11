#pragma once

#include <cstdint>
#include <string>

namespace AutosarMusicPlayer::Common {

using SongId = std::uint32_t;

struct SongInfo {
    SongId id{};
    std::string title{};
    std::uint32_t durationSeconds{};
};

constexpr std::uint32_t kMaxPlaylistSize = 200U;

} // namespace AutosarMusicPlayer::Common
