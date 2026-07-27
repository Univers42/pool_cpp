# C++ Module 08 — Templated containers, iterators, algorithms

The STL, forbidden until now, becomes mandatory. The arc across the three
exercises: first *use* the STL through its two pillars — iterators and
`<algorithm>` (ex00) — then *build on top of it*, wrapping a standard
container behind your own invariants (ex01), and finally *extend it*,
opening up a container adapter that deliberately hides its container
(ex02). Everything is C++98 (`-Wall -Wextra -Werror -std=c++98`), and every
class template lives entirely in its header, as templates require.

## ex00 — Easy find

Iterator-based generic programming. `easyfind` is a function template that
accepts any container of `int` exposing `begin()`/`end()` and searches it
with `std::find` instead of a hand-rolled loop. It returns the iterator (the
most informative result) and converts `std::find`'s end-sentinel on a miss
into a thrown `std::out_of_range` — the same exception standard containers
use for failed lookups. Two overloads (`T&`/`const T&`) mirror the standard
library's `begin()`/`begin() const` duality.

The demo runs the same code over `std::vector`, `std::list`, and a const
`std::deque`; the tests assert found value, first-occurrence on duplicates,
throw on miss and on empty container, and the const overload.

## ex01 — Span

A bounded container of ints built on `std::vector`, answering two queries
with `<algorithm>` instead of manual loops: **longest span** is
`std::max_element` minus `std::min_element`; **shortest span** sorts a copy
and scans adjacent gaps once — O(n log n) instead of the naive O(n²)
all-pairs comparison. A member function template `addNumber(first, last)`
takes any iterator range for bulk insertion. Edge cases are handled
explicitly: differences are computed in `unsigned int` so `INT_MIN`/`INT_MAX`
spans do not overflow, and two nested exceptions cover a full span and
queries with fewer than 2 values (a range insert that would overflow is
rejected before inserting anything).

The demo is the subject's example (prints `2` then `14`); the tests cover
all exception paths, the integer extremes, a 20,000-element deterministic
range insert, a 100,000-element random fill, and deep-copy semantics.

## ex02 — Mutated abomination (MutantStack)

`std::stack` is a container adapter: it wraps a real container in a
`protected` member `c` and narrows the interface, which is why it has no
iterators. The standard makes `c` protected precisely so derived classes can
extend the adapter — so `MutantStack` publicly inherits from `std::stack`
and adds `begin()`/`end()` (plus const and reverse forms) that forward to
`this->c` (the `this->` is required because `c` lives in a dependent base
class). Exposing the container's iterator typedefs is all it takes to work
with iterator loops and `<algorithm>`, while `MutantStack` remains usable
anywhere a `std::stack` is expected.

The demo is the subject's main; the tests assert its output matches the same
scenario run through a `std::list` (the subject's acceptance criterion), and
also check const/reverse iteration, copy semantics, a `std::vector` backing
container, and a 100k-push stress run.

## Building and running

Each exercise is self-contained:

```sh
cd ex00          # or ex01, ex02
make             # build build/bin/exNN
make run         # build and run the demo main
make test        # build and run tests/test.cpp (exits non-zero on failure)
make fclean      # remove all build artifacts
```

Each exercise also has its own `README.md` with the full design rationale.
