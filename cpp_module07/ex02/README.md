# ex02 — Array

## The concept

A class template `Array<T>` is a container written once and instantiated per
type: `Array<int>`, `Array<std::string>`, etc. The exercise teaches:

- **Class templates**: the whole class lives in the header (`Array.hpp`),
  because the compiler must see the full definition at each instantiation.
- **Value-initialization**: `new T[n]()` — the trailing `()` default-initializes
  every element, so `Array<int>(5)` holds five zeros (same reason
  `int* a = new int();` gives `*a == 0`).
- **Deep copy / Rule of Three**: a class owning `new[]` memory needs a copy
  constructor, copy assignment operator, and destructor that manage that
  memory, otherwise copies share one buffer and `delete[]` runs twice.
- **Bounds checking with exceptions**: `operator[]` throws an
  `std::exception`-derived type on an out-of-range index instead of reading
  unallocated memory.

## How this code demonstrates it

`Array.hpp` (header-only, C++98):

- `Array()` — empty array, no allocation (`_array = NULL`, size 0); the
  subject forbids preventive allocation.
- `Array(unsigned int n)` — `new T[n]()`, elements default-initialized.
- Copy constructor and `operator=` allocate a fresh buffer and copy element by
  element; mutating one array never affects the other. Assignment is
  self-assignment safe.
- `operator[]` (const and non-const overloads) throws the nested
  `OutOfBoundsException : public std::exception` when `index >= _size`.
  A negative index like `numbers[-2]` converts to a huge `unsigned int` and is
  caught by the same check.
- `size() const` returns the element count.

`main.cpp` is the classic evaluation scenario: fill an `Array<int>` and a raw
mirror array with the same random values, copy the Array in an inner scope
(copies are destroyed there — proves deep copy, no double free), verify the
original still matches the mirror, then trigger both out-of-bounds exceptions.

`tests/test.cpp` is an assert-based check (exits non-zero on failure) covering
empty arrays, `Array(0)`, default initialization, deep copy through both copy
paths, assignment from an empty array, out-of-bounds throws on const and
non-const access, `size()`, and a `std::string` instantiation.

## Build / run / test

```sh
make            # builds build/bin/ex02
./build/bin/ex02
make test       # builds and runs tests/test.cpp against Array.hpp
make fclean     # removes all build artifacts
```

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`. Valgrind-clean
(`--leak-check=full`) on both binaries.
