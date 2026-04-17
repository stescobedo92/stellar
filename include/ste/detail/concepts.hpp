// SPDX-License-Identifier: MIT
// Stellar Toolkit Extensions — ste::detail concepts
#pragma once

#include <concepts>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>

namespace ste::detail {

// Anything implicitly convertible to std::string_view (std::string, literals, etc.)
template <class T>
concept string_like = std::convertible_to<const T&, std::string_view>;

// A range whose value type can be formatted to std::string via a provided callable.
template <class R>
concept input_range_of_values = std::ranges::input_range<R>;

// Callable producing a std::string (or convertible) given a T.
template <class F, class T>
concept string_formatter = requires(F f, const T& v) {
    { f(v) } -> std::convertible_to<std::string>;
};

// An associative container whose mapped_type acts like a sequence container
// supporting push_back / emplace_back (List-like).
template <class M>
concept dict_of_pushable = requires {
    typename M::key_type;
    typename M::mapped_type;
} && requires(typename M::mapped_type& v, typename M::mapped_type::value_type i) {
    v.push_back(i);
};

template <class M>
concept dict_of_insertable_set = requires {
    typename M::key_type;
    typename M::mapped_type;
} && requires(typename M::mapped_type& v, typename M::mapped_type::value_type i) {
    v.insert(i);
} && !dict_of_pushable<M>;

}  // namespace ste::detail
