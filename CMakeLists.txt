cmake_minimum_required(VERSION 3.20)

project(music_player_autosar LANGUAGES CXX)

option(MUSIC_PLAYER_BUILD_TESTS "Build unit tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(music_player_common INTERFACE)
target_include_directories(music_player_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/rte
)

target_compile_features(music_player_common INTERFACE cxx_std_17)

add_library(music_player_bsw STATIC
    src/bsw/cdd/src/usb_mass_storage.cpp
)
target_include_directories(music_player_bsw PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bsw/hal/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bsw/cdd/include
)
target_link_libraries(music_player_bsw PUBLIC music_player_common)

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

if(MUSIC_PLAYER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
