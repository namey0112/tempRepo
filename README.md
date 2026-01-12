# AUTOSAR Music Player ECU

[![CI Status](https://github.com/username/music-player-autosar/workflows/AUTOSAR%20Music%20Player%20CI/badge.svg)](https://github.com/username/music-player-autosar/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A safety-critical automotive music player application developed following **AUTOSAR (AUTomotive Open System ARchitecture)** standards and **ISO 26262** functional safety requirements.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Build Requirements](#build-requirements)
- [Building the Project](#building-the-project)
- [Running Tests](#running-tests)
- [Code Quality](#code-quality)
- [Documentation](#documentation)
- [Contributing](#contributing)

## 🎯 Overview

This project demonstrates professional ECU software development practices for automotive systems, implementing a music player application with:

- **AUTOSAR 4.3.1 compliant architecture** (ASW/BSW/RTE layers)
- **ISO 26262 ASIL-B** safety requirements
- **Modern C++14/17** features adapted for embedded systems
- **Comprehensive unit testing** with Google Test
- **Design patterns**: State, Strategy, Observer, MVC
- **Zero exceptions** - error codes only for safety-critical code
- **MISRA C++ / AUTOSAR C++14** coding guidelines

## ✨ Features

### Application Features
- Multi-source media playback (USB, Bluetooth)
- Playlist management with observer pattern
- State-based playback control (Play/Pause/Stop)
- HMI integration via AUTOSAR RTE
- Robust error handling without exceptions

### Software Engineering Features
- **Template Metaprogramming** for compile-time optimizations
- **RAII and Smart Pointers** for automatic resource management
- **Result<T> type** for safe error propagation (similar to C++23 std::expected)
- **Comprehensive static analysis** (clang-tidy, clang-format)
- **CI/CD pipeline** with automated testing and compliance checks

## 🏗️ Architecture

### Layer Structure

```
┌─────────────────────────────────────────────────┐
│          Application Software (ASW)              │
│  ┌──────────────┐  ┌──────────────────────┐    │
│  │ Playback Mgr │  │ Media Source Handler │    │
│  │ (State)      │  │ (Strategy)           │    │
│  └──────────────┘  └──────────────────────┘    │
│  ┌──────────────┐  ┌──────────────────────┐    │
│  │ Playlist     │  │ HMI Interface        │    │
│  │ (Model/MVC)  │  │ (Observer/MVC)       │    │
│  └──────────────┘  └──────────────────────┘    │
├─────────────────────────────────────────────────┤
│      Runtime Environment (RTE) - Generated       │
├─────────────────────────────────────────────────┤
│          Basic Software (BSW)                    │
│  ┌──────────────┐  ┌──────────────────────┐    │
│  │ HAL          │  │ CDD                  │    │
│  │ - Audio      │  │ - USB Storage        │    │
│  │ - CAN        │  │                      │    │
│  └──────────────┘  └──────────────────────┘    │
└─────────────────────────────────────────────────┘
```

### Key Design Patterns

| Component | Pattern | Purpose |
|-----------|---------|---------|
| **PlaybackManager** | State Pattern | Manages Play/Pause/Stop states cleanly |
| **MediaSourceHandler** | Strategy Pattern | Pluggable media sources (USB/BT/AUX) |
| **Playlist** | Observer + MVC (Model) | Notifies HMI of playlist changes |
| **HMI Controller** | MVC (Controller) | Coordinates user interactions |

See [doc/architecture.md](doc/architecture.md) for detailed architecture documentation.

## 🔧 Build Requirements

### Minimum Requirements
- **CMake** 3.20 or higher
- **C++ Compiler** with C++17 support:
  - GCC 7.0+ 
  - Clang 5.0+
  - MSVC 2017+
- **Google Test** (automatically fetched via CMake)

### Optional Tools
- **clang-tidy** 14+ for static analysis
- **clang-format** 14+ for code formatting
- **Doxygen** for documentation generation
- **lcov** for code coverage reports

### Installing Dependencies (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    clang-tidy-14 \
    clang-format-14 \
    doxygen \
    graphviz \
    lcov
```

## 🚀 Building the Project

### Standard Build

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run tests
cd build
ctest --output-on-failure
```

### Build with Ninja (faster)

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `MUSIC_PLAYER_BUILD_TESTS` | `ON` | Build unit tests |
| `CMAKE_BUILD_TYPE` | `Release` | Build configuration (Debug/Release) |

## 🧪 Running Tests

### All Tests
```bash
cd build
ctest --output-on-failure
```

### Specific Test
```bash
cd build
./test/music_player_unit_tests --gtest_filter="PlaybackStateMachine.*"
```

### With Code Coverage
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
cmake --build build
cd build
ctest
lcov --capture --directory . --output-file coverage.info
lcov --list coverage.info
```

## 🔍 Code Quality

### Static Analysis

Run clang-tidy:
```bash
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
clang-tidy -p build src/**/*.cpp
```

### Code Formatting

Check formatting:
```bash
find src test -name '*.cpp' -o -name '*.hpp' | xargs clang-format --dry-run --Werror
```

Apply formatting:
```bash
find src test -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i
```

### Compliance Checks

The project enforces:
- **AUTOSAR C++14** coding guidelines via clang-tidy
- **ISO 26262** safety requirements (no exceptions, bounded complexity)
- **Warnings as errors** (`-Werror`) for all builds
- **Comprehensive warning flags** (see CMakeLists.txt)

See [config/static_analysis.config](config/static_analysis.config) for complete settings.

## 📚 Documentation

### Generate API Documentation

```bash
doxygen Doxyfile
```

Documentation will be generated in `doc/generated/html/`.

### Key Documentation Files

- [Architecture Overview](doc/architecture.md) - System design and patterns
- [Requirements](doc/requirements.md) - Functional and safety requirements
- [API Reference](doc/generated/html/index.html) - Auto-generated API docs

## 📁 Project Structure

```
music_player_autosar/
├── .clang-format          # Code style configuration (AUTOSAR C++14)
├── .clang-tidy            # Static analysis rules (ISO 26262)
├── CMakeLists.txt         # Build system
├── Doxyfile               # Documentation generator config
├── config/
│   ├── arxml/             # AUTOSAR configuration files
│   └── static_analysis.config
├── doc/                   # Documentation
├── generated/rte/         # AUTOSAR RTE (auto-generated)
├── src/
│   ├── asw/              # Application Software Components
│   ├── bsw/              # Basic Software (HAL, CDD)
│   └── common/           # Shared utilities
└── test/
    ├── mocks/            # Mock objects
    └── unit_tests/       # Unit test suites
```

## 🤝 Contributing

This is an educational project demonstrating AUTOSAR and safety-critical development practices.

### Coding Standards
- Follow AUTOSAR C++14 guidelines
- All code must pass clang-tidy checks
- Maintain test coverage above 85%
- Use meaningful commit messages
- Update documentation for API changes

### Pull Request Process
1. Ensure all tests pass
2. Run static analysis tools
3. Update documentation if needed
4. Get review from maintainers

## 📜 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🎓 Educational Purpose

This project was created as part of an AUTOSAR and embedded C++ training program, demonstrating:

1. **Advanced C++ Features** for embedded systems (Mục 1)
2. **Smart Pointers & Resource Management** (Mục 2)
3. **Unit Testing** best practices (Mục 3)
4. **Memory Layout** understanding (Mục 4)
5. **Namespaces & Templates** (Mục 5)
6. **OOP Principles** in automotive context (Mục 6)
7. **Design Patterns** for ECU software (Mục 7)
8. **Professional Build Systems** (Mục 8, 9)

---

**Developed with ❤️ for automotive embedded systems**
