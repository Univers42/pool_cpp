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

`iter.hpp` is header-only, as a template must be: the definition has to be
visible at instantiation time, and the 42 rule "no function implementations in
headers" explicitly exempts function templates. That exemption is *only* for
templates, so the two ordinary classes here are split the usual way —
`vect2.hpp`/`vect2.cpp` and `bigint.hpp`/`bigint.cpp`, both in `SRC_LIST`. See
"Sections 8–10" below for what they are for.

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

## Sections 8–10: complex element types

`iter` never changes, so the way to push on it is to change what goes *through*
it. Two classes live next to it for that: `vect2` (a pair of ints with the full
operator set) and `bigint` (an unbounded non-negative integer kept as decimal
digits). Each one makes a different point that `int` cannot.

**`vect2` — const-ness reaches past the element.** `vect2` carries two
`operator[]` overloads, and `T` deduced from the array picks between them:
`BoundsInto` reads `p[axis]` through a `const vect2&` and gets the one
returning `int`, then writes `(*lo)[axis]` through a plain `vect2*` and gets
the one returning `int&`. Section 3 showed const riding on `T` for a primitive;
on a class it reaches every member the callback calls.

The same section separates the two prototypes that look identical at the call
site:

```cpp
void bumpByCopy(vect2 v)  { ++v; }   // element untouched
void bumpByRef (vect2& v) { ++v; }   // element incremented
```

`iter` writes `f(array[i])` and lets the callback's parameter decide whether
that is the element or a copy of it. "Any prototype" cuts both ways — the
useless one is just as valid, and just as quiet.

It also settles what section 4 leaves open: the by-value `F` is only a trap for
*accumulators*. `Translate` and `ScaleBy` carry input, so `iter`'s copy is worth
exactly as much as the original. Whether `F` may be copied is a property of the
functor, not of `iter`.

**Arrays of arrays — `iter` calling itself.** Pass a `vect2[2][4]` and `T`
deduces to `vect2[4]`, an *array type*: `iter`'s `T*` is `vect2 (*)[4]` and
`array[i]` is a whole row. A callback binding `const T (&row)[N]` then runs
`iter` on that row, so the outer call walks rows and the inner one walks
corners. `printRow<vect2, 4>` is the subject's "instantiated function template
as a callback" requirement showing up a second time, one level down.

That case also cuts section 6's length problem exactly in half: the row arrives
as a *reference to an array*, so `N` is part of its type and cannot be got
wrong, while the outer array decayed to a pointer on the way in and its `2` is
still yours to get right. Same array, both answers, one call apart.

**`bigint` — the visit order is the answer, and the copy has a price.** Every
other section here would pass just as well with the loop running backwards.
Horner's method would not: `iter` over an `int[39]` with
`*acc <<= bigint(1); *acc += bigint(digit);` reconstructs 2¹²⁷−1 exactly, and
the same digits visited in reverse give a different number. Forward order is
part of `iter`'s contract, which is why the suite records the *sequence* rather
than a count or a sum.

The Fibonacci pass makes the array the *output* — 100 `bigint` slots filled in
one call, with the two terms the recurrence needs carried behind pointers — and
then reads it back:

```cpp
::iter(fib, 100, FibInto(&prev, &cur));   // fills F(0..99), leaves F(101) in cur
::iter(fib, 100, SumInto(&total));        // sum F(0..99) + 1 == F(101)
```

That identity is a nice thing to be able to write and a **weak check**, which
is worth stating because I originally believed the opposite. Anything that hits
both passes the same way cancels out of it: reversing `iter` leaves the sum
untouched (addition commutes, and the array still holds the same hundred
numbers), and breaking `bigint`'s final carry breaks both sides together. I
found that out by mutating `iter` and watching the identity keep holding. What
actually pins the pass down is the known value — `fib[99] ==
218922995834555169026` — and Horner, whose answer changes when the order does.
A check that survives the mutation was never a check.

Finally, `bigint` prices the section 4 trap instead of asserting it. On an `int`
counter the by-value copy is free and the lost state is the only cost; a
`bigint` copy is a heap allocation, and the class counts its own copies. The
measurement: **1** copy for the whole `iter` call — `F` is copied into the
parameter once and reused for all 100 invocations — and **0** for a functor
holding a pointer. That is why every `...Into` type in the file is shaped that
way.

Both classes were repaired from sketches, so both are checked on their own
before `iter` is allowed to drive them (`bigint`'s carry chains, normalisation,
shifts and ordering; `vect2`'s full operator set). A wrong answer coming out of
`iter` looks identical whether `iter` or the element type produced it, and only
one of those is the subject of this exercise.

## Build / run / test

```sh
make            # builds build/bin/ex01
make run        # build + run the guided tour
make test       # builds and runs tests/test.cpp -> build/bin/test
make fclean     # remove all build artifacts
```

`main.cpp` is an eleven-section guided tour: the subject's three required cases,
why `iter` has two template parameters (and every callable shape `F` accepts),
const-ness riding on `T`, the by-value functor trap, exact invocation counts,
the length problem above, arbitrary element types, the three complex-type
sections described above, and the empty/NULL edges. Every `OK`/`BAD` line is
counted and the program exits non-zero if one turns, so `make run` is a check
and not just a read.

Three layers of checking, because a passing suite proves nothing on its own:

```sh
make verify               # all three layers, stops on the first failure
make run                  # the tour; non-zero exit if a verdict line turned
make test                 # 100 assertions, fixed cases + fuzz
./build/bin/test 1234     # same, different fuzz seed
./tests/mutants.sh        # 11 injected defects; all must be caught
./tests/compile_fail.sh   # 17 cases: 14 that must NOT compile, 3 controls
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
