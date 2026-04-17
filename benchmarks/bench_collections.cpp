// SPDX-License-Identifier: MIT
#include <benchmark/benchmark.h>
#include <ste/collections.hpp>

#include <unordered_map>
#include <vector>

// =============================================================================
//  AddNested vs naive (contains + emplace)
// =============================================================================

static void BM_Naive_AddNested(benchmark::State& state) {
    for (auto _ : state) {
        std::unordered_map<int, std::vector<int>> m;
        for (int i = 0; i < 1000; ++i) {
            auto it = m.find(i % 10);  // 1st hash
            if (it == m.end()) m[i % 10] = std::vector<int>{i};  // 2nd hash
            else it->second.push_back(i);
        }
        benchmark::DoNotOptimize(m);
    }
}

static void BM_Ste_AddNested(benchmark::State& state) {
    for (auto _ : state) {
        std::unordered_map<int, std::vector<int>> m;
        for (int i = 0; i < 1000; ++i) ste::AddNested(m, i % 10, i);  // single hash via try_emplace
        benchmark::DoNotOptimize(m);
    }
}

BENCHMARK(BM_Naive_AddNested);
BENCHMARK(BM_Ste_AddNested);

// =============================================================================
//  Join — ste vs naive using std::accumulate-style concat
// =============================================================================

static void BM_Naive_Join(benchmark::State& state) {
    std::vector<std::string> v(100, "hello");
    for (auto _ : state) {
        std::string out;
        for (std::size_t i = 0; i < v.size(); ++i) {
            if (i) out += ", ";
            out += v[i];
        }
        benchmark::DoNotOptimize(out);
    }
}
static void BM_Ste_Join(benchmark::State& state) {
    std::vector<std::string> v(100, "hello");
    for (auto _ : state) {
        auto out = ste::Join(v, ", ");
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(BM_Naive_Join);
BENCHMARK(BM_Ste_Join);
