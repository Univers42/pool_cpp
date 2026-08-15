# ex00 — Start with a few functions

## Concept: function templates

A function template is a blueprint the compiler instantiates once per type it
is called with. Writing `swap`, `min`, `max` once as templates gives us a
working version for `int`, `double`, `std::string`, or any user-defined type —
the only contract is that the type supports copy construction/assignment (for
`swap`) and the comparison operators (for `min`/`max`). This is compile-time
polymorphism: no inheritance, no virtual dispatch, no runtime cost.

## How this code demonstrates it

`whatever.hpp` defines the three templates. Templates must live in headers:
the compiler needs the full definition at every instantiation point.

Key design decisions:

- `swap(T&, T&)` takes non-const references and swaps via a temporary copy —
  the simplest thing that works in C++98. It costs exactly one copy
  construction and two copy assignments, and `main.cpp` counts them rather
  than asserting it.
- `min`/`max` take and return `const T&` — no copy of potentially heavy
  objects like `std::string`, and the return can bind to the caller's
  originals.
- On equal arguments both return the **second** parameter, as the subject
  requires. `(a < b) ? a : b` and `(a > b) ? a : b` yield `b` when `a == b`,
  so no extra equality branch is needed. This is **unverifiable by value** —
  when both are 5, comparing the result to 5 proves nothing — so the tests
  compare addresses: `&::min(x, y) == &y`. Returning a reference is what makes
  the rule observable at all.
- Every call site says `::swap` / `::min` / `::max`.

## Why the `::` is not decoration

This is the part I had wrong for a long time, and `tests/compile_fail.sh` is
what corrected it. The usual claim is "without `::` the call is ambiguous with
`std::swap`". It is not — and the truth is worse:

- **`swap(str1, str2)` compiles and silently calls `std::swap`.** `<string>`
  declares a `swap` overload just for `basic_string`; it is *more specialised*
  than our generic `swap(T&, T&)`, so partial ordering **prefers** it. It
  swaps buffers in O(1) instead of making our three copies. The call works,
  and it is never our template. `tests/test.cpp` proves which one ran by
  watching the string's buffer address move.
- **An exact non-template match wins outright.** A type in a namespace that
  ships its own `swap` gets that one via argument-dependent lookup.
- **A genuine ambiguity needs an equally generic candidate.** Only when the
  other `swap` is exactly as generic as ours does the call become a hard
  error. That case is in `tests/compile_fail.sh`.

So an unqualified call gives you the wrong function or no function. Silent
substitution is the dangerous half, because nothing tells you.

## Build / run / test

```sh
make            # builds build/bin/ex00
make run        # build + run the guided tour
make test       # builds and runs tests/test.cpp (exits non-zero on failure)
make fclean     # removes all build artifacts
```

`main.cpp` is a nine-section guided tour rather than a bare demo: the subject's
scenario, one template across five types, the `::` story above, reference
identity on ties, the zero-copy cost of `const T&` (counted), what `swap`
really costs in C++98, why `::min(1, 2.5)` will not deduce, why a template is
only compiled when instantiated, and the dangling-reference trap that returning
`const T&` buys you.

Three layers of checking, because a passing suite proves nothing on its own:

```sh
make verify               # all three layers, stops on the first failure
make test                 # 78 assertions, fixed cases + fuzz
./build/bin/test 1234     # same, different fuzz seed
./tests/mutants.sh        # 13 injected defects; all must be caught
./tests/compile_fail.sh   # 9 cases: 8 that must NOT compile, 1 control
```

The fuzz section checks *identity* invariants as well as values, and asserts
its own coverage (that the run actually produced ties, and both strict
orderings) — a fuzz run that never generated a tie would prove nothing about
the rule that matters most here.

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`; also builds warning-free
under `g++`. Valgrind-clean.
