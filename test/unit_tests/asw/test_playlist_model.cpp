#include <gtest/gtest.h>

#include "hmi_controller.hpp"
#include "playlist.hpp"
#include "rte_mocks/mock_rte_musicplayer.hpp"

using AutosarMusicPlayer::Asw::Playlist::Playlist;
using AutosarMusicPlayer::Common::AppError;
using AutosarMusicPlayer::Common::SongInfo;

TEST(PlaylistModel, AddSongSetsCurrentAndNotifiesRteViaHmi) {
    Playlist playlist;
    AutosarMusicPlayer::Test::Mocks::MockRteMusicPlayerApp rte;
    AutosarMusicPlayer::Asw::Hmi::HmiController hmi(playlist, &rte);

    EXPECT_EQ(playlist.AddSong(SongInfo{10U, "A", 100U}), AppError::Ok);
    ASSERT_NE(playlist.GetCurrentSong(), nullptr);
    EXPECT_EQ(playlist.GetCurrentSong()->id, 10U);

    // HMI should forward song change to RTE.
    ASSERT_FALSE(rte.songChanged.empty());
    EXPECT_EQ(rte.songChanged.back(), 10U);
}

TEST(PlaylistModel, SetCurrentSongNotFound) {
    Playlist playlist;
    EXPECT_EQ(playlist.SetCurrentSong(123U), AppError::NotFound);
}
