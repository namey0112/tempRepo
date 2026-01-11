#include <gtest/gtest.h>

#include "playback_manager.hpp"
#include "bsw_mocks/mock_audio_codec.hpp"
#include "rte_mocks/mock_rte_musicplayer.hpp"

using AutosarMusicPlayer::Asw::Playback::PlaybackManager;
using AutosarMusicPlayer::Common::AppError;

TEST(PlaybackStateMachine, StartsFromStoppedAndTransitions) {
    AutosarMusicPlayer::Test::Mocks::MockAudioCodec codec;
    AutosarMusicPlayer::Test::Mocks::MockRteMusicPlayerApp rte;

    PlaybackManager mgr(codec, &rte);
    EXPECT_STREQ(mgr.StateName(), "Stopped");

    EXPECT_EQ(mgr.Play(), AppError::Ok);
    EXPECT_STREQ(mgr.StateName(), "Playing");
    EXPECT_EQ(codec.startCalls, 1u);

    EXPECT_EQ(mgr.Pause(), AppError::Ok);
    EXPECT_STREQ(mgr.StateName(), "Paused");
    EXPECT_EQ(codec.pauseCalls, 1u);

    EXPECT_EQ(mgr.Stop(), AppError::Ok);
    EXPECT_STREQ(mgr.StateName(), "Stopped");
    EXPECT_EQ(codec.stopCalls, 1u);

    ASSERT_FALSE(rte.playbackStates.empty());
}

TEST(PlaybackStateMachine, PauseFromStoppedIsInvalid) {
    AutosarMusicPlayer::Test::Mocks::MockAudioCodec codec;
    PlaybackManager mgr(codec, nullptr);

    EXPECT_EQ(mgr.Pause(), AppError::InvalidArgument);
    EXPECT_STREQ(mgr.StateName(), "Stopped");
}
