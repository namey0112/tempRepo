#include "usb_mass_storage.hpp"

namespace AutosarMusicPlayer::Bsw::Cdd {

Common::AppError UsbMassStorage::Mount() {
    mounted_ = true;
    return Common::AppError::Ok;
}

Common::AppError UsbMassStorage::Unmount() {
    mounted_ = false;
    return Common::AppError::Ok;
}

bool UsbMassStorage::IsMounted() const {
    return mounted_;
}

Common::AppError UsbMassStorage::ListMusicFiles(std::vector<FileEntry>& outFiles) const {
    if (!mounted_) {
        return Common::AppError::NotReady;
    }

    // Deterministic “filesystem”.
    outFiles = {
        {"track_001.wav"},
        {"track_002.wav"},
        {"track_003.wav"},
    };

    return Common::AppError::Ok;
}

} // namespace AutosarMusicPlayer::Bsw::Cdd
