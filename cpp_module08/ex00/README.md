# ex00 — Easy find

## Concept

First contact with the STL, which becomes mandatory in Module 08. The
exercise teaches three things at once:

- **Function templates over container types**: `easyfind` takes any type `T`
  and only assumes it is a container of `int` exposing `begin()`/`end()`.
  The template works on `std::vector`, `std::list`, `std::deque`, ... without
  a line of container-specific code — that is the point of iterator-based
  generic programming.
- **`<algorithm>` over hand-rolled loops**: the search is `std::find`, not a
  `for` loop. The subject explicitly rewards using STL algorithms wherever
  they fit.
- **Error signalling**: `std::find` returns `end()` on failure — a sentinel.
  This implementation converts that sentinel into a thrown
  `std::out_of_range`, so a successful return always yields a dereferenceable
  iterator and callers cannot silently ignore a miss.

## Design decisions

- `easyfind` returns the iterator (not the value, not a bool): it is the most
  informative result — position, value, and ability to mutate through it.
- Two overloads: `T&` returning `T::iterator` and `const T&` returning
  `T::const_iterator`, so const containers are searchable too (mirrors how
  the standard library duals `begin()`/`begin() const`).
- The miss throws `std::out_of_range`, the exact exception standard
  containers use for a failed lookup (`vector::at`) — the subject suggests
  taking inspiration from how standard containers behave. A custom exception
  class would force a non-template `what()` implementation, which may not
  live in a header and does not justify an extra `.cpp` on its own.
- Everything lives in `easyfind.hpp`: templates must be visible at
  instantiation, and the subject makes the `.tpp` split optional.
- Associative containers are not handled, as the subject allows.

## Build / run / test

```sh
make            # builds build/bin/ex00
make run        # builds and runs the demo (vector, list, const deque)
make test       # builds tests/test.cpp against the header and runs it
make fclean     # removes all build artifacts
```

The demo (`main.cpp`) shows a successful find, a miss that throws, and a
lookup in a const container. `tests/test.cpp` asserts: found value, first
occurrence on duplicates, throw on miss, throw on empty container, and the
const/list overload — it exits non-zero on any failure.
