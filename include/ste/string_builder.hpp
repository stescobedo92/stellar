// SPDX-License-Identifier: MIT
// Stellar Toolkit Extensions — ste::StringBuilder
//
// C++ analogue of System.Text.StringBuilder. Internally a std::string with
// geometric-growth semantics; exposes the same fluent interface as the
// C# extensions (AppendFormatIf / AppendLineIf / InsertJoin / AppendRepeated).
#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <format>
#include <functional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

namespace ste {

class StringBuilder {
public:
    StringBuilder() = default;
    explicit StringBuilder(std::size_t reserve_bytes) { buffer_.reserve(reserve_bytes); }
    explicit StringBuilder(std::string_view initial) : buffer_(initial) {}

    // -- Basic appends ------------------------------------------------------
    StringBuilder& Append(std::string_view s)              { buffer_.append(s); return *this; }
    StringBuilder& Append(char c)                          { buffer_.push_back(c); return *this; }
    StringBuilder& AppendLine()                            { buffer_.push_back('\n'); return *this; }
    StringBuilder& AppendLine(std::string_view s)          { buffer_.append(s).push_back('\n'); return *this; }

    // std::format passthrough — type-safe, no varargs (no printf footguns).
    template <class... Args>
    StringBuilder& AppendFormat(std::format_string<Args...> fmt, Args&&... args) {
        std::format_to(std::back_inserter(buffer_), fmt, std::forward<Args>(args)...);
        return *this;
    }

    // -- Repetition ---------------------------------------------------------
    StringBuilder& AppendRepeated(std::string_view value, std::size_t count) {
        buffer_.reserve(buffer_.size() + value.size() * count);
        for (std::size_t i = 0; i < count; ++i) buffer_.append(value);
        return *this;
    }
    StringBuilder& AppendRepeated(char value, std::size_t count) {
        buffer_.append(count, value);
        return *this;
    }

    // -- Conditional appends (mirrors C#) -----------------------------------
    template <class IfFn, class ElseFn = std::nullptr_t>
    StringBuilder& AppendFormatIf(bool condition, IfFn&& if_fn, ElseFn&& else_fn = nullptr) {
        if (condition) if_fn(*this);
        else if constexpr (!std::same_as<std::remove_cvref_t<ElseFn>, std::nullptr_t>) else_fn(*this);
        return *this;
    }

    template <class Getter>
        requires std::invocable<Getter&>
    StringBuilder& AppendLineIf(bool condition, Getter&& getter) {
        if (condition) AppendLine(getter());
        return *this;
    }

    StringBuilder& AppendIf(bool condition, std::string_view s) {
        if (condition) buffer_.append(s);
        return *this;
    }

    // -- Insertion ----------------------------------------------------------
    StringBuilder& Insert(std::size_t index, std::string_view s) {
        buffer_.insert(index, s);
        return *this;
    }

    // Joins items (with optional formatter) into the builder at a start index.
    template <std::ranges::input_range R, class Fn>
    StringBuilder& InsertJoin(std::size_t start_index, R&& range, Fn formatter,
                              std::string_view separator = "") {
        std::string piece;
        bool first = true;
        for (auto&& v : range) {
            if (!first) piece.append(separator);
            piece.append(static_cast<std::string>(formatter(v)));
            first = false;
        }
        buffer_.insert(start_index, piece);
        return *this;
    }

    template <std::ranges::input_range R>
    StringBuilder& InsertJoin(std::size_t start_index, R&& range,
                              std::string_view separator = "") {
        using T = std::ranges::range_value_t<R>;
        if constexpr (std::convertible_to<T, std::string_view>) {
            std::string piece;
            bool first = true;
            for (auto&& v : range) {
                if (!first) piece.append(separator);
                piece.append(std::string_view(v));
                first = false;
            }
            buffer_.insert(start_index, piece);
        } else {
            return InsertJoin(start_index, std::forward<R>(range),
                              [](const auto& x) { return std::to_string(x); }, separator);
        }
        return *this;
    }

    // -- Access / mutation --------------------------------------------------
    [[nodiscard]] std::size_t      Length() const noexcept   { return buffer_.size(); }
    [[nodiscard]] bool             Empty()  const noexcept   { return buffer_.empty(); }
    [[nodiscard]] std::string_view View()   const noexcept   { return buffer_; }
    [[nodiscard]] const std::string& ToString() const noexcept { return buffer_; }
    [[nodiscard]] std::string MoveToString() noexcept { return std::move(buffer_); }

    void Clear()  noexcept { buffer_.clear(); }
    void Reserve(std::size_t n) { buffer_.reserve(n); }

    StringBuilder& Remove(std::size_t start, std::size_t count) {
        buffer_.erase(start, count);
        return *this;
    }

    StringBuilder& Replace(std::string_view from, std::string_view to) {
        if (from.empty()) return *this;
        std::string out;
        out.reserve(buffer_.size());
        std::size_t pos = 0;
        while (pos <= buffer_.size()) {
            auto hit = buffer_.find(from, pos);
            if (hit == std::string::npos) { out.append(buffer_, pos); break; }
            out.append(buffer_, pos, hit - pos);
            out.append(to);
            pos = hit + from.size();
        }
        buffer_ = std::move(out);
        return *this;
    }

private:
    std::string buffer_;
};

}  // namespace ste
