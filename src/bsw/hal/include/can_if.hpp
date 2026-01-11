#pragma once

#include <cstdint>

#include "app_error_codes.hpp"

namespace AutosarMusicPlayer::Bsw::Hal {

struct CanFrame {
    std::uint32_t id{};
    std::uint8_t dlc{};
    std::uint8_t data[8]{};
};

class ICanIf {
public:
    virtual ~ICanIf() = default;

    virtual Common::AppError Transmit(const CanFrame& frame) = 0;
};

} // namespace AutosarMusicPlayer::Bsw::Hal
