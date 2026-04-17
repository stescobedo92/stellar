// SPDX-License-Identifier: MIT
// Stellar Toolkit Extensions — ste::detail::ascii
//
// Branchless / constexpr ASCII helpers. These are used everywhere in the
// library so they must be cheap, noexcept, and allocation-free.
#pragma once

#include <cstddef>
#include <cstdint>

namespace ste::detail {

[[nodiscard]] constexpr bool is_ascii(char c) noexcept {
    return static_cast<unsigned char>(c) < 0x80U;
}

[[nodiscard]] constexpr bool is_upper(char c) noexcept {
    return c >= 'A' && c <= 'Z';
}

[[nodiscard]] constexpr bool is_lower(char c) noexcept {
    return c >= 'a' && c <= 'z';
}

[[nodiscard]] constexpr bool is_alpha(char c) noexcept {
    return is_upper(c) || is_lower(c);
}

[[nodiscard]] constexpr bool is_digit(char c) noexcept {
    return c >= '0' && c <= '9';
}

[[nodiscard]] constexpr bool is_alnum(char c) noexcept {
    return is_alpha(c) || is_digit(c);
}

[[nodiscard]] constexpr bool is_space(char c) noexcept {
    // Matches std::isspace on default locale: ' ', \t, \n, \v, \f, \r
    return c == ' ' || (c >= '\t' && c <= '\r');
}

[[nodiscard]] constexpr bool is_separator(char c) noexcept {
    // Approximates C#'s char.IsSeparator for ASCII: whitespace-ish separators.
    return c == ' ' || c == '\t' || c == '\xA0';
}

[[nodiscard]] constexpr char to_lower(char c) noexcept {
    return is_upper(c) ? static_cast<char>(c + ('a' - 'A')) : c;
}

[[nodiscard]] constexpr char to_upper(char c) noexcept {
    return is_lower(c) ? static_cast<char>(c - ('a' - 'A')) : c;
}

// Case-insensitive ASCII char equality.
[[nodiscard]] constexpr bool ieq(char a, char b) noexcept {
    return to_lower(a) == to_lower(b);
}

}  // namespace ste::detail
