#include <gtest/gtest.h>

#include "app_error_codes.hpp"

using AutosarMusicPlayer::Common::AppError;
using AutosarMusicPlayer::Common::ToString;

TEST(ErrorCodes, ToStringHasValues) {
    EXPECT_STREQ(ToString(AppError::Ok), "Ok");
    EXPECT_STREQ(ToString(AppError::InvalidArgument), "InvalidArgument");
    EXPECT_STREQ(ToString(AppError::NotFound), "NotFound");
}
