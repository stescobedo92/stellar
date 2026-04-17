// SPDX-License-Identifier: MIT
// Stellar Toolkit Extensions — ste::strings
//
// Header-only C++23 string utilities matching the surface of
// BinaryCoffee.Extensions (C#) one-for-one, plus a curated set of
// professional extras. All hot paths are single-pass, use pre-reserved
// buffers, and accept std::string_view so callers never pay for a copy.
//
// Fluent syntax is available via ste::str (a thin std::string wrapper):
//     ste::str("hello world").ToCamelCase();   // → "HelloWorld"
// Free functions are also exported in the ste:: namespace for ergonomic
// use with raw std::string / std::string_view.
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "detail/ascii.hpp"
#include "detail/concepts.hpp"

namespace ste {

// ============================================================================
//  Predicates — IsNullOrEmpty / IsNullOrWhiteSpace (mirror C# BCL)
// ============================================================================

[[nodiscard]] constexpr bool IsNullOrEmpty(std::string_view s) noexcept {
    return s.empty();
}

[[nodiscard]] constexpr bool IsNullOrWhiteSpace(std::string_view s) noexcept {
    for (char c : s)
        if (!detail::is_space(c)) return false;
    return true;
}

// ============================================================================
//  Case conversion
// ============================================================================

// Matches C# BinaryCoffee.Extensions behaviour exactly: whitespace / separator
// toggles 'needUpper'; first non-ws char is uppercased; rest lowercased.
[[nodiscard]] inline std::string ToCamelCase(std::string_view name) {
    if (name.empty()) return {};
    std::string out;
    out.reserve(name.size());
    bool need_upper = true;
    for (char ch : name) {
        if (detail::is_space(ch) || detail::is_separator(ch)) {
            need_upper = true;
            continue;
        }
        out.push_back(need_upper ? detail::to_upper(ch) : detail::to_lower(ch));
        need_upper = false;
    }
    return out;
}

[[nodiscard]] inline std::string ToPascalCase(std::string_view name) {
    if (name.empty()) return {};
    std::string out;
    out.reserve(name.size());
    bool capitalize = true;
    for (char ch : name) {
        if (detail::is_space(ch) || detail::is_separator(ch)) {
            capitalize = true;
            continue;
        }
        out.push_back(capitalize ? detail::to_upper(ch) : detail::to_lower(ch));
        capitalize = false;
    }
    return out;
}

namespace detail {
template <bool Upper>
[[nodiscard]] inline std::string to_snake_impl(std::string_view name) {
    if (name.empty()) return {};
    std::string out;
    out.reserve(name.size() + name.size() / 4);
    bool started = false;
    char last = '\0';
    for (char ch : name) {
        if (is_upper(ch) && started && is_lower(last)) {
            out.push_back('_');
        }
        out.push_back(Upper ? to_upper(ch) : to_lower(ch));
        started = true;
        last = ch;
    }
    return out;
}
}  // namespace detail

[[nodiscard]] inline std::string ToSnakeLowerCase(std::string_view n) { return detail::to_snake_impl<false>(n); }
[[nodiscard]] inline std::string ToSnakeUpperCase(std::string_view n) { return detail::to_snake_impl<true>(n); }

// Extra (not in C#): kebab-case, same word-boundary rule as snake_case.
[[nodiscard]] inline std::string ToKebabCase(std::string_view name) {
    if (name.empty()) return {};
    std::string out;
    out.reserve(name.size() + name.size() / 4);
    bool started = false;
    char last = '\0';
    for (char ch : name) {
        if (detail::is_space(ch) || detail::is_separator(ch)) {
            if (started && !out.empty() && out.back() != '-') out.push_back('-');
            started = true;
            last = ch;
            continue;
        }
        if (detail::is_upper(ch) && started && detail::is_lower(last)) out.push_back('-');
        out.push_back(detail::to_lower(ch));
        started = true;
        last = ch;
    }
    while (!out.empty() && out.back() == '-') out.pop_back();
    return out;
}

// Title-case (ASCII invariant). Capitalizes first letter of each whitespace-separated word.
[[nodiscard]] inline std::string ToTitleCase(std::string_view name) {
    std::string out;
    out.reserve(name.size());
    bool at_word_start = true;
    for (char ch : name) {
        if (detail::is_space(ch)) {
            out.push_back(ch);
            at_word_start = true;
            continue;
        }
        out.push_back(at_word_start ? detail::to_upper(ch) : detail::to_lower(ch));
        at_word_start = false;
    }
    return out;
}

// ============================================================================
//  Regex and simple transforms
// ============================================================================

[[nodiscard]] inline bool MatchesPattern(std::string_view input, std::string_view pattern) {
    if (input.empty())   throw std::invalid_argument("input is empty");
    if (pattern.empty()) throw std::invalid_argument("pattern is empty");
    // std::regex needs std::string; construct once.
    std::regex re{std::string(pattern)};
    return std::regex_search(input.begin(), input.end(), re);
}

// Variadic convenience — RemoveCharacters(s, 'a', 'b') or RemoveCharacters(s, std::string_view{"ab"}).
[[nodiscard]] inline std::string RemoveCharacters(std::string_view input, std::string_view chars) {
    if (input.empty()) return {};
    std::string out;
    out.reserve(input.size());
    for (char c : input) {
        if (chars.find(c) == std::string_view::npos) out.push_back(c);
    }
    return out;
}

template <std::same_as<char>... Chars>
[[nodiscard]] inline std::string RemoveCharacters(std::string_view input, Chars... cs) {
    const std::array<char, sizeof...(Chars)> table{cs...};
    if (input.empty()) return {};
    std::string out;
    out.reserve(input.size());
    for (char c : input) {
        if (std::find(table.begin(), table.end(), c) == table.end()) out.push_back(c);
    }
    return out;
}

[[nodiscard]] inline std::string Reverse(std::string_view input) {
    std::string out(input.rbegin(), input.rend());
    return out;
}

// URL slug: lowercase ASCII, [^a-z0-9\s-] removed, whitespace runs → single '-', trimmed '-'.
[[nodiscard]] inline std::string ToSlug(std::string_view input) {
    if (input.empty()) return {};
    std::string out;
    out.reserve(input.size());
    bool prev_dash = true;  // trim leading
    for (char ch : input) {
        char low = detail::to_lower(ch);
        if ((low >= 'a' && low <= 'z') || (low >= '0' && low <= '9') || low == '-') {
            out.push_back(low);
            prev_dash = (low == '-');
        } else if (detail::is_space(ch)) {
            if (!prev_dash) { out.push_back('-'); prev_dash = true; }
        }
        // anything else silently dropped
    }
    while (!out.empty() && out.back() == '-') out.pop_back();
    return out;
}

[[nodiscard]] inline std::string CapitalizeWords(std::string_view input) {
    if (input.empty()) return {};
    std::string out;
    out.reserve(input.size());
    bool word_start = true;
    for (char ch : input) {
        if (ch == ' ') { out.push_back(ch); word_start = true; continue; }
        out.push_back(word_start ? detail::to_upper(ch) : detail::to_lower(ch));
        word_start = false;
    }
    return out;
}

[[nodiscard]] inline std::string NormalizeSpaces(std::string_view input) {
    // Trim leading/trailing whitespace then collapse internal whitespace runs to a single space.
    std::size_t b = 0;
    std::size_t e = input.size();
    while (b < e && detail::is_space(input[b])) ++b;
    while (e > b && detail::is_space(input[e - 1])) --e;
    if (b == e) return {};
    std::string out;
    out.reserve(e - b);
    bool prev_space = false;
    for (std::size_t i = b; i < e; ++i) {
        char c = input[i];
        if (detail::is_space(c)) {
            if (!prev_space) out.push_back(' ');
            prev_space = true;
        } else {
            out.push_back(c);
            prev_space = false;
        }
    }
    return out;
}

[[nodiscard]] inline std::string StripTags(std::string_view input) {
    if (input.empty()) return {};
    std::string out;
    out.reserve(input.size());
    bool in_tag = false;
    for (char c : input) {
        if (c == '<')      in_tag = true;
        else if (c == '>') in_tag = false;
        else if (!in_tag)  out.push_back(c);
    }
    return out;
}

// ============================================================================
//  Extras — case-insensitive helpers (ASCII)
// ============================================================================

[[nodiscard]] inline bool EqualsIgnoreCase(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (!detail::ieq(a[i], b[i])) return false;
    return true;
}

[[nodiscard]] inline bool StartsWithIgnoreCase(std::string_view s, std::string_view prefix) noexcept {
    if (prefix.size() > s.size()) return false;
    return EqualsIgnoreCase(s.substr(0, prefix.size()), prefix);
}

[[nodiscard]] inline bool EndsWithIgnoreCase(std::string_view s, std::string_view suffix) noexcept {
    if (suffix.size() > s.size()) return false;
    return EqualsIgnoreCase(s.substr(s.size() - suffix.size()), suffix);
}

[[nodiscard]] inline bool ContainsIgnoreCase(std::string_view haystack, std::string_view needle) noexcept {
    if (needle.empty()) return true;
    if (needle.size() > haystack.size()) return false;
    const std::size_t last = haystack.size() - needle.size();
    for (std::size_t i = 0; i <= last; ++i) {
        bool ok = true;
        for (std::size_t j = 0; j < needle.size(); ++j) {
            if (!detail::ieq(haystack[i + j], needle[j])) { ok = false; break; }
        }
        if (ok) return true;
    }
    return false;
}

// ============================================================================
//  Extras — truncate, pad, repeat, left / right
// ============================================================================

[[nodiscard]] inline std::string Truncate(std::string_view input, std::size_t max_len,
                                          std::string_view ellipsis = "...") {
    if (input.size() <= max_len) return std::string(input);
    if (ellipsis.size() >= max_len) return std::string(input.substr(0, max_len));
    std::string out;
    out.reserve(max_len);
    out.append(input.substr(0, max_len - ellipsis.size()));
    out.append(ellipsis);
    return out;
}

[[nodiscard]] inline std::string PadLeft(std::string_view input, std::size_t total_width, char pad = ' ') {
    if (input.size() >= total_width) return std::string(input);
    std::string out;
    out.reserve(total_width);
    out.append(total_width - input.size(), pad);
    out.append(input);
    return out;
}

[[nodiscard]] inline std::string PadRight(std::string_view input, std::size_t total_width, char pad = ' ') {
    if (input.size() >= total_width) return std::string(input);
    std::string out;
    out.reserve(total_width);
    out.append(input);
    out.append(total_width - input.size(), pad);
    return out;
}

[[nodiscard]] inline std::string Repeat(std::string_view s, std::size_t n) {
    std::string out;
    out.reserve(s.size() * n);
    for (std::size_t i = 0; i < n; ++i) out.append(s);
    return out;
}

[[nodiscard]] constexpr std::string_view Left(std::string_view s, std::size_t n) noexcept {
    return s.substr(0, std::min(n, s.size()));
}

[[nodiscard]] constexpr std::string_view Right(std::string_view s, std::size_t n) noexcept {
    return n >= s.size() ? s : s.substr(s.size() - n);
}

// ============================================================================
//  Extras — classification / counting
// ============================================================================

[[nodiscard]] constexpr bool IsNumeric(std::string_view s) noexcept {
    if (s.empty()) return false;
    for (char c : s)
        if (!detail::is_digit(c)) return false;
    return true;
}

[[nodiscard]] constexpr bool IsAlpha(std::string_view s) noexcept {
    if (s.empty()) return false;
    for (char c : s)
        if (!detail::is_alpha(c)) return false;
    return true;
}

[[nodiscard]] constexpr bool IsAlphanumeric(std::string_view s) noexcept {
    if (s.empty()) return false;
    for (char c : s)
        if (!detail::is_alnum(c)) return false;
    return true;
}

[[nodiscard]] constexpr std::size_t WordCount(std::string_view s) noexcept {
    std::size_t count = 0;
    bool in_word = false;
    for (char c : s) {
        if (detail::is_space(c)) { in_word = false; }
        else if (!in_word)       { in_word = true; ++count; }
    }
    return count;
}

[[nodiscard]] constexpr std::size_t LineCount(std::string_view s) noexcept {
    if (s.empty()) return 0;
    std::size_t n = 1;
    for (char c : s) if (c == '\n') ++n;
    return n;
}

// ============================================================================
//  Extras — Split (eager std::vector<string_view>) and SplitView (lazy range)
// ============================================================================

[[nodiscard]] inline std::vector<std::string_view> Split(std::string_view s, char sep,
                                                         bool keep_empty = true) {
    std::vector<std::string_view> out;
    std::size_t start = 0;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == sep) {
            if (keep_empty || i > start) out.emplace_back(s.data() + start, i - start);
            start = i + 1;
        }
    }
    if (keep_empty || start < s.size()) out.emplace_back(s.data() + start, s.size() - start);
    return out;
}

// Lazy lightweight split — returns a std::ranges::split_view-like range.
[[nodiscard]] inline auto SplitView(std::string_view s, char sep) {
    return s | std::views::split(sep) | std::views::transform([](auto&& sub) {
        return std::string_view(&*sub.begin(), static_cast<std::size_t>(std::ranges::distance(sub)));
    });
}

// ============================================================================
//  Extras — ReplaceAll
// ============================================================================

[[nodiscard]] inline std::string ReplaceAll(std::string_view input, std::string_view from,
                                            std::string_view to) {
    if (from.empty()) return std::string(input);
    std::string out;
    out.reserve(input.size());
    std::size_t pos = 0;
    while (pos <= input.size()) {
        const std::size_t hit = input.find(from, pos);
        if (hit == std::string_view::npos) { out.append(input.substr(pos)); break; }
        out.append(input.substr(pos, hit - pos));
        out.append(to);
        pos = hit + from.size();
    }
    return out;
}

// ============================================================================
//  Extras — Levenshtein distance (two-row DP, O(min(m,n)) memory)
// ============================================================================

[[nodiscard]] inline std::size_t LevenshteinDistance(std::string_view a, std::string_view b) {
    if (a.size() < b.size()) std::swap(a, b);
    if (b.empty()) return a.size();
    std::vector<std::size_t> prev(b.size() + 1), curr(b.size() + 1);
    for (std::size_t j = 0; j <= b.size(); ++j) prev[j] = j;
    for (std::size_t i = 1; i <= a.size(); ++i) {
        curr[0] = i;
        for (std::size_t j = 1; j <= b.size(); ++j) {
            const std::size_t cost = (a[i - 1] == b[j - 1]) ? 0U : 1U;
            curr[j] = std::min({curr[j - 1] + 1, prev[j] + 1, prev[j - 1] + cost});
        }
        prev.swap(curr);
    }
    return prev[b.size()];
}

// ============================================================================
//  Extras — Base64 (RFC 4648, no line breaks)
// ============================================================================

namespace detail {
inline constexpr std::array<char, 64> b64_alphabet{
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'};

// 256-entry decode table initialized at compile time.
inline constexpr auto make_b64_decode_table() {
    std::array<std::int8_t, 256> t{};
    for (auto& v : t) v = -1;
    for (std::int8_t i = 0; i < 64; ++i) t[static_cast<std::uint8_t>(b64_alphabet[i])] = i;
    return t;
}
inline constexpr auto b64_decode_table = make_b64_decode_table();
}  // namespace detail

[[nodiscard]] inline std::string ToBase64(std::string_view input) {
    std::string out;
    out.reserve(((input.size() + 2) / 3) * 4);
    std::size_t i = 0;
    while (i + 3 <= input.size()) {
        const auto b0 = static_cast<std::uint8_t>(input[i]);
        const auto b1 = static_cast<std::uint8_t>(input[i + 1]);
        const auto b2 = static_cast<std::uint8_t>(input[i + 2]);
        out.push_back(detail::b64_alphabet[b0 >> 2]);
        out.push_back(detail::b64_alphabet[((b0 & 0x03) << 4) | (b1 >> 4)]);
        out.push_back(detail::b64_alphabet[((b1 & 0x0F) << 2) | (b2 >> 6)]);
        out.push_back(detail::b64_alphabet[b2 & 0x3F]);
        i += 3;
    }
    if (i < input.size()) {
        const auto b0 = static_cast<std::uint8_t>(input[i]);
        const std::uint8_t b1 = (i + 1 < input.size()) ? static_cast<std::uint8_t>(input[i + 1]) : 0;
        out.push_back(detail::b64_alphabet[b0 >> 2]);
        out.push_back(detail::b64_alphabet[((b0 & 0x03) << 4) | (b1 >> 4)]);
        if (i + 1 < input.size()) out.push_back(detail::b64_alphabet[(b1 & 0x0F) << 2]);
        else                      out.push_back('=');
        out.push_back('=');
    }
    return out;
}

[[nodiscard]] inline std::optional<std::string> FromBase64(std::string_view input) {
    if (input.size() % 4 != 0) return std::nullopt;
    std::string out;
    out.reserve((input.size() / 4) * 3);
    for (std::size_t i = 0; i < input.size(); i += 4) {
        std::int32_t v[4];
        for (int k = 0; k < 4; ++k) {
            char c = input[i + k];
            if (c == '=') { v[k] = 0; continue; }
            auto d = detail::b64_decode_table[static_cast<std::uint8_t>(c)];
            if (d < 0) return std::nullopt;
            v[k] = d;
        }
        out.push_back(static_cast<char>((v[0] << 2) | (v[1] >> 4)));
        if (input[i + 2] != '=') out.push_back(static_cast<char>(((v[1] & 0x0F) << 4) | (v[2] >> 2)));
        if (input[i + 3] != '=') out.push_back(static_cast<char>(((v[2] & 0x03) << 6) | v[3]));
    }
    return out;
}

// ============================================================================
//  Join — collection → string (with/without formatter)
// ============================================================================

template <std::ranges::input_range R, class Fn>
    requires std::invocable<Fn&, std::ranges::range_reference_t<R>>
[[nodiscard]] inline std::string Join(R&& range, Fn formatter, std::string_view separator = "") {
    std::string out;
    bool first = true;
    for (auto&& item : range) {
        if (!first) out.append(separator);
        out.append(static_cast<std::string>(formatter(item)));
        first = false;
    }
    return out;
}

template <std::ranges::input_range R>
[[nodiscard]] inline std::string Join(R&& range, std::string_view separator = "") {
    using T = std::ranges::range_value_t<R>;
    if constexpr (std::convertible_to<T, std::string_view>) {
        std::string out;
        bool first = true;
        for (auto&& item : range) {
            if (!first) out.append(separator);
            out.append(std::string_view(item));
            first = false;
        }
        return out;
    } else {
        return Join(std::forward<R>(range),
                    [](const auto& v) { return std::to_string(v); }, separator);
    }
}

// ============================================================================
//  ste::str — fluent wrapper exposing PascalCase methods matching the C# API
// ============================================================================

class str : public std::string {
public:
    using std::string::string;

    str() = default;
    str(const std::string& s) : std::string(s) {}
    str(std::string&& s) noexcept : std::string(std::move(s)) {}
    str(std::string_view sv) : std::string(sv) {}
    str(const char* s) : std::string(s) {}

    [[nodiscard]] str ToCamelCase()       const { return str(ste::ToCamelCase(*this)); }
    [[nodiscard]] str ToPascalCase()      const { return str(ste::ToPascalCase(*this)); }
    [[nodiscard]] str ToSnakeLowerCase()  const { return str(ste::ToSnakeLowerCase(*this)); }
    [[nodiscard]] str ToSnakeUpperCase()  const { return str(ste::ToSnakeUpperCase(*this)); }
    [[nodiscard]] str ToKebabCase()       const { return str(ste::ToKebabCase(*this)); }
    [[nodiscard]] str ToTitleCase()       const { return str(ste::ToTitleCase(*this)); }

    [[nodiscard]] bool MatchesPattern(std::string_view p) const { return ste::MatchesPattern(*this, p); }

    [[nodiscard]] str RemoveCharacters(std::string_view chars) const {
        return str(ste::RemoveCharacters(*this, chars));
    }
    template <std::same_as<char>... Cs>
    [[nodiscard]] str RemoveCharacters(Cs... cs) const {
        return str(ste::RemoveCharacters(std::string_view(*this), cs...));
    }

    [[nodiscard]] str Reverse()          const { return str(ste::Reverse(*this)); }
    [[nodiscard]] str ToSlug()           const { return str(ste::ToSlug(*this)); }
    [[nodiscard]] str CapitalizeWords()  const { return str(ste::CapitalizeWords(*this)); }
    [[nodiscard]] str NormalizeSpaces()  const { return str(ste::NormalizeSpaces(*this)); }
    [[nodiscard]] str StripTags()        const { return str(ste::StripTags(*this)); }

    [[nodiscard]] bool IsNullOrEmpty()       const noexcept { return ste::IsNullOrEmpty(*this); }
    [[nodiscard]] bool IsNullOrWhiteSpace()  const noexcept { return ste::IsNullOrWhiteSpace(*this); }

    [[nodiscard]] bool EqualsIgnoreCase(std::string_view o)     const noexcept { return ste::EqualsIgnoreCase(*this, o); }
    [[nodiscard]] bool StartsWithIgnoreCase(std::string_view o) const noexcept { return ste::StartsWithIgnoreCase(*this, o); }
    [[nodiscard]] bool EndsWithIgnoreCase(std::string_view o)   const noexcept { return ste::EndsWithIgnoreCase(*this, o); }
    [[nodiscard]] bool ContainsIgnoreCase(std::string_view o)   const noexcept { return ste::ContainsIgnoreCase(*this, o); }

    [[nodiscard]] str Truncate(std::size_t n, std::string_view ell = "...") const { return str(ste::Truncate(*this, n, ell)); }
    [[nodiscard]] str PadLeft(std::size_t w, char c = ' ')                  const { return str(ste::PadLeft(*this, w, c)); }
    [[nodiscard]] str PadRight(std::size_t w, char c = ' ')                 const { return str(ste::PadRight(*this, w, c)); }
    [[nodiscard]] str Repeat(std::size_t n) const { return str(ste::Repeat(*this, n)); }
    [[nodiscard]] std::string_view Left(std::size_t n)  const noexcept { return ste::Left(*this, n); }
    [[nodiscard]] std::string_view Right(std::size_t n) const noexcept { return ste::Right(*this, n); }

    [[nodiscard]] bool IsNumeric()       const noexcept { return ste::IsNumeric(*this); }
    [[nodiscard]] bool IsAlpha()         const noexcept { return ste::IsAlpha(*this); }
    [[nodiscard]] bool IsAlphanumeric()  const noexcept { return ste::IsAlphanumeric(*this); }
    [[nodiscard]] std::size_t WordCount() const noexcept { return ste::WordCount(*this); }
    [[nodiscard]] std::size_t LineCount() const noexcept { return ste::LineCount(*this); }

    [[nodiscard]] std::vector<std::string_view> Split(char sep, bool keep_empty = true) const {
        return ste::Split(*this, sep, keep_empty);
    }
    [[nodiscard]] str ReplaceAll(std::string_view from, std::string_view to) const {
        return str(ste::ReplaceAll(*this, from, to));
    }
    [[nodiscard]] std::size_t LevenshteinDistance(std::string_view o) const {
        return ste::LevenshteinDistance(*this, o);
    }
    [[nodiscard]] str ToBase64()                       const { return str(ste::ToBase64(*this)); }
    [[nodiscard]] std::optional<str> FromBase64()      const {
        auto r = ste::FromBase64(*this);
        return r ? std::optional<str>{str(*r)} : std::nullopt;
    }
};

}  // namespace ste
