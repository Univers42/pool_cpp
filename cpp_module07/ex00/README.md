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
  the simplest thing that works in C++98.
- `min`/`max` take and return `const T&` — no copy of potentially heavy
  objects like `std::string`, and the return can bind to the caller's
  originals.
- On equal arguments both return the **second** parameter, as the subject
  requires. `(a < b) ? a : b` and `(a > b) ? a : b` yield `b` when `a == b`,
  so no extra equality branch is needed. The test suite verifies this by
  comparing addresses, not values.
- `main.cpp` calls them as `::swap` etc. — the `::` qualifier forces lookup in
  the global namespace so there is no ambiguity with `std::swap`/`std::min`/
  `std::max` that headers may drag in.

## Build / run / test

```sh
make            # builds build/bin/ex00
./build/bin/ex00
make test       # builds and runs tests/test.cpp (exits non-zero on failure)
make fclean     # removes all build artifacts
```

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`.
