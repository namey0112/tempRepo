#pragma once

#include <vector>

#include "app_error_codes.hpp"
#include "app_types.hpp"
#include "media_source_strategy.hpp"
#include "usb_mass_storage.hpp"

namespace AutosarMusicPlayer::Asw::MediaSource {

class UsbSource final : public IMediaSourceStrategy {
public:
    explicit UsbSource(Bsw::Cdd::UsbMassStorage& storage) : storage_(storage) {}

    const char* Name() const override { return "USB"; }

    Common::AppError Activate() override { return storage_.Mount(); }
    Common::AppError Deactivate() override { return storage_.Unmount(); }

    Common::AppError GetAvailableTracks(std::vector<Common::SongInfo>& outTracks) override;

private:
    Bsw::Cdd::UsbMassStorage& storage_;
};

} // namespace AutosarMusicPlayer::Asw::MediaSource
