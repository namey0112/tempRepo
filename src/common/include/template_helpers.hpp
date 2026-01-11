#pragma once

#include <type_traits>

namespace AutosarMusicPlayer::Common {

template <typename T>
struct AlwaysFalse : std::false_type {};

template <typename T>
inline constexpr bool AlwaysFalseV = AlwaysFalse<T>::value;

} // namespace AutosarMusicPlayer::Common
