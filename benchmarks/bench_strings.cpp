// SPDX-License-Identifier: MIT
// Benchmarks comparing "naive C++" implementations (regex-based, multi-pass,
// no buffer reservation) against the ste:: single-pass, pre-reserved variants.
//
// Run:  ./stellar_bench --benchmark_filter=.*
#include <benchmark/benchmark.h>
#include <ste/strings.hpp>

#include <algorithm>
#include <cctype>
#include <regex>
#include <string>
#include <string_view>

// -------- Sample corpus ---------------------------------------------------------
static const std::string kLorem =
    "  Lorem   ipsum DOLOR sit amet, consectetur   adipiscing ELIT, sed do "
    "<b>eiusmod</b> tempor incididunt ut <i>labore</i> et dolore magna "
    "aliqua.  Ut enim ad minim veniam, quis nostrud exercitation   ";

// =============================================================================
//  NormalizeSpaces: naive (regex) vs ste (state-machine)
// =============================================================================

static std::string naive_normalize_spaces(std::string_view s) {
    std::string trimmed(s);
    // trim
    auto not_space = [](unsigned char c){ return !std::isspace(c); };
    auto b = std::find_if(trimmed.begin(), trimmed.end(), not_space);
    trimmed.erase(trimmed.begin(), b);
    auto e = std::find_if(trimmed.rbegin(), trimmed.rend(), not_space).base();
    trimmed.erase(e, trimmed.end());
    // collapse with std::regex
    static const std::regex re{R"(\s+)"};
    return std::regex_replace(trimmed, re, " ");
}

static void BM_Naive_NormalizeSpaces(benchmark::State& state) {
    for (auto _ : state) {
        auto r = naive_normalize_spaces(kLorem);
        benchmark::DoNotOptimize(r);
    }
}
static void BM_Ste_NormalizeSpaces(benchmark::State& state) {
    for (auto _ : state) {
        auto r = ste::NormalizeSpaces(kLorem);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Naive_NormalizeSpaces);
BENCHMARK(BM_Ste_NormalizeSpaces);

// =============================================================================
//  StripTags: naive (regex) vs ste (state-machine)
// =============================================================================

static std::string naive_strip_tags(std::string_view s) {
    static const std::regex re{"<.*?>"};
    return std::regex_replace(std::string(s), re, "");
}

static void BM_Naive_StripTags(benchmark::State& state) {
    for (auto _ : state) {
        auto r = naive_strip_tags(kLorem);
        benchmark::DoNotOptimize(r);
    }
}
static void BM_Ste_StripTags(benchmark::State& state) {
    for (auto _ : state) {
        auto r = ste::StripTags(kLorem);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Naive_StripTags);
BENCHMARK(BM_Ste_StripTags);

// =============================================================================
//  ToSlug: naive (double regex + ToLower) vs ste (one pass)
// =============================================================================

static std::string naive_to_slug(std::string_view input) {
    std::string s(input);
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    static const std::regex inv{"[^a-z0-9\\s-]"};
    s = std::regex_replace(s, inv, "");
    static const std::regex sp{R"(\s+)"};
    s = std::regex_replace(s, sp, "-");
    // trim '-'
    auto not_dash = [](char c){ return c != '-'; };
    auto b = std::find_if(s.begin(), s.end(), not_dash);
    s.erase(s.begin(), b);
    auto e = std::find_if(s.rbegin(), s.rend(), not_dash).base();
    s.erase(e, s.end());
    return s;
}

static void BM_Naive_ToSlug(benchmark::State& state) {
    for (auto _ : state) {
        auto r = naive_to_slug("Hello, World! This is a Test 100%");
        benchmark::DoNotOptimize(r);
    }
}
static void BM_Ste_ToSlug(benchmark::State& state) {
    for (auto _ : state) {
        auto r = ste::ToSlug("Hello, World! This is a Test 100%");
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Naive_ToSlug);
BENCHMARK(BM_Ste_ToSlug);

// =============================================================================
//  ToPascalCase: naive (split + join) vs ste (single pass)
// =============================================================================

static std::string naive_pascal(std::string_view s) {
    std::string out;
    std::string current;
    auto flush = [&]{
        if (!current.empty()) {
            current[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(current[0])));
            for (std::size_t i = 1; i < current.size(); ++i)
                current[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(current[i])));
            out += current;
            current.clear();
        }
    };
    for (char c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) flush();
        else current.push_back(c);
    }
    flush();
    return out;
}

static void BM_Naive_ToPascalCase(benchmark::State& state) {
    for (auto _ : state) {
        auto r = naive_pascal("my awesome variable name");
        benchmark::DoNotOptimize(r);
    }
}
static void BM_Ste_ToPascalCase(benchmark::State& state) {
    for (auto _ : state) {
        auto r = ste::ToPascalCase("my awesome variable name");
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Naive_ToPascalCase);
BENCHMARK(BM_Ste_ToPascalCase);

// =============================================================================
//  ReplaceAll: naive (regex) vs ste (linear)
// =============================================================================

static std::string naive_replace_all(std::string_view input, std::string_view from, std::string_view to) {
    return std::regex_replace(std::string(input), std::regex(std::string(from)), std::string(to));
}

static void BM_Naive_ReplaceAll(benchmark::State& state) {
    for (auto _ : state) {
        auto r = naive_replace_all(kLorem, "ipsum", "IPSUM");
        benchmark::DoNotOptimize(r);
    }
}
static void BM_Ste_ReplaceAll(benchmark::State& state) {
    for (auto _ : state) {
        auto r = ste::ReplaceAll(kLorem, "ipsum", "IPSUM");
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Naive_ReplaceAll);
BENCHMARK(BM_Ste_ReplaceAll);

// =============================================================================
//  Base64 encode (sanity: ste:: only — no naive competitor in std::)
// =============================================================================

static void BM_Ste_Base64(benchmark::State& state) {
    const std::string payload(1024, 'x');
    for (auto _ : state) {
        auto r = ste::ToBase64(payload);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Ste_Base64);
