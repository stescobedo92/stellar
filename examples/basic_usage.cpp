// SPDX-License-Identifier: MIT
// Stellar Toolkit Extensions — quickstart example.
#include <ste/ste.hpp>

#include <iostream>
#include <vector>

int main() {
    // ---- Fluent style (matches C# naming one-for-one) -----------------------
    ste::str s{"  my AWESOME variable name  "};
    std::cout << "PascalCase: "   << s.NormalizeSpaces().ToPascalCase()    << '\n';
    std::cout << "snake_case: "   << s.NormalizeSpaces().ToSnakeLowerCase() << '\n';
    std::cout << "kebab-case: "   << s.NormalizeSpaces().ToKebabCase()     << '\n';
    std::cout << "slug:       "   << ste::str("Hello, World! 100%").ToSlug() << '\n';

    // ---- Free-function style (accepts std::string_view, zero-copy args) -----
    std::string_view html = "<div><b>Hello</b>, <i>world</i>!</div>";
    std::cout << "stripped:   " << ste::StripTags(html) << '\n';

    // ---- Collections --------------------------------------------------------
    std::vector<int> nums{1, 2, 3, 4, 5};
    std::cout << "joined:     " << ste::Join(nums, ",") << '\n';
    auto [even, odd] = ste::Partition(nums, [](int x){ return x % 2 == 0; });
    std::cout << "even count: " << even.size() << "  odd count: " << odd.size() << '\n';

    // ---- StringBuilder ------------------------------------------------------
    ste::StringBuilder sb;
    sb.Append("result=[").AppendFormat("{:08x}", 0xDEADBEEFu).Append("]");
    std::cout << sb.ToString() << '\n';

    // ---- Async --------------------------------------------------------------
    auto fut = ste::ToCamelCaseAsync("hello from async");
    std::cout << "async:      " << fut.get() << '\n';

    // ---- Base64 -------------------------------------------------------------
    auto enc = ste::ToBase64("Hello, Stellar!");
    std::cout << "base64:     " << enc << '\n';
    std::cout << "decoded:    " << *ste::FromBase64(enc) << '\n';

    return 0;
}
