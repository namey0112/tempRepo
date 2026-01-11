cmake_minimum_required(VERSION 3.20)

include(FetchContent)

# Prefer vendored googletest if present, otherwise fetch.
set(_GTEST_VENDOR_DIR "${CMAKE_CURRENT_LIST_DIR}/framework/googletest")
if(EXISTS "${_GTEST_VENDOR_DIR}/CMakeLists.txt")
    add_subdirectory("${_GTEST_VENDOR_DIR}" "${CMAKE_BINARY_DIR}/googletest")
else()
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # For MSVC: use shared CRT by default.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(music_player_unit_tests
    unit_tests/asw/test_playback_state_machine.cpp
    unit_tests/asw/test_media_source_strategy.cpp
    unit_tests/asw/test_playlist_model.cpp
    unit_tests/common/test_error_codes.cpp
)

target_link_libraries(music_player_unit_tests PRIVATE
    GTest::gtest_main
    music_player_asw
)

target_include_directories(music_player_unit_tests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/mocks
)

include(GoogleTest)
gtest_discover_tests(music_player_unit_tests)
