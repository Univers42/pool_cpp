# CPP Module 06 — C++ Casts

This module is about choosing the *right* cast on purpose. C-style casts
(`(int)x`) can silently perform any of several unrelated conversions; C++
splits them into named casts that each do one checkable thing. Each exercise
is built around exactly one of them:

| Exercise | Cast | Lesson |
|---|---|---|
| ex00 | `static_cast` | Explicit value conversions between arithmetic types |
| ex01 | `reinterpret_cast` | Pointer ↔ integer round-trip (the one safe use) |
| ex02 | `dynamic_cast` | Runtime type identification of polymorphic objects |

A secondary thread runs through ex00 and ex01: the C++98 idiom for a
**non-instantiable static utility class** — declare the constructors,
copy assignment, and destructor `private` and never define them (what
C++11 later spelled `= delete`).

Each exercise has its own README with full detail; summaries below.

## ex00 — Conversion of scalar types

`ScalarConverter::convert(const std::string&)` takes a C++ literal as a
string (`'c'`, `-42`, `4.2f`, `nan`, `+inff`, ...), detects its type,
and explicitly converts the value to `char`, `int`, `float`, and `double`
with one `static_cast` per target — after range checks, because casting an
out-of-range `double` to `int` is undefined behavior. Impossible conversions
(NaN/infinity to `char`/`int`, overflow) print `impossible`; non-printable
chars print `Non displayable`. Parsing funnels every numeric literal through
`std::strtod`, so overflowing input like `9999999999` is reported instead of
invoking UB. The test compares `convert()`'s captured stdout byte-for-byte
against expected output for the subject's examples, pseudo-literals,
overflow, and garbage input.

## ex01 — Serialization

`Serializer` has two static methods: `serialize(Data*)` returns the pointer
reinterpreted as a `uintptr_t`, and `deserialize(uintptr_t)` turns it back.
This is the canonical safe use of `reinterpret_cast`: the standard guarantees
the pointer→integer→pointer round-trip yields the original pointer.
(`static_cast` refuses pointer↔integer conversions entirely.) The demo and
tests create a non-empty `Data` struct, round-trip its address, and verify
the returned pointer compares equal to the original and its members are
intact.

## ex02 — Identify real type

Given only a `Base*` or `Base&` to an object that is really an `A`, `B`, or
`C` (`Base` has a virtual destructor, so `dynamic_cast` can interrogate its
dynamic type), `identify` recovers the real type — without `<typeinfo>`.
The point is the asymmetry between the two forms of `dynamic_cast`:

- `identify(Base*)` — pointer form returns `NULL` on mismatch; failure is a
  testable value.
- `identify(Base&)` — reference form throws on mismatch; failure is an
  exception, caught with `catch (...)` since naming `std::bad_cast` would
  need the forbidden header. No pointers used inside.

The demo generates five random objects and identifies each both ways; the
test asserts both overloads print exactly `A`/`B`/`C` for known objects and
agree on 20 random ones.

## Building and running

Everything compiles with `c++ -std=c++98 -Wall -Wextra -Werror` and is
valgrind-clean. In any exercise directory:

```sh
cd exNN
make            # build build/bin/exNN
make run        # build and run the demo (ex00 takes its literal as argv[1],
                # so run it directly: ./build/bin/ex00 42.0f)
make test       # build and run tests/test.cpp; exits non-zero on failure
make fclean     # remove build artifacts
```
