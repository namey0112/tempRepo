# AUTOSAR Music Player - Architecture Documentation

## Table of Contents
1. [Overview](#overview)
2. [AUTOSAR Layered Architecture](#autosar-layered-architecture)
3. [Software Components (SW-C)](#software-components-sw-c)
4. [Design Patterns Implementation](#design-patterns-implementation)
5. [Memory Management Strategy](#memory-management-strategy)
6. [Error Handling Architecture](#error-handling-architecture)
7. [Component Interaction Diagrams](#component-interaction-diagrams)

---

## 1. Overview

The Music Player application follows **AUTOSAR 4.3.1** architecture principles, implementing a three-layer structure:

- **Application Software Layer (ASW)**: Business logic and application-specific components
- **Runtime Environment (RTE)**: Middleware providing communication infrastructure
- **Basic Software Layer (BSW)**: Hardware abstraction and system services

### Key Architecture Principles

1. **Separation of Concerns**: Each SW-C has a single, well-defined responsibility
2. **Interface-Based Design**: Components interact through abstract interfaces
3. **Compile-Time Polymorphism**: Prefer templates and static dispatch for performance
4. **Zero Exceptions**: All error handling uses error codes (ISO 26262 compliance)
5. **RAII Everywhere**: Automatic resource management via smart pointers

---

## 2. AUTOSAR Layered Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    Application Software (ASW)                    │
│                                                                   │
│  ┌───────────────────┐  ┌──────────────────────────────────┐   │
│  │ SWC_Playback_Mgr  │  │ SWC_Media_Source_Handler         │   │
│  │                   │  │                                  │   │
│  │ - State Pattern   │  │ - Strategy Pattern               │   │
│  │ - Play/Pause/Stop │  │ - USB / Bluetooth sources        │   │
│  └───────────────────┘  └──────────────────────────────────┘   │
│                                                                   │
│  ┌───────────────────┐  ┌──────────────────────────────────┐   │
│  │ SWC_Playlist      │  │ SWC_HMI_Interface                │   │
│  │                   │  │                                  │   │
│  │ - Model (MVC)     │  │ - Controller/View (MVC)          │   │
│  │ - Observer Subject│  │ - Observer Pattern               │   │
│  └───────────────────┘  └──────────────────────────────────┘   │
│                                                                   │
├───────────────────────────────────────────────────────────────────┤
│              Runtime Environment (RTE) - Generated                │
│                                                                   │
│  - Port-based communication between SW-Cs                        │
│  - Sender/Receiver and Client/Server paradigms                   │
│  - Event-driven scheduling hooks                                 │
│                                                                   │
├───────────────────────────────────────────────────────────────────┤
│                    Basic Software (BSW)                           │
│                                                                   │
│  ┌─────────────────────────────┐  ┌──────────────────────────┐  │
│  │ Hardware Abstraction (HAL)  │  │ Complex Drivers (CDD)    │  │
│  │                             │  │                          │  │
│  │ - IAudioCodec (interface)   │  │ - USB Mass Storage       │  │
│  │ - ICanInterface             │  │ - File I/O Wrapper       │  │
│  │                             │  │                          │  │
│  └─────────────────────────────┘  └──────────────────────────┘  │
│                                                                   │
└───────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
                        ┌──────────────────┐
                        │   ECU Hardware   │
                        │ - Audio Codec    │
                        │ - CAN Bus        │
                        │ - USB Controller │
                        └──────────────────┘
```

---

## 3. Software Components (SW-C)

### 3.1 SWC_Playback_Manager

**Purpose**: Manages playback state transitions (Playing, Paused, Stopped)

**Design Pattern**: State Pattern

**Key Classes**:
- `PlaybackStateMachine`: Context managing state transitions
- `PlaybackState`: Abstract base state
- `PlayState`, `PauseState`, `StopState`: Concrete state implementations

**State Diagram**:
```
    ┌──────────┐
    │  Stopped │ ◄────────┐
    └──────────┘          │
         │                │
         │ Play()         │ Stop()
         ▼                │
    ┌──────────┐          │
    │ Playing  │──────────┤
    └──────────┘          │
         │   ▲            │
 Pause() │   │ Play()     │
         ▼   │            │
    ┌──────────┐          │
    │  Paused  │──────────┘
    └──────────┘
```

**Dependencies**:
- `Bsw::Hal::IAudioCodec`: Hardware abstraction for audio control
- `Rte::IRteMusicPlayerApp`: RTE interface for notifications

**File Location**: `src/asw/swc_playback_manager/`

---

### 3.2 SWC_Media_Source_Handler

**Purpose**: Abstracts different media sources (USB, Bluetooth, AUX)

**Design Pattern**: Strategy Pattern

**Key Classes**:
- `IMediaSourceStrategy`: Abstract strategy interface
- `UsbSource`, `BluetoothSource`: Concrete strategies
- `MediaSourceHandler`: Context managing active strategy

**Strategy Selection**:
```cpp
// Runtime strategy switching
handler.SetStrategy(std::make_unique<UsbSource>());
handler.RefreshPlaylist(playlist);  // Uses USB strategy

handler.SetStrategy(std::make_unique<BluetoothSource>());
handler.RefreshPlaylist(playlist);  // Uses BT strategy
```

**Benefits**:
- Easy to add new sources (e.g., AUX, SD Card) without modifying existing code
- Source-specific logic encapsulated in strategy classes
- Testable in isolation with mock strategies

**File Location**: `src/asw/swc_media_source_handler/`

---

### 3.3 SWC_Playlist_Model

**Purpose**: Manages song collection and current selection (Model in MVC)

**Design Patterns**: Observer Pattern, Model (MVC)

**Key Features**:
- Uses `std::vector<std::unique_ptr<Song>>` for automatic memory management
- Notifies observers when playlist or current song changes
- Implements `IPlaylistObserver` interface for notification

**Observer Notification Flow**:
```
Playlist::AddSong()
    │
    ├─► NotifyPlaylistChanged()
    │       │
    │       └─► observer1->OnPlaylistChanged()
    │       └─► observer2->OnPlaylistChanged()
    │       └─► ...
    │
    └─► NotifySongChanged()
            │
            └─► observer1->OnSongChanged(songId)
            └─► observer2->OnSongChanged(songId)
```

**Memory Management**:
```cpp
class Playlist {
    // RAII: unique_ptr ensures automatic cleanup
    std::vector<std::unique_ptr<Song>> songs_;
    
    // No manual delete needed!
};
```

**File Location**: `src/asw/swc_playlist_model/`

---

### 3.4 SWC_HMI_Interface

**Purpose**: Controller and View in MVC pattern, coordinates user interactions

**Design Patterns**: Observer Pattern (observer), MVC (Controller/View)

**Responsibilities**:
- Observes `Playlist` for changes
- Sends user commands to `PlaybackStateMachine`
- Updates display via RTE

**Interaction Example**:
```cpp
// HmiController observes Playlist
playlist.RegisterObserver(&hmiController);

// User presses "Next Song"
hmiController.OnNextSongPressed();
    └─► playlist.SetCurrentSong(nextId);
            └─► NotifySongChanged(nextId);
                    └─► hmiController.OnSongChanged(nextId);
                            └─► UpdateDisplay();
```

**File Location**: `src/asw/swc_hmi_interface/`

---

## 4. Design Patterns Implementation

### 4.1 State Pattern (Playback Manager)

**Problem**: Managing complex state-dependent behavior without massive switch statements

**Solution**:
```cpp
class PlaybackStateMachine {
    std::unique_ptr<PlaybackState> state_;  // Current state
    
    AppError Play() {
        return state_->OnPlay(*this);  // Delegate to state
    }
};

class PlayState : public PlaybackState {
    AppError OnPlay(PlaybackStateMachine& context) override {
        return AppError::Ok;  // Already playing
    }
    
    AppError OnPause(PlaybackStateMachine& context) override {
        context.Codec().Pause();
        context.TransitionTo(std::make_unique<PauseState>());
        return AppError::Ok;
    }
};
```

**Benefits**:
- Each state's logic is isolated in its own class
- Easy to add new states (e.g., FastForward, Rewind)
- State transitions are explicit and traceable

---

### 4.2 Strategy Pattern (Media Source)

**Problem**: Supporting multiple media sources with different implementations

**Solution**:
```cpp
class IMediaSourceStrategy {
public:
    virtual AppError GetAvailableTracks(
        std::vector<SongInfo>& outTracks) = 0;
};

class UsbSource : public IMediaSourceStrategy {
    AppError GetAvailableTracks(
        std::vector<SongInfo>& outTracks) override {
        // Scan USB drive for MP3 files
        // ...
    }
};
```

**Benefits**:
- Open/Closed Principle: open for extension, closed for modification
- Each source can have complex logic without affecting others
- Easy to mock for testing

---

### 4.3 Observer Pattern (Playlist & HMI)

**Problem**: HMI needs to react to playlist changes without tight coupling

**Solution**:
```cpp
class IPlaylistObserver {
public:
    virtual void OnPlaylistChanged() = 0;
    virtual void OnSongChanged(SongId id) = 0;
};

class Playlist {
    std::vector<IPlaylistObserver*> observers_;
    
    void NotifyPlaylistChanged() {
        for (auto* obs : observers_) {
            obs->OnPlaylistChanged();
        }
    }
};
```

**Benefits**:
- Playlist doesn't need to know about HMI specifics
- Multiple observers can react to same changes
- Testable with mock observers

---

## 5. Memory Management Strategy

### 5.1 Smart Pointer Usage

**Guidelines**:

| Pointer Type | Use Case | Example |
|--------------|----------|---------|
| `std::unique_ptr<T>` | Exclusive ownership | Song objects in Playlist |
| `std::shared_ptr<T>` | Shared ownership | (Rarely used in embedded) |
| Raw pointer `T*` | Non-owning reference | Observer pointers |

**Example from Playlist**:
```cpp
class Playlist {
private:
    // Playlist OWNS songs - use unique_ptr
    std::vector<std::unique_ptr<Song>> songs_;
    
    // Playlist does NOT own observers - use raw pointer
    std::vector<IPlaylistObserver*> observers_;
};
```

### 5.2 RAII Principles

Every resource acquisition is paired with automatic release:

```cpp
class MediaSourceHandler {
    std::unique_ptr<IMediaSourceStrategy> strategy_;
    
    void SetStrategy(std::unique_ptr<IMediaSourceStrategy> newStrategy) {
        // Old strategy automatically deleted via unique_ptr
        strategy_ = std::move(newStrategy);
    }
    
    // Destructor automatically cleans up strategy_
    ~MediaSourceHandler() = default;
};
```

---

## 6. Error Handling Architecture

### 6.1 No Exceptions (ISO 26262 Requirement)

**Why**: Exceptions have non-deterministic overhead and stack unwinding issues in safety-critical systems

**Solution**: Error code enums + Result<T> type

```cpp
enum class AppError : uint8_t {
    Ok,
    InvalidArgument,
    NotFound,
    Busy,
    NotReady,
    IoError,
    Unsupported,
    InternalError
};
```

### 6.2 Result<T> Type

Inspired by C++23 `std::expected`:

```cpp
Result<int> Divide(int a, int b) {
    if (b == 0) {
        return AppError::InvalidArgument;
    }
    return a / b;  // Success case
}

// Usage
auto result = Divide(10, 2);
if (result.IsOk()) {
    int value = result.Value();  // Safe access
} else {
    LogError(result.Error());
}
```

**Benefits**:
- Compile-time error handling verification
- No runtime overhead compared to manual error checks
- Type-safe - can't ignore errors accidentally

---

## 7. Component Interaction Diagrams

### 7.1 Play Song Sequence

```
User          HMI              Playlist        PlaybackMgr      AudioCodec
 │             │                   │                │               │
 │  Click Play │                   │                │               │
 ├────────────►│                   │                │               │
 │             │  GetCurrentSong() │                │               │
 │             ├──────────────────►│                │               │
 │             │◄──────────────────┤                │               │
 │             │    SongInfo       │                │               │
 │             │                   │     Play()     │               │
 │             ├────────────────────────────────────►│               │
 │             │                   │                │  StartPlayback│
 │             │                   │                ├──────────────►│
 │             │                   │                │◄──────────────┤
 │             │◄────────────────────────────────────┤               │
 │             │       AppError::Ok                 │               │
 │◄────────────┤                   │                │               │
 │   Update UI │                   │                │               │
```

### 7.2 Change Media Source Flow

```
User          HMI         MediaSourceHandler   Strategy    Playlist
 │             │                   │               │           │
 │  Switch USB │                   │               │           │
 ├────────────►│                   │               │           │
 │             │  SetStrategy()    │               │           │
 │             ├──────────────────►│               │           │
 │             │     (UsbSource)   │               │           │
 │             │                   │   Activate()  │           │
 │             │                   ├──────────────►│           │
 │             │                   │◄──────────────┤           │
 │             │  RefreshPlaylist()│               │           │
 │             ├──────────────────►│ GetTracks()   │           │
 │             │                   ├──────────────►│           │
 │             │                   │◄──────────────┤           │
 │             │                   │ [Track List]  │           │
 │             │                   │               │  Clear()  │
 │             │                   ├─────────────────────────►│
 │             │                   │               │  AddSong()│
 │             │                   ├─────────────────────────►│
 │             │                   │               │ (for each)│
 │             │◄──────────────────┤               │           │
 │◄────────────┤                   │               │           │
 │  Playlist   │                   │               │           │
 │   Updated   │                   │               │           │
```

---

## Summary

This architecture demonstrates:

✅ **AUTOSAR compliance** with proper layering (ASW/RTE/BSW)  
✅ **Design patterns** for maintainable, extensible code  
✅ **Modern C++** features adapted for embedded constraints  
✅ **Safety-critical** practices (no exceptions, RAII, error codes)  
✅ **Testability** through interfaces and dependency injection  

The system is designed to be:
- **Scalable**: Easy to add new features (sources, states, observers)
- **Maintainable**: Clear separation of concerns
- **Safe**: ISO 26262 compliant error handling
- **Performant**: Compile-time polymorphism where possible
