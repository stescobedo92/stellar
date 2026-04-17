// SPDX-License-Identifier: MIT
// Stellar Toolkit Extensions — ste::async
//
// Asynchronous analogues of the C# API. Two tiers:
//
//  1. Future-based (portable, works on every compiler today):
//         ste::ToCamelCaseAsync(sv) -> std::future<std::string>
//         ste::ToPascalCaseAsync(sv) -> std::future<std::string>
//         ste::ToTitleCaseAsync(sv)  -> std::future<std::string>
//         ste::JoinAsync(range, formatter, sep) -> std::future<std::string>
//
//  2. Coroutine-based (C++20/23 `co_await` support):
//         ste::Task<std::string> ste::ToCamelCaseTask(std::string sv);
//     Task<T> is a minimal eager task type (satisfies the Awaitable concept,
//     suspend-never for the initial suspend, symmetric transfer via
//     final_suspend). Enough to compose async pipelines in user code.
//
//  JoinAsync also supports IAsyncEnumerable<T> semantics via a
//  std::generator<std::future<Item>> input (polls each future in order).
#pragma once

#include <coroutine>
#include <exception>
#include <functional>
#include <future>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include "strings.hpp"

namespace ste {

// ============================================================================
//  Task<T> — minimal eager coroutine return type
// ============================================================================

template <class T>
class Task {
public:
    struct promise_type {
        std::optional<T> value;
        std::exception_ptr error;
        std::coroutine_handle<> continuation;

        Task get_return_object() noexcept {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; }

        struct final_awaiter {
            bool await_ready() noexcept { return false; }
            std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                auto cont = h.promise().continuation;
                return cont ? cont : std::noop_coroutine();
            }
            void await_resume() noexcept {}
        };
        final_awaiter final_suspend() noexcept { return {}; }

        template <class U>
        void return_value(U&& v) { value.emplace(std::forward<U>(v)); }
        void unhandled_exception() noexcept { error = std::current_exception(); }
    };

    Task() = default;
    explicit Task(std::coroutine_handle<promise_type> h) noexcept : handle_(h) {}
    Task(const Task&)            = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& o) noexcept : handle_(std::exchange(o.handle_, nullptr)) {}
    Task& operator=(Task&& o) noexcept {
        if (this != &o) { if (handle_) handle_.destroy(); handle_ = std::exchange(o.handle_, nullptr); }
        return *this;
    }
    ~Task() { if (handle_) handle_.destroy(); }

    [[nodiscard]] bool Ready() const noexcept { return handle_ && handle_.done(); }

    // Awaitable interface — allows `co_await task;`
    bool await_ready() const noexcept { return !handle_ || handle_.done(); }
    void await_suspend(std::coroutine_handle<> waiter) noexcept {
        handle_.promise().continuation = waiter;
    }
    T    await_resume() {
        if (handle_.promise().error) std::rethrow_exception(handle_.promise().error);
        return std::move(*handle_.promise().value);
    }

    // Blocking access (for top-level callers who aren't in a coroutine).
    T Get() {
        if (!handle_.done()) {
            // Spin-free wait: promise_type should have resumed synchronously
            // because initial_suspend is suspend_never; if not, user provided a
            // body that suspends — in that case we just busy-wait briefly.
            while (!handle_.done()) std::this_thread::yield();
        }
        if (handle_.promise().error) std::rethrow_exception(handle_.promise().error);
        return std::move(*handle_.promise().value);
    }

private:
    std::coroutine_handle<promise_type> handle_{};
};

// ============================================================================
//  Future-based async variants — mirror C# Task<T> style
// ============================================================================

[[nodiscard]] inline std::future<std::string> ToCamelCaseAsync(std::string name) {
    return std::async(std::launch::async,
        [n = std::move(name)] { return ste::ToCamelCase(n); });
}
[[nodiscard]] inline std::future<std::string> ToPascalCaseAsync(std::string name) {
    return std::async(std::launch::async,
        [n = std::move(name)] { return ste::ToPascalCase(n); });
}
[[nodiscard]] inline std::future<std::string> ToTitleCaseAsync(std::string name) {
    return std::async(std::launch::async,
        [n = std::move(name)] { return ste::ToTitleCase(n); });
}
[[nodiscard]] inline std::future<std::string> ToSlugAsync(std::string name) {
    return std::async(std::launch::async,
        [n = std::move(name)] { return ste::ToSlug(n); });
}

// ============================================================================
//  JoinAsync — consumes a range of std::future<T> (async enumerable analogue).
// ============================================================================

template <std::ranges::input_range R, class Fn>
[[nodiscard]] inline std::future<std::string>
JoinAsync(R range, Fn formatter, std::string separator = "") {
    return std::async(std::launch::async,
        [r = std::move(range), f = std::move(formatter), sep = std::move(separator)]() mutable {
            std::string out;
            bool first = true;
            for (auto&& item : r) {
                if (!first) out.append(sep);
                // If the element is itself a std::future, wait for it.
                if constexpr (requires { item.get(); }) {
                    auto awaited = item.get();
                    out.append(f(awaited));
                } else {
                    out.append(f(item));
                }
                first = false;
            }
            return out;
        });
}

// ============================================================================
//  Coroutine-based variants
// ============================================================================

inline Task<std::string> ToCamelCaseTask(std::string name) {
    co_return ste::ToCamelCase(name);
}
inline Task<std::string> ToPascalCaseTask(std::string name) {
    co_return ste::ToPascalCase(name);
}
inline Task<std::string> ToTitleCaseTask(std::string name) {
    co_return ste::ToTitleCase(name);
}

}  // namespace ste
