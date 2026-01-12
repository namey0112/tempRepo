#pragma once

#include <cstddef>
#include <type_traits>

namespace AutosarMusicPlayer::Common {

// ============================================================================
// Type Manipulation Utilities
// ============================================================================

/**
 * @brief Helper for static_assert in template contexts
 * @tparam T Type parameter (unused, for dependent context)
 */
template <typename T>
struct AlwaysFalse : std::false_type {};

template <typename T>
inline constexpr bool AlwaysFalseV = AlwaysFalse<T>::value;

// ============================================================================
// SFINAE Helper Aliases
// ============================================================================

/**
 * @brief Convenience alias for std::enable_if
 * @tparam B Boolean condition
 * @tparam T Type to enable (default: void)
 */
template <bool B, typename T = void>
using EnableIf = typename std::enable_if<B, T>::type;

/**
 * @brief Convenience alias for std::enable_if negation
 */
template <bool B, typename T = void>
using DisableIf = typename std::enable_if<!B, T>::type;

// ============================================================================
// Type List Implementation
// ============================================================================

/**
 * @brief Compile-time list of types for template metaprogramming
 * @tparam Ts Types in the list
 */
template <typename... Ts>
struct TypeList {
    static constexpr std::size_t size = sizeof...(Ts);
};

/**
 * @brief Check if a type T is in the parameter pack
 */
template <typename T, typename... Args>
struct IsOneOf : std::false_type {};

template <typename T, typename... Args>
struct IsOneOf<T, T, Args...> : std::true_type {};

template <typename T, typename U, typename... Args>
struct IsOneOf<T, U, Args...> : IsOneOf<T, Args...> {};

template <typename T, typename... Args>
inline constexpr bool IsOneOfV = IsOneOf<T, Args...>::value;

// ============================================================================
// Type Selection and Conditional
// ============================================================================

/**
 * @brief Get the Nth type from a parameter pack
 */
template <std::size_t N, typename... Ts>
struct TypeAt;

template <typename T, typename... Ts>
struct TypeAt<0, T, Ts...> {
    using type = T;
};

template <std::size_t N, typename T, typename... Ts>
struct TypeAt<N, T, Ts...> {
    using type = typename TypeAt<N - 1, Ts...>::type;
};

template <std::size_t N, typename... Ts>
using TypeAtT = typename TypeAt<N, Ts...>::type;

// ============================================================================
// Integral Constant Helpers
// ============================================================================

/**
 * @brief Compile-time integer constant
 */
template <typename T, T V>
using IntegralConstant = std::integral_constant<T, V>;

template <bool B>
using BoolConstant = IntegralConstant<bool, B>;

template <std::size_t N>
using SizeConstant = IntegralConstant<std::size_t, N>;

// ============================================================================
// Function Traits
// ============================================================================

/**
 * @brief Extract return type from callable
 */
template <typename F>
struct FunctionTraits;

template <typename R, typename... Args>
struct FunctionTraits<R (*)(Args...)> {
    using ReturnType = R;
    static constexpr std::size_t arity = sizeof...(Args);
};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...)> {
    using ReturnType = R;
    using ClassType = C;
    static constexpr std::size_t arity = sizeof...(Args);
};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) const> {
    using ReturnType = R;
    using ClassType = C;
    static constexpr std::size_t arity = sizeof...(Args);
};

// ============================================================================
// Compile-Time String Hashing (for switch-case optimization)
// ============================================================================

/**
 * @brief Compile-time FNV-1a hash for string literals
 * Useful for converting string comparisons to integer comparisons
 */
constexpr std::size_t HashFnv1a(const char* str, std::size_t hash = 14695981039346656037ULL) noexcept {
    return (*str == '\0') ? hash : HashFnv1a(str + 1, (hash ^ static_cast<std::size_t>(*str)) * 1099511628211ULL);
}

/**
 * @brief Compile-time string hash operator for convenient usage
 * Usage: constexpr auto hash = "MyString"_hash;
 */
constexpr std::size_t operator"" _hash(const char* str, std::size_t) noexcept {
    return HashFnv1a(str);
}

// ============================================================================
// Range Utilities
// ============================================================================

/**
 * @brief Check if all boolean values are true
 */
template <bool... Bs>
struct All : std::true_type {};

template <bool B, bool... Bs>
struct All<B, Bs...> : std::integral_constant<bool, B && All<Bs...>::value> {};

template <bool... Bs>
inline constexpr bool AllV = All<Bs...>::value;

/**
 * @brief Check if any boolean value is true
 */
template <bool... Bs>
struct Any : std::false_type {};

template <bool B, bool... Bs>
struct Any<B, Bs...> : std::integral_constant<bool, B || Any<Bs...>::value> {};

template <bool... Bs>
inline constexpr bool AnyV = Any<Bs...>::value;

// ============================================================================
// Concept-like Constraints (Pre-C++20)
// ============================================================================

/**
 * @brief Check if T is an integral type (excluding bool)
 */
template <typename T>
struct IsInteger : std::integral_constant<bool, std::is_integral<T>::value && !std::is_same<T, bool>::value> {};

template <typename T>
inline constexpr bool IsIntegerV = IsInteger<T>::value;

/**
 * @brief Check if T is an enumeration type
 */
template <typename T>
inline constexpr bool IsEnumV = std::is_enum<T>::value;

} // namespace AutosarMusicPlayer::Common
