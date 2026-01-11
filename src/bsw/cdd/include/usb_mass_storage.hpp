#pragma once

#include <string>
#include <vector>

#include "app_error_codes.hpp"

namespace AutosarMusicPlayer::Bsw::Cdd {

class UsbMassStorage {
public:
    struct FileEntry {
        std::string name;
    };

    // In a real ECU this would talk to USB MSC driver.
    // Here it's a deterministic stub so higher layers can be tested.
    [[nodiscard]] Common::AppError Mount();
    [[nodiscard]] Common::AppError Unmount();

    [[nodiscard]] bool IsMounted() const;

    [[nodiscard]] Common::AppError ListMusicFiles(std::vector<FileEntry>& outFiles) const;

private:
    bool mounted_{false};
};

} // namespace AutosarMusicPlayer::Bsw::Cdd
