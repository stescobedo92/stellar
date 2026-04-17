// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include <ste/async.hpp>

#include <future>
#include <string>
#include <vector>

TEST(Async, ToCamelCaseAsync) {
    auto f = ste::ToCamelCaseAsync("hello world");
    EXPECT_EQ(f.get(), "HelloWorld");
}

TEST(Async, ToPascalCaseAsync) {
    auto f = ste::ToPascalCaseAsync("hello there");
    EXPECT_EQ(f.get(), "HelloThere");
}

TEST(Async, ToTitleCaseAsync) {
    auto f = ste::ToTitleCaseAsync("lorem ipsum");
    EXPECT_EQ(f.get(), "Lorem Ipsum");
}

TEST(Async, JoinAsync_RangeOfStrings) {
    std::vector<std::string> v{"alpha","beta","gamma"};
    auto f = ste::JoinAsync(v, [](const std::string& s){ return s; }, ", ");
    EXPECT_EQ(f.get(), "alpha, beta, gamma");
}

TEST(Async, JoinAsync_RangeOfFutures) {
    std::vector<std::future<std::string>> v;
    v.emplace_back(std::async(std::launch::async, []{ return std::string("a"); }));
    v.emplace_back(std::async(std::launch::async, []{ return std::string("b"); }));
    v.emplace_back(std::async(std::launch::async, []{ return std::string("c"); }));
    auto f = ste::JoinAsync(std::move(v), [](const std::string& s){ return s; }, "-");
    EXPECT_EQ(f.get(), "a-b-c");
}

TEST(Async, CoroutineTask) {
    auto t = ste::ToCamelCaseTask("hello world");
    EXPECT_EQ(t.Get(), "HelloWorld");
}
