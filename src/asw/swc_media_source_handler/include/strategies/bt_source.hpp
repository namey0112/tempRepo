#pragma once

#include <vector>

#include "app_error_codes.hpp"
#include "app_types.hpp"
#include "media_source_strategy.hpp"

namespace AutosarMusicPlayer::Asw::MediaSource {

class BtSource final : public IMediaSourceStrategy {
public:
    const char* Name() const override { return "Bluetooth"; }

    Common::AppError Activate() override { return Common::AppError::Ok; }
    Common::AppError Deactivate() override { return Common::AppError::Ok; }

    Common::AppError GetAvailableTracks(std::vector<Common::SongInfo>& outTracks) override {
        outTracks = {
            {1U, "BT Stream", 0U},
        };
        return Common::AppError::Ok;
    }
};

} // namespace AutosarMusicPlayer::Asw::MediaSource
