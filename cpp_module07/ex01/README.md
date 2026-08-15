# ex01 — Iter

## The concept

Function templates as generic algorithms. `iter` is a tiny, hand-rolled
`std::for_each`: it walks any array and applies a callable to every element,
with the compiler deducing all the types at the call site. The exercise's real
lesson is **template argument deduction and const-correctness**: the same
`iter` must accept

- non-const arrays with callbacks that mutate elements (`void f(T&)`),
- const arrays with read-only callbacks (`void f(const T&)`),
- an *instantiated function template* as the callback.

## How this code demonstrates it

`iter.hpp` declares the whole solution in five lines:

```cpp
template <typename T, typename F>
void iter(T* array, const size_t length, F f) {
  for (size_t i = 0; i < length; ++i) f(array[i]);
}
```

Two design decisions do all the work:

1. **`T` deduces const-ness from the array.** Passing a
   `const std::string[]` deduces `T = const std::string`, so `array[i]` is a
   const lvalue and only binds to a `const T&` parameter — the compiler
   enforces const-correctness for free, with no overloads needed.
2. **`F` deduces the callable's whole type** instead of forcing a fixed
   signature like `void (*)(T&)`. That is what lets one template accept both
   `void f(int&)` and `void f(const int&)`, and lets `print<std::string>`
   (an instantiated function template, explicitly required by the subject)
   decay to a function pointer and be passed straight through. It also
   accepts a function-pointer variable, a functor object, a functor whose
   `operator()` is itself a template, and a callback that returns a value
   (the result is simply discarded). The price of an unconstrained `F` is
   C++98 error novels from *inside* the template when the callable does not
   fit, instead of at the call site.

Everything is header-only, as templates must be: the definition has to be
visible at instantiation time (the 42 rule "no function implementations in
headers" explicitly exempts function templates).

## The two traps this exercise hides

**`F` is a by-value parameter.** Pass a functor that accumulates state and the
accumulation lands on `iter`'s *copy*, which dies when `iter` returns — the
caller's functor is untouched. `std::for_each` has exactly this shape, which is
why it *returns* the functor; `iter` does not, so the state has to live
somewhere the copy still points at (a functor holding `int*` works). Both
halves are demonstrated in `main.cpp` section 4 and asserted in the tests.

**The length is the one thing nothing deduces.** `iter` takes `T*`, so an array
has already decayed to a pointer by the time it arrives and
`sizeof(a)/sizeof(a[0])` at the wrong place silently yields 2 instead of 7.
Both g++ and clang ship a dedicated warning for that mistake
(`-Wsizeof-pointer-div`). `main.cpp` shows the C++98 way out — a
`template <typename T, size_t N> size_t lengthOf(T (&)[N])` that binds a
reference to the *array*, so `N` is deduced and a decayed pointer will not bind
at all.

## Build / run / test

```sh
make            # builds build/bin/ex01
make run        # build + run the guided tour
make test       # builds and runs tests/test.cpp -> build/bin/test
make fclean     # remove all build artifacts
```

`main.cpp` is an eight-section guided tour: the subject's three required cases,
why `iter` has two template parameters (and every callable shape `F` accepts),
const-ness riding on `T`, the by-value functor trap, exact invocation counts,
the length problem above, arbitrary element types, and the empty/NULL edges.

Three layers of checking, because a passing suite proves nothing on its own:

```sh
make verify               # all three layers, stops on the first failure
make test                 # 48 assertions, fixed cases + fuzz
./build/bin/test 1234     # same, different fuzz seed
./tests/mutants.sh        # 11 injected defects; all must be caught
./tests/compile_fail.sh   # 10 cases: 9 that must NOT compile, 1 control
```

The suite records the **sequence** of visited values rather than a sum or a
count, because a backwards loop has the same count *and* the same sum as a
correct one — only the sequence separates them. It also brackets the working
range with sentinel slots, so an off-by-one in either direction lands on a
guard whatever the values happen to be. That is what earlier versions of this
suite missed: a mutant iterating `length + 1` times survived, caught only when
adjacent stack garbage happened to change the sum.

`tests/compile_fail.sh` is where the headline claim lives, since a test that
fails to compile is not a test: passing a mutating callback to a `const` array
must be **rejected**, and it is — const-correctness enforced by deduction
alone, no second overload, no `const_cast`, no runtime check.

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`; also builds warning-free
under `g++`. Valgrind-clean.
