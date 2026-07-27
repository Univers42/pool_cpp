# ex00 — Conversion of scalar types

## The concept

C++ distinguishes **implicit conversions** (the compiler silently promotes
`int` to `double`, etc.) from **explicit conversions** you must spell out.
This exercise is about making every scalar conversion explicit and visible:
take a string holding a C++ literal (`'c'`, `0`, `-42`, `4.2f`, `4.2`,
`nan`, `+inff`, ...), detect which type it denotes, materialize its value,
then explicitly convert that value to each of the four scalar types — `char`,
`int`, `float`, `double` — reporting when a conversion is impossible
(NaN/infinity to `char`/`int`, out-of-range values) or not displayable
(control characters).

The module-wide rule requires a *specific* cast per conversion. Here every
conversion is a value conversion between arithmetic types, which is exactly
what **`static_cast`** is for: it is checked at compile time, cannot cast
away constness or reinterpret bits, and documents intent at the call site.
`dynamic_cast`/`reinterpret_cast`/`const_cast` would all be wrong tools —
there is no polymorphism, no bit reinterpretation, no constness involved.

The second lesson is the **non-instantiable utility class**: `ScalarConverter`
holds no state, so the subject forbids instantiating it. In C++98 (no
`= delete`) the idiom is to declare the constructor, copy constructor, copy
assignment operator, and destructor `private` and never define them: user
code fails to compile at the access check, and internal misuse fails at link
time.

## How the code demonstrates it

- `ScalarConverter.hpp` — the class is exactly what the subject asks: one
  public static `convert(const std::string&)`, plus the four private,
  undefined special members that make it non-instantiable.
- `ScalarConverter.cpp` — all helpers live in an anonymous namespace (they
  are implementation detail, not class API):
  - `isCharLiteral` / `isPseudoLiteral` / `isIntLiteral` / `isFloatLiteral` /
    `isDoubleLiteral` classify the input (decimal notation only, per subject).
  - Parsing funnels every numeric literal through one `std::strtod` call into
    a `double` (`double` represents every `char`, `int`, and `float` value
    exactly enough for this exercise). Pseudo-literals are built from
    `std::numeric_limits<double>` (`quiet_NaN()`, `infinity()`). Using
    `strtod` instead of `atoi` means an out-of-range integer literal like
    `9999999999` becomes a large finite double and is reported as
    `int: impossible` instead of triggering undefined behavior.
  - `printChar` / `printInt` / `printFloat` / `printDouble` each perform one
    explicit `static_cast` to their target type, after range checks
    (`std::isnan`, comparisons against `std::numeric_limits<int>`) so no cast
    is ever executed on a value the target type cannot hold — casting an
    out-of-range `double` to `int` is undefined behavior, so the check must
    come first.
  - The `.0` suffix on whole-valued floats/doubles (`42` → `42.0f` / `42.0`)
    is decided by `v == std::floor(v)` with a magnitude guard, never by an
    `int` cast that could overflow.
- `main.cpp` — the subject's driver: one command-line argument, converted and
  printed in the required four-line format.

Invalid or empty input prints `impossible` for all four types, matching the
per-type message format the subject prescribes.

## Build / run / test

```sh
make            # builds build/bin/ex00
./build/bin/ex00 42.0f
./build/bin/ex00 nan
./build/bin/ex00 "'a'"     # quote so the shell keeps the single quotes

make test       # builds and runs tests/test.cpp (build/bin/test)
make fclean     # removes build/
```

`tests/test.cpp` captures `convert()`'s stdout via a `streambuf` swap and
compares it byte-for-byte against expected output: the subject's three
verbatim examples, both char spellings, negatives, all pseudo-literals,
int overflow, and garbage input. It exits non-zero on any mismatch.

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`; valgrind-clean (no
allocations besides `std::string`'s own).
