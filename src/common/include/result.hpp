#pragma once

#include <type_traits>
#include <utility>

#include "app_error_codes.hpp"

namespace AutosarMusicPlayer::Common {

/**
 * @brief Result type for error handling without exceptions
 * 
 * Similar to std::expected (C++23), this provides a type-safe way to handle
 * errors in safety-critical embedded systems where exceptions are prohibited.
 * 
 * A Result<T> contains either a value of type T or an error code.
 * This is essential for ISO 26262 compliance as it enforces explicit error handling.
 * 
 * @tparam T The success value type
 * 
 * Usage:
 * @code
 * Result<int> Divide(int a, int b) {
 *     if (b == 0) {
 *         return AppError::InvalidArgument;
 *     }
 *     return a / b;
 * }
 * 
 * auto result = Divide(10, 2);
 * if (result.IsOk()) {
 *     int value = result.Value();
 * } else {
 *     AppError err = result.Error();
 * }
 * @endcode
 */
template <typename T>
class Result {
public:
    // ========================================================================
    // Constructors
    // ========================================================================

    /**
     * @brief Construct a successful result with a value
     */
    explicit Result(T value) noexcept(std::is_nothrow_move_constructible<T>::value)
        : hasValue_(true) {
        new (&storage_.value) T(std::move(value));
    }

    /**
     * @brief Construct an error result
     */
    explicit Result(AppError error) noexcept : hasValue_(false) {
        storage_.error = error;
    }

    /**
     * @brief Destructor - properly destroys the stored value if present
     */
    ~Result() noexcept {
        if (hasValue_) {
            storage_.value.~T();
        }
    }

    // ========================================================================
    // Copy/Move Operations
    // ========================================================================

    Result(const Result& other) noexcept(std::is_nothrow_copy_constructible<T>::value)
        : hasValue_(other.hasValue_) {
        if (hasValue_) {
            new (&storage_.value) T(other.storage_.value);
        } else {
            storage_.error = other.storage_.error;
        }
    }

    Result(Result&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
        : hasValue_(other.hasValue_) {
        if (hasValue_) {
            new (&storage_.value) T(std::move(other.storage_.value));
        } else {
            storage_.error = other.storage_.error;
        }
    }

    Result& operator=(const Result& other) noexcept(std::is_nothrow_copy_constructible<T>::value) {
        if (this != &other) {
            if (hasValue_) {
                storage_.value.~T();
            }
            hasValue_ = other.hasValue_;
            if (hasValue_) {
                new (&storage_.value) T(other.storage_.value);
            } else {
                storage_.error = other.storage_.error;
            }
        }
        return *this;
    }

    Result& operator=(Result&& other) noexcept(std::is_nothrow_move_constructible<T>::value) {
        if (this != &other) {
            if (hasValue_) {
                storage_.value.~T();
            }
            hasValue_ = other.hasValue_;
            if (hasValue_) {
                new (&storage_.value) T(std::move(other.storage_.value));
            } else {
                storage_.error = other.storage_.error;
            }
        }
        return *this;
    }

    // ========================================================================
    // State Queries
    // ========================================================================

    /**
     * @brief Check if the result contains a value (success)
     */
    [[nodiscard]] bool IsOk() const noexcept {
        return hasValue_;
    }

    /**
     * @brief Check if the result contains an error (failure)
     */
    [[nodiscard]] bool IsError() const noexcept {
        return !hasValue_;
    }

    /**
     * @brief Conversion to bool - returns true if result is Ok
     */
    explicit operator bool() const noexcept {
        return IsOk();
    }

    // ========================================================================
    // Value Access
    // ========================================================================

    /**
     * @brief Access the value (requires IsOk() == true)
     * @warning Undefined behavior if called on error result
     */
    [[nodiscard]] T& Value() & noexcept {
        return storage_.value;
    }

    [[nodiscard]] const T& Value() const& noexcept {
        return storage_.value;
    }

    [[nodiscard]] T&& Value() && noexcept {
        return std::move(storage_.value);
    }

    /**
     * @brief Access the error (requires IsError() == true)
     * @warning Undefined behavior if called on success result
     */
    [[nodiscard]] AppError Error() const noexcept {
        return storage_.error;
    }

    /**
     * @brief Get value or default if error
     */
    [[nodiscard]] T ValueOr(T defaultValue) const& noexcept(std::is_nothrow_copy_constructible<T>::value) {
        return hasValue_ ? storage_.value : std::move(defaultValue);
    }

    [[nodiscard]] T ValueOr(T defaultValue) && noexcept(std::is_nothrow_move_constructible<T>::value) {
        return hasValue_ ? std::move(storage_.value) : std::move(defaultValue);
    }

private:
    // Union for storage optimization - only one is active at a time
    union Storage {
        T value;
        AppError error;

        Storage() noexcept {}
        ~Storage() noexcept {}
    };

    Storage storage_;
    bool hasValue_;
};

/**
 * @brief Specialization for void result (only success/error, no value)
 * 
 * Usage:
 * @code
 * Result<void> DoOperation() {
 *     if (/* error condition */) {
 *         return AppError::IoError;
 *     }
 *     return Result<void>::Ok();
 * }
 * @endcode
 */
template <>
class Result<void> {
public:
    /**
     * @brief Create a successful void result
     */
    static Result Ok() noexcept {
        return Result(true);
    }

    /**
     * @brief Construct an error result
     */
    explicit Result(AppError error) noexcept : error_(error), hasValue_(false) {}

    [[nodiscard]] bool IsOk() const noexcept {
        return hasValue_;
    }

    [[nodiscard]] bool IsError() const noexcept {
        return !hasValue_;
    }

    explicit operator bool() const noexcept {
        return IsOk();
    }

    [[nodiscard]] AppError Error() const noexcept {
        return error_;
    }

private:
    explicit Result(bool) noexcept : error_(AppError::Ok), hasValue_(true) {}

    AppError error_;
    bool hasValue_;
};

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Create a successful result
 */
template <typename T>
[[nodiscard]] Result<typename std::decay<T>::type> Ok(T&& value) noexcept(
    std::is_nothrow_constructible<typename std::decay<T>::type, T>::value) {
    return Result<typename std::decay<T>::type>(std::forward<T>(value));
}

/**
 * @brief Create an error result
 */
template <typename T>
[[nodiscard]] Result<T> Err(AppError error) noexcept {
    return Result<T>(error);
}

} // namespace AutosarMusicPlayer::Common
