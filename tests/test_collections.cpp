// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include <ste/collections.hpp>

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

TEST(Collections, AddNested_VectorMap) {
    std::unordered_map<std::string, std::vector<int>> m;
    ste::AddNested(m, "a", 1);
    ste::AddNested(m, "a", 2);
    ste::AddNested(m, "b", 9);
    ASSERT_EQ(m["a"].size(), 2U);
    EXPECT_EQ(m["a"][0], 1);
    EXPECT_EQ(m["a"][1], 2);
    EXPECT_EQ(m["b"][0], 9);
}

TEST(Collections, AddNested_SetMap) {
    std::map<int, std::set<int>> m;
    ste::AddNested(m, 10, 1);
    ste::AddNested(m, 10, 2);
    ste::AddNested(m, 10, 1);  // duplicate — set keeps only one
    EXPECT_EQ(m[10].size(), 2U);
}

TEST(Collections, Chunk) {
    std::vector<int> v{1,2,3,4,5,6,7};
    auto c = ste::Chunk(v, 3);
    ASSERT_EQ(c.size(), 3U);
    EXPECT_EQ(c[0], (std::vector<int>{1,2,3}));
    EXPECT_EQ(c[1], (std::vector<int>{4,5,6}));
    EXPECT_EQ(c[2], (std::vector<int>{7}));
}

TEST(Collections, Partition) {
    std::vector<int> v{1,2,3,4,5};
    auto [even, odd] = ste::Partition(v, [](int x){ return x % 2 == 0; });
    EXPECT_EQ(even, (std::vector<int>{2,4}));
    EXPECT_EQ(odd,  (std::vector<int>{1,3,5}));
}

TEST(Collections, DistinctBy) {
    struct P { int id; std::string name; };
    std::vector<P> v{{1,"a"},{2,"b"},{1,"c"}};
    auto out = ste::DistinctBy(v, [](const P& p){ return p.id; });
    ASSERT_EQ(out.size(), 2U);
    EXPECT_EQ(out[0].name, "a");
    EXPECT_EQ(out[1].name, "b");
}

TEST(Collections, MinMaxBy) {
    std::vector<std::string> v{"apple","kiwi","strawberry"};
    auto longest  = ste::MaxBy(v, [](const auto& s){ return s.size(); });
    auto shortest = ste::MinBy(v, [](const auto& s){ return s.size(); });
    ASSERT_TRUE(longest.has_value());
    EXPECT_EQ(*longest,  "strawberry");
    ASSERT_TRUE(shortest.has_value());
    EXPECT_EQ(*shortest, "kiwi");
}

TEST(Collections, GroupBy) {
    std::vector<int> v{1,2,3,4,5,6};
    auto g = ste::GroupBy(v, [](int x){ return x % 2; });
    EXPECT_EQ(g[0].size(), 3U);
    EXPECT_EQ(g[1].size(), 3U);
}

TEST(Collections, ToDictionary) {
    std::vector<std::string> v{"one","two","three"};
    auto d = ste::ToDictionary(v,
        [](const auto& s){ return s.size(); },
        [](const auto& s){ return s; });
    EXPECT_EQ(d[3], "one");
    EXPECT_EQ(d[5], "three");
}

TEST(Collections, AnyAllNone) {
    std::vector<int> v{1,2,3};
    EXPECT_TRUE (ste::Any(v, [](int x){ return x == 2; }));
    EXPECT_FALSE(ste::All(v, [](int x){ return x > 1; }));
    EXPECT_TRUE (ste::None(v,[](int x){ return x > 99; }));
}
