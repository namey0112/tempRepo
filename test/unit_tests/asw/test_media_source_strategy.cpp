#include <gtest/gtest.h>

#include "media_source_strategy.hpp"
#include "playlist.hpp"
#include "strategies/usb_source.hpp"
#include "usb_mass_storage.hpp"

using AutosarMusicPlayer::Asw::MediaSource::MediaSourceHandler;
using AutosarMusicPlayer::Asw::MediaSource::UsbSource;
using AutosarMusicPlayer::Asw::Playlist::Playlist;
using AutosarMusicPlayer::Bsw::Cdd::UsbMassStorage;
using AutosarMusicPlayer::Common::AppError;

TEST(MediaSource, UsbStrategyLoadsPlaylist) {
    UsbMassStorage storage;
    Playlist playlist;

    MediaSourceHandler handler;
    handler.SetStrategy(std::make_unique<UsbSource>(storage));

    EXPECT_EQ(handler.RefreshPlaylist(playlist), AppError::Ok);
    EXPECT_GT(playlist.Size(), 0u);
}
