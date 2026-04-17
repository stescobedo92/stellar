# Stellar — Toolkit Extensions for C++23

**Stellar** is a header-only C++23 library that provides a curated set of
professional string, collection, async, and text-building utilities under the
`ste::` namespace, designed for modern, high-performance applications.

Every hot path is single-pass and pre-reserves its output buffer, so the common
operations allocate **exactly once** (for the returned `std::string`) and often
outperform the equivalent `std::regex` / multi-pass solution by an order of
magnitude — see the benchmark results below.

```cpp
#include <ste/ste.hpp>

ste::str s{"  my AWESOME variable name  "};
s.NormalizeSpaces().ToPascalCase();   // → "MyAwesomeVariableName"
ste::ToSlug("Hello, World! 100%");    // → "hello-world-100"
```

## Highlights

- **Namespace `ste::`** — every public symbol lives here.
- **Two calling styles** — free functions over `std::string_view` for peak
  performance, or fluent `ste::str` (a thin `std::string` subclass) for
  `.Method()` chaining.
- **Header-only**, C++23, no third-party runtime deps.
- **Zero regex** in hot paths — state machines for `NormalizeSpaces`, `ToSlug`,
  `StripTags`, `ReplaceAll`, etc.
- **Async tier** — both `std::future<T>` (Task-like) and a C++20 coroutine
  `ste::Task<T>` suitable for `co_await`.
- **GoogleTest** suite for every public function.
- **Google Benchmark** comparing naive C++ (regex / multi-pass) vs. the
  optimized `ste::` version.

## API

| Functions                                   |
|---------------------------------------------|
| `ste::ToCamelCase(sv)`                      |
| `ste::ToPascalCase(sv)`                     |
| `ste::ToSnakeLowerCase(sv)`                 |
| `ste::ToSnakeUpperCase(sv)`                 |
| `ste::ToKebabCase(sv)`                      |
| `ste::ToTitleCase(sv)`                      |
| `ste::MatchesPattern(sv, pattern)`          |
| `ste::RemoveCharacters(sv, chars...)`       |
| `ste::Reverse(sv)`                          |
| `ste::ToSlug(sv)`                           |
| `ste::CapitalizeWords(sv)`                  |
| `ste::NormalizeSpaces(sv)`                  |
| `ste::StripTags(sv)`                        |
| `ste::IsNullOrEmpty(sv)`                    |
| `ste::IsNullOrWhiteSpace(sv)`               |
| `ste::IsNumeric(sv)`                        |
| `ste::IsAlpha(sv)`                          |
| `ste::IsAlphanumeric(sv)`                   |
| `ste::EqualsIgnoreCase(a, b)`               |
| `ste::StartsWithIgnoreCase(s, prefix)`      |
| `ste::EndsWithIgnoreCase(s, suffix)`        |
| `ste::ContainsIgnoreCase(haystack, needle)` |
| `ste::Truncate(sv, max_len, ellipsis)`      |
| `ste::PadLeft(sv, width, pad)`              |
| `ste::PadRight(sv, width, pad)`             |
| `ste::Repeat(sv, n)`                        |
| `ste::Left(sv, n)`                          |
| `ste::Right(sv, n)`                         |
| `ste::WordCount(sv)`                        |
| `ste::LineCount(sv)`                        |
| `ste::Split(sv, separator, keep_empty)`     |
| `ste::SplitView(sv, separator)`             |
| `ste::ReplaceAll(sv, from, to)`             |
| `ste::LevenshteinDistance(a, b)`            |
| `ste::ToBase64(sv)`                         |
| `ste::FromBase64(sv)`                       |
| `ste::Join(range, separator)`               |
| `ste::Join(range, formatter, separator)`    |
| `ste::AddNested(map, key, value)`           |
| `ste::Chunk(range, size)`                   |
| `ste::Partition(range, predicate)`          |
| `ste::DistinctBy(range, keyFn)`             |
| `ste::GroupBy(range, keyFn)`                |
| `ste::MinBy(range, projection)`             |
| `ste::MaxBy(range, projection)`             |
| `ste::ToDictionary(range, keyFn, valueFn)`  |
| `ste::ForEach(range, fn)`                   |
| `ste::Any(range, predicate)`                |
| `ste::All(range, predicate)`                |
| `ste::None(range, predicate)`               |
| `ste::StringBuilder::Append(s)`             |
| `ste::StringBuilder::AppendLine(s)`         |
| `ste::StringBuilder::AppendFormat(fmt, …)`  |
| `ste::StringBuilder::AppendRepeated(v, n)`  |
| `ste::StringBuilder::AppendFormatIf(cond, ifFn, elseFn)` |
| `ste::StringBuilder::AppendLineIf(cond, getter)`         |
| `ste::StringBuilder::AppendIf(cond, s)`     |
| `ste::StringBuilder::Insert(index, s)`      |
| `ste::StringBuilder::InsertJoin(index, range, formatter, sep)` |
| `ste::StringBuilder::Replace(from, to)`     |
| `ste::StringBuilder::Remove(start, count)`  |
| `ste::StringBuilder::Clear()`               |
| `ste::StringBuilder::MoveToString()`        |
| `ste::ToCamelCaseAsync(s)`                  |
| `ste::ToPascalCaseAsync(s)`                 |
| `ste::ToTitleCaseAsync(s)`                  |
| `ste::ToSlugAsync(s)`                       |
| `ste::JoinAsync(range, formatter, sep)`     |
| `ste::ToCamelCaseTask(s)`                   |
| `ste::ToPascalCaseTask(s)`                  |
| `ste::ToTitleCaseTask(s)`                   |
| `ste::Task<T>` (coroutine return type, awaitable) |

## Install

### vcpkg — self-hosted registry (always up-to-date)

This repo **is** a vcpkg registry. Add it to your `vcpkg-configuration.json`:

```jsonc
{
  "default-registry": {
    "kind": "git",
    "baseline": "<microsoft/vcpkg commit sha>",
    "repository": "https://github.com/microsoft/vcpkg"
  },
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/stescobedo92/stellar",
      "baseline": "<commit sha of main>",
      "reference": "main",
      "packages": [ "stellar" ]
    }
  ]
}
```

Then `vcpkg install stellar` works out of the box.

### vcpkg — microsoft/vcpkg (once upstream PR is merged)

```bash
vcpkg install stellar
```

### Conan 2.x

```bash
conan install --requires=stellar/0.1.0 --build=missing
```

The recipe is published to Conan Center (or your own Artifactory remote —
see the `publish-conan` workflow).

### CMake FetchContent (no package manager)

```cmake
include(FetchContent)
FetchContent_Declare(stellar
    GIT_REPOSITORY https://github.com/stescobedo92/stellar.git
    GIT_TAG v0.1.0
)
FetchContent_MakeAvailable(stellar)
target_link_libraries(my_app PRIVATE stellar::stellar)
```

## Building

```bash
# From the stellar/ directory:
cmake --preset default
cmake --build --preset default -j

# Run tests
ctest --preset default

# Run benchmarks
./build/default/benchmarks/stellar_bench
```

Requires a C++23 compiler (GCC ≥ 13, Clang ≥ 17, MSVC ≥ 19.38 / VS 2022 17.8).

## Performance snapshot

Naive ≈ what most code bases do: `std::regex_replace`, multi-pass,
no buffer reservation.

| Operation (≈200 B corpus)   | Naive C++   | ste::        | Speed-up |
|-----------------------------|-------------|--------------|----------|
| `NormalizeSpaces`           | ~11 µs      | ~0.25 µs     | ~40×     |
| `StripTags`                 | ~9 µs       | ~0.20 µs     | ~45×     |
| `ToSlug`                    | ~18 µs      | ~0.35 µs     | ~50×     |
| `ToPascalCase`              | ~0.6 µs     | ~0.15 µs     | ~4×      |
| `ReplaceAll("ipsum")`       | ~7 µs       | ~0.30 µs     | ~23×     |

(Numbers are illustrative; run `stellar_bench` on your machine for exact figures.)

## License

MIT — see [LICENSE](LICENSE).
