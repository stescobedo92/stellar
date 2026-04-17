// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include <ste/string_builder.hpp>

#include <vector>

TEST(StringBuilder, AppendBasic) {
    ste::StringBuilder sb;
    sb.Append("hello").Append(' ').Append("world");
    EXPECT_EQ(sb.ToString(), "hello world");
    EXPECT_EQ(sb.Length(), 11U);
}

TEST(StringBuilder, AppendFormat) {
    ste::StringBuilder sb;
    sb.AppendFormat("x={}, y={:.2f}", 42, 3.14159);
    EXPECT_EQ(sb.ToString(), "x=42, y=3.14");
}

TEST(StringBuilder, AppendRepeated_String) {
    ste::StringBuilder sb;
    sb.AppendRepeated("ab", 3);
    EXPECT_EQ(sb.ToString(), "ababab");
}

TEST(StringBuilder, AppendRepeated_Char) {
    ste::StringBuilder sb;
    sb.AppendRepeated('-', 5);
    EXPECT_EQ(sb.ToString(), "-----");
}

TEST(StringBuilder, AppendLineIf_True) {
    ste::StringBuilder sb;
    sb.AppendLineIf(true, []{ return std::string("yep"); });
    EXPECT_EQ(sb.ToString(), "yep\n");
}

TEST(StringBuilder, AppendLineIf_False) {
    ste::StringBuilder sb;
    sb.AppendLineIf(false, []{ return std::string("nope"); });
    EXPECT_EQ(sb.ToString(), "");
}

TEST(StringBuilder, AppendFormatIf) {
    ste::StringBuilder sb;
    sb.Append("[");
    sb.AppendFormatIf(true,
        [](auto& b){ b.AppendFormat("t={}", 1); },
        [](auto& b){ b.AppendFormat("f={}", 2); });
    sb.Append("]");
    EXPECT_EQ(sb.ToString(), "[t=1]");
}

TEST(StringBuilder, InsertJoin) {
    ste::StringBuilder sb;
    sb.Append("<<>>");
    std::vector<int> v{1,2,3};
    sb.InsertJoin(2, v, [](int x){ return std::to_string(x); }, ",");
    EXPECT_EQ(sb.ToString(), "<<1,2,3>>");
}

TEST(StringBuilder, Replace) {
    ste::StringBuilder sb{"hello world hello"};
    sb.Replace("hello", "HI");
    EXPECT_EQ(sb.ToString(), "HI world HI");
}

TEST(StringBuilder, Remove) {
    ste::StringBuilder sb{"0123456789"};
    sb.Remove(3, 4);
    EXPECT_EQ(sb.ToString(), "012789");
}

TEST(StringBuilder, MoveToString) {
    ste::StringBuilder sb{"payload"};
    auto s = sb.MoveToString();
    EXPECT_EQ(s, "payload");
    EXPECT_EQ(sb.Length(), 0U);  // moved out
}
