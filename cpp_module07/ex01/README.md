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
   decay to a function pointer and be passed straight through.

`main.cpp` demonstrates all three subject cases: printing an int array,
mutating it in place via a non-const-ref callback, and iterating a **const**
string array with an instantiated template callback. `tests/test.cpp` asserts
the same behaviors (plus the length-0 edge case and an exact
invocation-count check that catches off-by-one in either direction) and
exits non-zero on failure.

Everything is header-only, as templates must be: the definition has to be
visible at instantiation time (the 42 rule "no function implementations in
headers" explicitly exempts function templates).

## Build / run / test

```sh
make            # builds build/bin/ex01
make run        # build + execute the demo
make test       # builds and runs tests/test.cpp -> build/bin/test
make fclean     # remove all build artifacts
```

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`.
