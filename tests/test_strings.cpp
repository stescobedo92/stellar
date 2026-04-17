// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include <ste/strings.hpp>

#include <string>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

// ---------- Case conversion -----------------------------------------------------

TEST(Strings, ToCamelCase_Empty)    { EXPECT_EQ(ste::ToCamelCase(""), ""); }
TEST(Strings, ToCamelCase_Simple)   { EXPECT_EQ(ste::ToCamelCase("hello world"),  "HelloWorld"); }
TEST(Strings, ToCamelCase_MultiWs)  { EXPECT_EQ(ste::ToCamelCase("  foo   bar  "), "FooBar"); }
TEST(Strings, ToCamelCase_Single)   { EXPECT_EQ(ste::ToCamelCase("foo"),          "Foo"); }
TEST(Strings, ToCamelCase_AllCaps)  { EXPECT_EQ(ste::ToCamelCase("HELLO WORLD"),  "HelloWorld"); }

TEST(Strings, ToPascalCase_Empty)   { EXPECT_EQ(ste::ToPascalCase(""),                ""); }
TEST(Strings, ToPascalCase_Basic)   { EXPECT_EQ(ste::ToPascalCase("my awesome name"), "MyAwesomeName"); }

TEST(Strings, ToSnakeLower_Empty)   { EXPECT_EQ(ste::ToSnakeLowerCase(""),        ""); }
TEST(Strings, ToSnakeLower_Basic)   { EXPECT_EQ(ste::ToSnakeLowerCase("HelloWorld"),   "hello_world"); }
TEST(Strings, ToSnakeLower_Acronym) { EXPECT_EQ(ste::ToSnakeLowerCase("MyXMLReader"),  "my_xmlreader"); }

TEST(Strings, ToSnakeUpper_Basic)   { EXPECT_EQ(ste::ToSnakeUpperCase("HelloWorld"),   "HELLO_WORLD"); }

TEST(Strings, ToKebabCase_Basic)    { EXPECT_EQ(ste::ToKebabCase("HelloWorld"),        "hello-world"); }
TEST(Strings, ToKebabCase_Spaces)   { EXPECT_EQ(ste::ToKebabCase("my title here"),     "my-title-here"); }

TEST(Strings, ToTitleCase_Basic)    { EXPECT_EQ(ste::ToTitleCase("hello world again"), "Hello World Again"); }
TEST(Strings, ToTitleCase_MixedIn)  { EXPECT_EQ(ste::ToTitleCase("HELLO WORLD"),       "Hello World"); }

// ---------- Predicates & transforms --------------------------------------------

TEST(Strings, IsNullOrEmpty)      { EXPECT_TRUE(ste::IsNullOrEmpty("")); EXPECT_FALSE(ste::IsNullOrEmpty("x")); }
TEST(Strings, IsNullOrWhiteSpace) { EXPECT_TRUE(ste::IsNullOrWhiteSpace("   \t\n")); EXPECT_FALSE(ste::IsNullOrWhiteSpace(" x ")); }

TEST(Strings, MatchesPattern_Ok)    { EXPECT_TRUE (ste::MatchesPattern("hello123", R"(\d+)")); }
TEST(Strings, MatchesPattern_Fail)  { EXPECT_FALSE(ste::MatchesPattern("hello",    R"(\d+)")); }
TEST(Strings, MatchesPattern_EmptyInput)   { EXPECT_THROW(ste::MatchesPattern("", ".*"), std::invalid_argument); }
TEST(Strings, MatchesPattern_EmptyPattern) { EXPECT_THROW(ste::MatchesPattern("x", ""), std::invalid_argument); }

TEST(Strings, RemoveCharacters_View) { EXPECT_EQ(ste::RemoveCharacters("hello world", "lo"), "he wrd"); }
TEST(Strings, RemoveCharacters_Pack) { EXPECT_EQ(ste::RemoveCharacters("hello world", 'l', 'o'), "he wrd"); }

TEST(Strings, Reverse)            { EXPECT_EQ(ste::Reverse("abcde"),  "edcba"); EXPECT_EQ(ste::Reverse(""), ""); }

TEST(Strings, ToSlug_Basic)       { EXPECT_EQ(ste::ToSlug("Hello World!"),     "hello-world"); }
TEST(Strings, ToSlug_Symbols)     { EXPECT_EQ(ste::ToSlug("  My $100 Post!!"), "my-100-post"); }
TEST(Strings, ToSlug_Empty)       { EXPECT_EQ(ste::ToSlug(""), ""); }

TEST(Strings, CapitalizeWords)    { EXPECT_EQ(ste::CapitalizeWords("hello   world"), "Hello   World"); }

TEST(Strings, NormalizeSpaces)    { EXPECT_EQ(ste::NormalizeSpaces("  hello   world  "), "hello world"); }
TEST(Strings, NormalizeSpaces_Empty) { EXPECT_EQ(ste::NormalizeSpaces("   "), ""); }

TEST(Strings, StripTags_Basic)    { EXPECT_EQ(ste::StripTags("<b>hi</b> <i>there</i>"), "hi there"); }
TEST(Strings, StripTags_Nested)   { EXPECT_EQ(ste::StripTags("<div><p>x</p></div>"),    "x"); }

// ---------- IgnoreCase ---------------------------------------------------------

TEST(Strings, EqualsIgnoreCase)     { EXPECT_TRUE (ste::EqualsIgnoreCase("Hello", "HELLO")); EXPECT_FALSE(ste::EqualsIgnoreCase("ab", "abc")); }
TEST(Strings, StartsWithIgnoreCase) { EXPECT_TRUE (ste::StartsWithIgnoreCase("HelloWorld", "hello")); }
TEST(Strings, EndsWithIgnoreCase)   { EXPECT_TRUE (ste::EndsWithIgnoreCase("HelloWorld",   "world")); }
TEST(Strings, ContainsIgnoreCase)   { EXPECT_TRUE (ste::ContainsIgnoreCase("HelloWorld",   "LLOWO")); EXPECT_FALSE(ste::ContainsIgnoreCase("abc", "xyz")); }

// ---------- Truncate / Pad / Repeat -------------------------------------------

TEST(Strings, Truncate_NoChange) { EXPECT_EQ(ste::Truncate("abc", 10),       "abc"); }
TEST(Strings, Truncate_Clipped)  { EXPECT_EQ(ste::Truncate("abcdefgh", 5),   "ab..."); }
TEST(Strings, PadLeft)           { EXPECT_EQ(ste::PadLeft("42", 5, '0'),     "00042"); }
TEST(Strings, PadRight)          { EXPECT_EQ(ste::PadRight("abc", 6, '.'),   "abc..."); }
TEST(Strings, Repeat)            { EXPECT_EQ(ste::Repeat("ab", 3),           "ababab"); }
TEST(Strings, Left_Right)        { EXPECT_EQ(ste::Left("abcdef", 3),  "abc"); EXPECT_EQ(ste::Right("abcdef", 3), "def"); }

// ---------- Classification / counting -----------------------------------------

TEST(Strings, IsNumeric)         { EXPECT_TRUE(ste::IsNumeric("0123"));    EXPECT_FALSE(ste::IsNumeric("12a")); EXPECT_FALSE(ste::IsNumeric("")); }
TEST(Strings, IsAlpha)           { EXPECT_TRUE(ste::IsAlpha("abcXYZ"));    EXPECT_FALSE(ste::IsAlpha("ab1")); }
TEST(Strings, IsAlphanumeric)    { EXPECT_TRUE(ste::IsAlphanumeric("a1")); EXPECT_FALSE(ste::IsAlphanumeric("a!")); }
TEST(Strings, WordCount)         { EXPECT_EQ(ste::WordCount("  one  two three "), 3U); EXPECT_EQ(ste::WordCount(""), 0U); }
TEST(Strings, LineCount)         { EXPECT_EQ(ste::LineCount("a\nb\nc"), 3U); EXPECT_EQ(ste::LineCount("a"), 1U); EXPECT_EQ(ste::LineCount(""), 0U); }

// ---------- Split / Replace / Levenshtein -------------------------------------

TEST(Strings, Split_Basic) {
    auto parts = ste::Split("a,b,,c", ',', true);
    ASSERT_EQ(parts.size(), 4U);
    EXPECT_EQ(parts[0], "a"); EXPECT_EQ(parts[1], "b"); EXPECT_EQ(parts[2], ""); EXPECT_EQ(parts[3], "c");
}
TEST(Strings, Split_SkipEmpty) {
    auto parts = ste::Split("a,,b", ',', false);
    ASSERT_EQ(parts.size(), 2U);
    EXPECT_EQ(parts[0], "a"); EXPECT_EQ(parts[1], "b");
}

TEST(Strings, ReplaceAll_Basic)  { EXPECT_EQ(ste::ReplaceAll("abcabc", "b", "BB"), "aBBcaBBc"); }
TEST(Strings, ReplaceAll_Empty)  { EXPECT_EQ(ste::ReplaceAll("abc", "", "x"),      "abc"); }

TEST(Strings, Levenshtein)       { EXPECT_EQ(ste::LevenshteinDistance("kitten", "sitting"), 3U); EXPECT_EQ(ste::LevenshteinDistance("abc", "abc"), 0U); EXPECT_EQ(ste::LevenshteinDistance("", "abc"), 3U); }

// ---------- Base64 ------------------------------------------------------------

TEST(Strings, Base64_RoundTrip) {
    for (auto s : {""sv, "f"sv, "fo"sv, "foo"sv, "foobar"sv, "Hello, World!"sv}) {
        auto enc = ste::ToBase64(s);
        auto dec = ste::FromBase64(enc);
        ASSERT_TRUE(dec.has_value()) << "decode failed for " << s;
        EXPECT_EQ(*dec, std::string(s));
    }
}
TEST(Strings, Base64_KnownVectors) {
    EXPECT_EQ(ste::ToBase64("Man"),   "TWFu");
    EXPECT_EQ(ste::ToBase64("Ma"),    "TWE=");
    EXPECT_EQ(ste::ToBase64("M"),     "TQ==");
}
TEST(Strings, Base64_BadInput) {
    EXPECT_FALSE(ste::FromBase64("abc").has_value());    // length not multiple of 4
    EXPECT_FALSE(ste::FromBase64("!!!!").has_value());   // invalid chars
}

// ---------- Join --------------------------------------------------------------

TEST(Strings, Join_Strings)     { std::vector<std::string> v{"a","b","c"}; EXPECT_EQ(ste::Join(v, ","),       "a,b,c"); }
TEST(Strings, Join_Formatter)   { std::vector<int> v{1,2,3};               EXPECT_EQ(ste::Join(v, [](int x){ return std::to_string(x * 10); }, "-"), "10-20-30"); }
TEST(Strings, Join_Ints)        { std::vector<int> v{1,2,3};               EXPECT_EQ(ste::Join(v, ","), "1,2,3"); }

// ---------- Fluent wrapper ----------------------------------------------------

TEST(FluentStr, Chaining) {
    ste::str s("  hello   world  ");
    auto out = s.NormalizeSpaces().ToPascalCase();
    EXPECT_EQ(out, "HelloWorld");
}

TEST(FluentStr, Slug) {
    EXPECT_EQ(ste::str("My 100% Post!!!").ToSlug(), "my-100-post");
}

TEST(FluentStr, Base64) {
    auto enc = ste::str("Hello").ToBase64();
    ASSERT_EQ(enc, "SGVsbG8=");
    EXPECT_EQ(*enc.FromBase64(), "Hello");
}
