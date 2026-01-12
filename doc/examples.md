# Usage Examples - AUTOSAR Music Player

This document provides practical examples demonstrating how to use the key components of the Music Player application.

## Table of Contents
1. [Basic Playback Control](#1-basic-playback-control)
2. [Media Source Management](#2-media-source-management)
3. [Playlist Operations](#3-playlist-operations)
4. [Observer Pattern Usage](#4-observer-pattern-usage)
5. [Error Handling with Result<T>](#5-error-handling-with-resultt)
6. [Template Metaprogramming Utilities](#6-template-metaprogramming-utilities)

---

## 1. Basic Playback Control

### Example: Using the State Machine

```cpp
#include "playback_state_machine.hpp"
#include "audio_codec.hpp"
#include "Rte_MusicPlayerApp.h"

using namespace AutosarMusicPlayer;

void PlaybackExample() {
    // Dependencies (normally provided by framework)
    Bsw::Hal::IAudioCodec& codec = GetAudioCodec();
    Rte::IRteMusicPlayerApp* rte = GetRteInterface();
    
    // Create state machine
    Asw::Playback::PlaybackStateMachine stateMachine(codec, rte);
    
    // Start playback
    Common::AppError result = stateMachine.Play();
    if (result == Common::AppError::Ok) {
        printf("Now playing: %s\n", stateMachine.StateName());
    }
    
    // Pause
    result = stateMachine.Pause();
    if (result == Common::AppError::Ok) {
        printf("Paused: %s\n", stateMachine.StateName());
    }
    
    // Resume
    result = stateMachine.Play();
    
    // Stop
    result = stateMachine.Stop();
    printf("Final state: %s\n", stateMachine.StateName());
}
```

### Output:
```
Now playing: Playing
Paused: Paused
Final state: Stopped
```

---

## 2. Media Source Management

### Example: Switching Between USB and Bluetooth

```cpp
#include "media_source_strategy.hpp"
#include "strategies/usb_source.hpp"
#include "strategies/bt_source.hpp"
#include "playlist.hpp"

using namespace AutosarMusicPlayer;

void MediaSourceExample() {
    Asw::MediaSource::MediaSourceHandler handler;
    Asw::Playlist::Playlist playlist;
    
    // Configure USB source
    auto usbSource = std::make_unique<Asw::MediaSource::UsbSource>(
        "/media/usb0"  // Mount point
    );
    
    handler.SetStrategy(std::move(usbSource));
    printf("Active source: %s\n", handler.ActiveSourceName());
    
    // Load tracks from USB
    Common::AppError result = handler.RefreshPlaylist(playlist);
    if (result == Common::AppError::Ok) {
        printf("Loaded %zu tracks from USB\n", playlist.Size());
    }
    
    // Switch to Bluetooth
    auto btSource = std::make_unique<Asw::MediaSource::BluetoothSource>();
    handler.SetStrategy(std::move(btSource));
    
    printf("Active source: %s\n", handler.ActiveSourceName());
    
    // Refresh with Bluetooth tracks
    result = handler.RefreshPlaylist(playlist);
    if (result == Common::AppError::Ok) {
        printf("Loaded %zu tracks from Bluetooth\n", playlist.Size());
    }
}
```

### Output:
```
Active source: USB Mass Storage
Loaded 42 tracks from USB
Active source: Bluetooth A2DP
Loaded 15 tracks from Bluetooth
```

---

## 3. Playlist Operations

### Example: Building and Managing a Playlist

```cpp
#include "playlist.hpp"
#include "app_types.hpp"

using namespace AutosarMusicPlayer;

void PlaylistExample() {
    Asw::Playlist::Playlist playlist;
    
    // Add songs
    Common::SongInfo song1{1, "Song A", 180};  // 3 minutes
    Common::SongInfo song2{2, "Song B", 240};  // 4 minutes
    Common::SongInfo song3{3, "Song C", 200};  // 3:20
    
    playlist.AddSong(song1);
    playlist.AddSong(song2);
    playlist.AddSong(song3);
    
    printf("Playlist size: %zu\n", playlist.Size());
    
    // Set current song
    Common::AppError result = playlist.SetCurrentSong(2);
    if (result == Common::AppError::Ok) {
        const Common::SongInfo* current = playlist.GetCurrentSong();
        if (current != nullptr) {
            printf("Now playing: %s (%us)\n", 
                   current->title.c_str(), 
                   current->durationSeconds);
        }
    }
    
    // Remove a song
    result = playlist.RemoveSong(1);
    if (result == Common::AppError::Ok) {
        printf("Song removed. New size: %zu\n", playlist.Size());
    }
    
    // Clear playlist
    playlist.Clear();
    printf("Playlist cleared. Size: %zu\n", playlist.Size());
}
```

### Output:
```
Playlist size: 3
Now playing: Song B (240s)
Song removed. New size: 2
Playlist cleared. Size: 0
```

---

## 4. Observer Pattern Usage

### Example: Observing Playlist Changes

```cpp
#include "playlist.hpp"
#include "hmi_observer.hpp"

using namespace AutosarMusicPlayer;

// Custom observer implementation
class MyHmiDisplay : public Asw::Playlist::IPlaylistObserver {
public:
    void OnPlaylistChanged() override {
        printf("[HMI] Playlist content changed - refreshing display\n");
    }
    
    void OnSongChanged(Common::SongId newSongId) override {
        printf("[HMI] Current song changed to ID: %u\n", newSongId);
    }
};

void ObserverExample() {
    Asw::Playlist::Playlist playlist;
    MyHmiDisplay display;
    
    // Register observer
    playlist.RegisterObserver(&display);
    
    // These operations will trigger notifications
    printf("Adding songs...\n");
    playlist.AddSong({1, "Track 1", 120});
    
    printf("\nChanging current song...\n");
    playlist.SetCurrentSong(1);
    
    printf("\nAdding another song...\n");
    playlist.AddSong({2, "Track 2", 150});
    
    // Unregister when done
    playlist.UnregisterObserver(&display);
    
    printf("\nAfter unregister - no notifications:\n");
    playlist.AddSong({3, "Track 3", 180});
}
```

### Output:
```
Adding songs...
[HMI] Playlist content changed - refreshing display

Changing current song...
[HMI] Current song changed to ID: 1

Adding another song...
[HMI] Playlist content changed - refreshing display

After unregister - no notifications:
```

---

## 5. Error Handling with Result<T>

### Example: Safe Error Propagation

```cpp
#include "result.hpp"
#include "app_error_codes.hpp"

using namespace AutosarMusicPlayer::Common;

// Function returning Result<T>
Result<int> ParseVolume(const char* input) {
    if (input == nullptr) {
        return AppError::InvalidArgument;
    }
    
    int value = atoi(input);
    if (value < 0 || value > 100) {
        return AppError::InvalidArgument;
    }
    
    return value;  // Success case
}

Result<void> SetSystemVolume(int volume) {
    if (volume > 80) {
        printf("Warning: High volume level\n");
    }
    
    // Simulate hardware call
    printf("Volume set to: %d\n", volume);
    return Result<void>::Ok();
}

void ErrorHandlingExample() {
    // Example 1: Success case
    auto result1 = ParseVolume("50");
    if (result1.IsOk()) {
        printf("Parsed volume: %d\n", result1.Value());
        SetSystemVolume(result1.Value());
    }
    
    // Example 2: Error case
    auto result2 = ParseVolume("150");  // Invalid (> 100)
    if (result2.IsError()) {
        printf("Error: %s\n", ToString(result2.Error()));
    }
    
    // Example 3: Using ValueOr
    auto result3 = ParseVolume(nullptr);
    int volume = result3.ValueOr(50);  // Default to 50
    printf("Volume (with default): %d\n", volume);
    
    // Example 4: Void result
    auto voidResult = SetSystemVolume(75);
    if (voidResult.IsOk()) {
        printf("Operation succeeded\n");
    }
}
```

### Output:
```
Parsed volume: 50
Volume set to: 50
Error: InvalidArgument
Volume (with default): 50
Volume set to: 75
Operation succeeded
```

---

## 6. Template Metaprogramming Utilities

### Example: Using Template Helpers

```cpp
#include "template_helpers.hpp"
#include <iostream>
#include <type_traits>

using namespace AutosarMusicPlayer::Common;

// Example 1: Type checking with IsOneOf
template <typename T>
void ProcessValue(T value) {
    if constexpr (IsOneOfV<T, int, long, short>) {
        std::cout << "Processing integer type: " << value << "\n";
    } else if constexpr (std::is_floating_point<T>::value) {
        std::cout << "Processing floating point: " << value << "\n";
    } else {
        static_assert(AlwaysFalseV<T>, "Unsupported type");
    }
}

// Example 2: SFINAE with EnableIf
template <typename T>
EnableIf<std::is_integral<T>::value, void>
PrintType(T value) {
    std::cout << value << " is an integral type\n";
}

template <typename T>
EnableIf<std::is_floating_point<T>::value, void>
PrintType(T value) {
    std::cout << value << " is a floating point type\n";
}

// Example 3: Compile-time string hashing
constexpr std::size_t GetCommandHash(const char* cmd) {
    return HashFnv1a(cmd);
}

enum class Command : std::size_t {
    Play = "play"_hash,
    Pause = "pause"_hash,
    Stop = "stop"_hash,
    Next = "next"_hash
};

void TemplateUtilitiesExample() {
    // Type checking
    ProcessValue(42);        // Integer
    ProcessValue(3.14f);     // Float
    
    // SFINAE example
    PrintType(100);          // Integral
    PrintType(2.718);        // Floating point
    
    // Compile-time hash switching
    const char* userInput = "play";
    std::size_t hash = GetCommandHash(userInput);
    
    switch (static_cast<Command>(hash)) {
        case Command::Play:
            std::cout << "Executing: PLAY\n";
            break;
        case Command::Pause:
            std::cout << "Executing: PAUSE\n";
            break;
        default:
            std::cout << "Unknown command\n";
            break;
    }
    
    // TypeList usage
    using SupportedTypes = TypeList<int, float, double, long>;
    std::cout << "Type list size: " << SupportedTypes::size << "\n";
    
    // All/Any helpers
    constexpr bool allTrue = AllV<true, true, true>;
    constexpr bool anyTrue = AnyV<false, true, false>;
    std::cout << "All true: " << allTrue << "\n";
    std::cout << "Any true: " << anyTrue << "\n";
}
```

### Output:
```
Processing integer type: 42
Processing floating point: 3.14
100 is an integral type
2.718 is a floating point type
Executing: PLAY
Type list size: 4
All true: 1
Any true: 1
```

---

## Integration Example: Complete Workflow

### Example: Full Music Player Session

```cpp
#include "playback_state_machine.hpp"
#include "media_source_strategy.hpp"
#include "playlist.hpp"
#include "hmi_controller.hpp"

using namespace AutosarMusicPlayer;

class MusicPlayerApp {
public:
    void Run() {
        // 1. Initialize components
        InitializeComponents();
        
        // 2. Load media from USB
        LoadMediaFromUsb();
        
        // 3. Start playback
        StartPlayback();
        
        // 4. User interactions
        SimulateUserInteractions();
        
        // 5. Cleanup (RAII handles this automatically)
    }
    
private:
    void InitializeComponents() {
        printf("=== Initializing Music Player ===\n");
        
        // Register HMI as observer
        playlist_.RegisterObserver(&hmiController_);
        
        printf("Components initialized\n\n");
    }
    
    void LoadMediaFromUsb() {
        printf("=== Loading Media ===\n");
        
        auto usbSource = std::make_unique<Asw::MediaSource::UsbSource>("/media/usb");
        mediaHandler_.SetStrategy(std::move(usbSource));
        
        auto result = mediaHandler_.RefreshPlaylist(playlist_);
        if (result == Common::AppError::Ok) {
            printf("Loaded %zu tracks\n\n", playlist_.Size());
        }
    }
    
    void StartPlayback() {
        printf("=== Starting Playback ===\n");
        
        if (playlist_.Size() > 0) {
            playlist_.SetCurrentSong(1);
            playbackMgr_.Play();
            printf("State: %s\n\n", playbackMgr_.StateName());
        }
    }
    
    void SimulateUserInteractions() {
        printf("=== User Interactions ===\n");
        
        // Pause
        printf("User: Pause\n");
        playbackMgr_.Pause();
        printf("State: %s\n\n", playbackMgr_.StateName());
        
        // Resume
        printf("User: Resume\n");
        playbackMgr_.Play();
        printf("State: %s\n\n", playbackMgr_.StateName());
        
        // Next song
        printf("User: Next Track\n");
        // This would trigger observer notifications
        playlist_.SetCurrentSong(2);
        
        // Stop
        printf("User: Stop\n");
        playbackMgr_.Stop();
        printf("State: %s\n\n", playbackMgr_.StateName());
    }
    
    // Components (RAII ensures proper cleanup)
    Asw::Playlist::Playlist playlist_;
    Asw::MediaSource::MediaSourceHandler mediaHandler_;
    Asw::Playback::PlaybackStateMachine playbackMgr_{GetCodec(), GetRte()};
    Asw::Hmi::HmiController hmiController_;
};

int main() {
    MusicPlayerApp app;
    app.Run();
    return 0;
}
```

---

## Best Practices Demonstrated

1. **RAII**: All resources managed by smart pointers and automatic destructors
2. **Error Handling**: Explicit error codes, no exceptions
3. **Design Patterns**: State, Strategy, Observer properly implemented
4. **Type Safety**: Compile-time checks using templates
5. **Const Correctness**: Proper use of const qualifiers
6. **Modern C++**: Using C++14/17 features appropriately

---

**Note**: These examples assume proper initialization of dependencies (codec, RTE interface). In real ECU code, these would be provided by the AUTOSAR framework.
