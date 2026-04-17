# Stellar — Toolkit Extensions for C++23

**Stellar** is a header-only C++23 library that mirrors the surface of
[`BinaryCoffee.Extensions`](https://github.com/AsereHighDev/BinaryCoffee.Extensions)
(C#) method-for-method under the `ste::` namespace, then adds a curated set of
professional utilities on top.

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
  performance, or fluent `ste::str` (a thin `std::string` subclass) for the
  familiar `.Method()` syntax from the C# original.
- **Header-only**, C++23, no third-party runtime deps.
- **Zero regex** in hot paths — state machines for `NormalizeSpaces`, `ToSlug`,
  `StripTags`, `ReplaceAll`, etc.
- **Async tier** — both `std::future<T>` (Task-like) and a C++20 coroutine
  `ste::Task<T>` suitable for `co_await`.
- **GoogleTest** suite for every public function.
- **Google Benchmark** comparing naive C++ (regex / multi-pass) vs. the
  optimized `ste::` version.

## API parity with BinaryCoffee.Extensions

| C# method                     | `ste::` equivalent                  |
|-------------------------------|-------------------------------------|
| `string.ToCamelCase()`        | `ste::ToCamelCase(sv)`              |
| `string.ToPascalCase()`       | `ste::ToPascalCase(sv)`             |
| `string.ToSnakeLowerCase()`   | `ste::ToSnakeLowerCase(sv)`         |
| `string.ToSnakeUpperCase()`   | `ste::ToSnakeUpperCase(sv)`         |
| `string.ToTitleCase(culture)` | `ste::ToTitleCase(sv)`              |
| `string.MatchesPattern(p)`    | `ste::MatchesPattern(sv, p)`        |
| `string.RemoveCharacters(..)` | `ste::RemoveCharacters(sv, ...)`    |
| `string.Reverse()`            | `ste::Reverse(sv)`                  |
| `string.ToSlug()`             | `ste::ToSlug(sv)`                   |
| `string.CapitalizeWords()`    | `ste::CapitalizeWords(sv)`          |
| `string.NormalizeSpaces()`    | `ste::NormalizeSpaces(sv)`          |
| `string.StripTags()`          | `ste::StripTags(sv)`                |
| `IEnumerable<T>.Join(...)`    | `ste::Join(range, ...)`             |
| `Dictionary.AddNested(k, v)`  | `ste::AddNested(map, k, v)`         |
| `StringBuilder.AppendFormatIf`| `ste::StringBuilder::AppendFormatIf`|
| `StringBuilder.AppendLineIf`  | `ste::StringBuilder::AppendLineIf`  |
| `StringBuilder.InsertJoin`    | `ste::StringBuilder::InsertJoin`    |
| `ToCamelCaseAsync()`          | `ste::ToCamelCaseAsync(s)`          |
| `ToPascalCaseAsync()`         | `ste::ToPascalCaseAsync(s)`         |
| `ToTitleCaseAsync()`          | `ste::ToTitleCaseAsync(s)`          |
| `IAsyncEnumerable.JoinAsync`  | `ste::JoinAsync(range, fn, sep)`    |

## Extras beyond the C# API

- **Case**: `ToKebabCase`
- **Predicates**: `IsNullOrEmpty`, `IsNullOrWhiteSpace`, `IsNumeric`, `IsAlpha`,
  `IsAlphanumeric`
- **Case-insensitive ASCII compare**: `EqualsIgnoreCase`, `StartsWithIgnoreCase`,
  `EndsWithIgnoreCase`, `ContainsIgnoreCase`
- **Truncate / PadLeft / PadRight / Repeat / Left / Right**
- **Counting**: `WordCount`, `LineCount`
- **Split / SplitView (lazy)**, **ReplaceAll**, **LevenshteinDistance**
- **Base64** encode/decode (RFC 4648)
- **Collections**: `Chunk`, `Partition`, `DistinctBy`, `GroupBy`, `MinBy`,
  `MaxBy`, `ToDictionary`, `Any`, `All`, `None`
- **Async**: `ste::Task<T>` coroutine return type (awaitable)

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
