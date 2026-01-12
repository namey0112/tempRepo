cmake_minimum_required(VERSION 3.20)

project(music_player_autosar LANGUAGES CXX)

option(MUSIC_PLAYER_BUILD_TESTS "Build unit tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Compiler Warning Flags (ISO 26262 & AUTOSAR C++14 Compliance)
# ============================================================================
set(MUSIC_PLAYER_WARNING_FLAGS
    -Wall
    -Wextra
    -Wpedantic
    -Werror                      # Treat warnings as errors
    -Wconversion                 # Implicit type conversions
    -Wsign-conversion            # Sign conversions
    -Wcast-align                 # Pointer alignment
    -Wcast-qual                  # Cast removes qualifiers
    -Wdouble-promotion           # Float to double promotion
    -Wfloat-equal                # Float equality comparison
    -Wformat=2                   # Printf/scanf format checks
    -Wnon-virtual-dtor           # Non-virtual destructor
    -Wold-style-cast             # C-style casts
    -Woverloaded-virtual         # Overloaded virtual functions
    -Wshadow                     # Variable shadowing
    -Wunused                     # Unused variables/functions
    -Wno-unknown-pragmas         # Allow unknown pragmas (for other compilers)
)

# GCC-specific warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND MUSIC_PLAYER_WARNING_FLAGS
        -Wlogical-op
        -Wduplicated-cond
        -Wduplicated-branches
        -Wuseless-cast
    )
endif()

# Clang-specific warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND MUSIC_PLAYER_WARNING_FLAGS
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
    )
endif()

# ============================================================================
# Export compile commands for clang-tidy
# ============================================================================
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(music_player_common INTERFACE)
target_include_directories(music_player_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/rte
)

target_compile_features(music_player_common INTERFACE cxx_std_17)
target_compile_options(music_player_common INTERFACE ${MUSIC_PLAYER_WARNING_FLAGS})

add_library(music_player_bsw STATIC
    src/bsw/cdd/src/usb_mass_storage.cpp
)
target_include_directories(music_player_bsw PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bsw/hal/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bsw/cdd/include
)
target_link_libraries(music_player_bsw PUBLIC music_player_common)
target_compile_options(music_player_bsw PRIVATE ${MUSIC_PLAYER_WARNING_FLAGS})

add_library(music_player_asw STATIC
    src/asw/swc_playback_manager/src/playback_manager.cpp
    src/asw/swc_playback_manager/src/playback_state_machine.cpp
    src/asw/swc_media_source_handler/src/media_source_handler.cpp
    src/asw/swc_playlist_model/src/playlist.cpp
    src/asw/swc_hmi_interface/src/hmi_controller.cpp
)

target_include_directories(music_player_asw PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asw/swc_playback_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asw/swc_media_source_handler/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asw/swc_playlist_model/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asw/swc_hmi_interface/include
)

target_link_libraries(music_player_asw PUBLIC music_player_common music_player_bsw)
target_compile_options(music_player_asw PRIVATE ${MUSIC_PLAYER_WARNING_FLAGS})

if(MUSIC_PLAYER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
