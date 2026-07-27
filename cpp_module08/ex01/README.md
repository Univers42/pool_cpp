# ex01 — Span

## The concept

Module 08 is about using the STL instead of reinventing it. `Span` is a
bounded container of ints whose two queries — shortest and longest distance
between any two stored values — are classic "don't write the loop yourself"
problems:

- **longest span** = max element − min element → `std::min_element` /
  `std::max_element` from `<algorithm>`.
- **shortest span** = minimum gap between *adjacent* values once sorted →
  `std::sort` a copy, then one linear scan. Sorting turns an O(n²)
  all-pairs comparison into O(n log n).
- **bulk insertion** via an iterator range → `std::vector::insert(pos,
  first, last)`, exposed through a member *function template* so any
  iterator type works (the template lives in the header, as C++98
  requires).

## Design decisions

- **Storage**: `std::vector<int>`, capacity `reserve()`d up front since N
  is known at construction. Insertion order is preserved; sorting happens
  on a copy inside `shortestSpan()` so the stored sequence is never
  mutated (both queries are `const`).
- **Overflow**: the span between `INT_MIN` and `INT_MAX` does not fit in
  an `int`. Differences are computed in `unsigned int`, whose modular
  arithmetic yields the exact distance for any pair of 32-bit ints —
  no `int64_t` needed (it is not C++98 anyway).
- **Exceptions**: two nested exception classes derived from
  `std::exception` — `SpanFullException` (adding past N, single or range)
  and `NotEnoughElementsException` (querying with fewer than 2 values).
  The range overload checks `std::distance(first, last)` *before*
  inserting, so a rejected insert adds nothing.
- **Orthodox Canonical Form**: default ctor (N = 0, a span that is always
  full), copy ctor, copy assignment, destructor. `std::vector` handles the
  deep copy, so all of them are one-liners.
- `main.cpp` is the subject's example verbatim (prints `2` then `14`);
  the required thorough tests are in `tests/test.cpp`.

## Build / run / test

```sh
make            # builds build/bin/ex01
./build/bin/ex01
# 2
# 14
make test       # builds and runs tests/test.cpp (exits non-zero on failure)
make fclean     # removes all build artifacts
```

Tests cover: the subject example, all exception paths (empty, single
element, full, range insert that would overflow), `INT_MIN`/`INT_MAX`
extremes, a 20,000-element deterministic range insert with known answers,
a 100,000-element random fill, and deep-copy semantics of the canonical
form.
