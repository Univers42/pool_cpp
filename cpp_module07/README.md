# CPP Module 07 — C++ Templates

This module introduces **templates**: writing code once and letting the
compiler generate a version per type. The arc across the three exercises:

1. **ex00** — function templates as generic *functions* (`swap`, `min`, `max`).
2. **ex01** — function templates as generic *algorithms* (`iter`, a hand-rolled
   `std::for_each`), where template argument deduction handles const-correctness
   and arbitrary callables.
3. **ex02** — a *class* template (`Array<T>`), combining templates with
   resource ownership: Rule of Three, deep copy, and exception-based bounds
   checking.

The common thread is compile-time polymorphism: no inheritance, no virtual
dispatch, no runtime cost — and everything lives in headers, because the
compiler needs the full template definition at each instantiation point.

## ex00 — whatever

`whatever.hpp` defines `swap`, `min`, and `max` as function templates that work
for any type supporting copy (for `swap`) and comparison operators (for
`min`/`max`). `min`/`max` take and return `const T&` (no copies of heavy
objects) and return the **second** argument on equality, as the subject
requires. `main.cpp` calls them as `::swap` etc. to avoid ambiguity with the
`std::` versions; the tests verify the equal-arguments rule by comparing
**addresses**, not values.

## ex01 — iter

`iter.hpp` is a five-line template: `iter(T* array, size_t length, F f)`
applies `f` to every element. Two deductions do all the work: `T` picks up
const-ness from the array (so a `const` array only accepts read-only
callbacks — enforced by the compiler, no overloads), and `F` deduces the whole
callable type, so one template accepts `void f(T&)`, `void f(const T&)`, and an
instantiated function template like `print<std::string>`. The demo shows
printing, in-place mutation, and iterating a const array with a template
callback; the tests add the length-0 edge case.

## ex02 — Array

`Array.hpp` is a header-only class template owning `new[]` memory. It shows
value-initialization (`new T[n]()` zeroes the elements), the Rule of Three
(copy constructor, copy assignment, and destructor doing deep copies — no
shared buffers, no double free), and `operator[]` (const and non-const)
throwing an `std::exception`-derived type on out-of-range indices, including
negative ones (which wrap to huge `unsigned int` values). The demo mirrors the
classic evaluation scenario against a raw array; the tests cover empty arrays,
both copy paths, out-of-bounds throws, and a `std::string` instantiation.
Valgrind-clean.

## Building

Each exercise builds independently, in C++98 with
`-Wall -Wextra -Werror`:

```sh
cd exNN
make            # build build/bin/exNN
make run        # build and execute the demo
make test       # build and run tests/test.cpp (exits non-zero on failure)
make fclean     # remove all build artifacts
```

Each exercise also has its own README with design details.
