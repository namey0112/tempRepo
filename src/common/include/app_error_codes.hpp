#pragma once

#include <cstdint>

namespace AutosarMusicPlayer::Common {

enum class AppError : std::uint8_t {
    Ok = 0,
    InvalidArgument,
    NotFound,
    Busy,
    NotReady,
    IoError,
    Unsupported,
    InternalError
};

[[nodiscard]] constexpr const char* ToString(AppError err) noexcept {
    switch (err) {
    case AppError::Ok: return "Ok";
    case AppError::InvalidArgument: return "InvalidArgument";
    case AppError::NotFound: return "NotFound";
    case AppError::Busy: return "Busy";
    case AppError::NotReady: return "NotReady";
    case AppError::IoError: return "IoError";
    case AppError::Unsupported: return "Unsupported";
    case AppError::InternalError: return "InternalError";
    default: return "Unknown";
    }
}

} // namespace AutosarMusicPlayer::Common
