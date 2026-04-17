// SPDX-License-Identifier: MIT
// Stellar Toolkit Extensions — ste::collections
//
// Collection helpers that replicate the BinaryCoffee.Extensions C# surface
// (Join, AddNested) and add a curated set of professional utilities
// (Chunk, Partition, DistinctBy, MinBy, MaxBy, GroupBy, ToDictionary).
#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "detail/concepts.hpp"
#include "strings.hpp"  // Join lives in strings.hpp so both headers stay consistent

namespace ste {

// ============================================================================
//  AddNested — mirror of C#:
//      dict.AddNested(key, value)
//  If key exists, appends to the existing container. Otherwise creates one.
//  Works with map<K, vector<V>>, map<K, set<V>>, unordered_map<K, list<V>>, etc.
// ============================================================================

template <class Map, class K, class V>
inline void AddNested(Map& m, K&& key, V&& value) {
    auto [it, inserted] = m.try_emplace(std::forward<K>(key));
    auto& coll = it->second;
    if constexpr (requires(decltype(coll)& c, V&& v) { c.emplace_back(std::forward<V>(v)); }) {
        coll.emplace_back(std::forward<V>(value));
    } else if constexpr (requires(decltype(coll)& c, V&& v) { c.insert(std::forward<V>(v)); }) {
        coll.insert(std::forward<V>(value));
    } else if constexpr (requires(decltype(coll)& c, V&& v) { c.push_back(std::forward<V>(v)); }) {
        coll.push_back(std::forward<V>(value));
    } else {
        static_assert(sizeof(V*) == 0,
            "ste::AddNested: mapped_type has no push_back/emplace_back/insert");
    }
}

// ============================================================================
//  Chunk — split a range into N-sized windows (eager, allocates one vector
//  per chunk; use std::views::chunk when you want a lazy variant).
// ============================================================================

template <std::ranges::input_range R>
[[nodiscard]] inline auto Chunk(R&& range, std::size_t size) {
    using T = std::ranges::range_value_t<R>;
    std::vector<std::vector<T>> out;
    if (size == 0) return out;
    std::vector<T> current;
    current.reserve(size);
    for (auto&& v : range) {
        current.emplace_back(std::forward<decltype(v)>(v));
        if (current.size() == size) {
            out.emplace_back(std::move(current));
            current.clear();
            current.reserve(size);
        }
    }
    if (!current.empty()) out.emplace_back(std::move(current));
    return out;
}

// ============================================================================
//  Partition — split into two vectors by predicate. First = matches, second = rest.
// ============================================================================

template <std::ranges::input_range R, class Pred>
[[nodiscard]] inline auto Partition(R&& range, Pred pred) {
    using T = std::ranges::range_value_t<R>;
    std::pair<std::vector<T>, std::vector<T>> result;
    for (auto&& v : range) {
        if (pred(v)) result.first.emplace_back(std::forward<decltype(v)>(v));
        else         result.second.emplace_back(std::forward<decltype(v)>(v));
    }
    return result;
}

// ============================================================================
//  DistinctBy — keep first occurrence of each unique key(elem).
// ============================================================================

template <std::ranges::input_range R, class KeyFn>
[[nodiscard]] inline auto DistinctBy(R&& range, KeyFn key) {
    using T   = std::ranges::range_value_t<R>;
    using Key = std::remove_cvref_t<std::invoke_result_t<KeyFn&, T const&>>;
    std::unordered_set<Key> seen;
    std::vector<T> out;
    for (auto&& v : range) {
        auto k = key(v);
        if (seen.insert(k).second) out.emplace_back(std::forward<decltype(v)>(v));
    }
    return out;
}

// ============================================================================
//  MinBy / MaxBy — return optional<T> of element with min/max projection.
// ============================================================================

template <std::ranges::input_range R, class Proj>
[[nodiscard]] inline auto MinBy(R&& range, Proj proj) -> std::optional<std::ranges::range_value_t<R>> {
    auto it = std::ranges::begin(range);
    auto e  = std::ranges::end(range);
    if (it == e) return std::nullopt;
    auto best = *it;
    auto best_key = proj(best);
    for (++it; it != e; ++it) {
        auto k = proj(*it);
        if (k < best_key) { best = *it; best_key = std::move(k); }
    }
    return best;
}

template <std::ranges::input_range R, class Proj>
[[nodiscard]] inline auto MaxBy(R&& range, Proj proj) -> std::optional<std::ranges::range_value_t<R>> {
    auto it = std::ranges::begin(range);
    auto e  = std::ranges::end(range);
    if (it == e) return std::nullopt;
    auto best = *it;
    auto best_key = proj(best);
    for (++it; it != e; ++it) {
        auto k = proj(*it);
        if (k > best_key) { best = *it; best_key = std::move(k); }
    }
    return best;
}

// ============================================================================
//  GroupBy — std::unordered_map<Key, std::vector<T>>
// ============================================================================

template <std::ranges::input_range R, class KeyFn>
[[nodiscard]] inline auto GroupBy(R&& range, KeyFn key) {
    using T   = std::ranges::range_value_t<R>;
    using Key = std::remove_cvref_t<std::invoke_result_t<KeyFn&, T const&>>;
    std::unordered_map<Key, std::vector<T>> out;
    for (auto&& v : range) {
        out[key(v)].emplace_back(std::forward<decltype(v)>(v));
    }
    return out;
}

// ============================================================================
//  ToDictionary / ToUnorderedMap
// ============================================================================

template <std::ranges::input_range R, class KeyFn, class ValueFn>
[[nodiscard]] inline auto ToDictionary(R&& range, KeyFn key, ValueFn value) {
    using T   = std::ranges::range_value_t<R>;
    using Key = std::remove_cvref_t<std::invoke_result_t<KeyFn&,   T const&>>;
    using Val = std::remove_cvref_t<std::invoke_result_t<ValueFn&, T const&>>;
    std::unordered_map<Key, Val> out;
    for (auto&& v : range) out.emplace(key(v), value(v));
    return out;
}

// ============================================================================
//  ForEach — applies function to each element in range (mutable support).
// ============================================================================

template <std::ranges::input_range R, class Fn>
inline void ForEach(R&& range, Fn fn) {
    for (auto&& v : range) fn(std::forward<decltype(v)>(v));
}

// ============================================================================
//  None / Any / All — short-circuit predicates
// ============================================================================

template <std::ranges::input_range R, class Pred>
[[nodiscard]] constexpr bool Any(R&& range, Pred pred) {
    for (auto&& v : range) if (pred(v)) return true;
    return false;
}
template <std::ranges::input_range R, class Pred>
[[nodiscard]] constexpr bool All(R&& range, Pred pred) {
    for (auto&& v : range) if (!pred(v)) return false;
    return true;
}
template <std::ranges::input_range R, class Pred>
[[nodiscard]] constexpr bool None(R&& range, Pred pred) {
    return !Any(std::forward<R>(range), std::move(pred));
}

}  // namespace ste
