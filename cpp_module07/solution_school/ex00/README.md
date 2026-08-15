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

`main.cpp` is a thirteen-section guided tour rather than a bare demo: the
subject's scenario, one template across five types, the `::` story above,
reference identity on ties, the zero-copy cost of `const T&` (counted), what
`swap` really costs in C++98, why `::min(1, 2.5)` will not deduce, why a
template is only compiled when instantiated, `std::complex`, the three
polymorphism sections below, and the dangling-reference trap that returning
`const T&` buys you. Every `OK`/`BAD` line is counted; the program exits
non-zero if any of them turn, so `make run` is a check and not just a read.

## Sections 10–12: the same templates, pointed at a hierarchy

`Animal` is abstract (`makeSound` is pure virtual), `Dog` and `Cat` each own a
heap-allocated `Brain` holding 100 `std::string` ideas. That pair of facts is
enough to make the three templates disagree with each other:

- **`min`/`max` accept an abstract `T`; `swap` cannot.** `min` and `max` take
  `const T&` and return `const T&` — no `T` is ever constructed, so it does not
  matter that no `Animal` can exist. `swap` opens with `T temp = a;`, which
  needs a real `Animal`, and the call is rejected. Section 9 split the three
  functions by which *operators* a type has; this splits them by what the
  *body* does with a `T`. Same lesson, opposite axis.
- **That refusal is the point.** Had `Animal` been concrete, `::swap(a, b)` on
  two `Animal&` would have compiled and *sliced*: the two base halves traded,
  each animal left with the other's name and its own `Brain`. Being unable to
  instantiate the base turns a silent bug into a build error.
- **`min` returning a reference is what keeps polymorphism alive.**
  `::min(a, b).makeSound()` still reaches `Cat::makeSound`, because the
  reference points at the caller's own object. Returning `T` by value would
  have sliced it — if it compiled at all.
- **The array the subject wants is an array of `Animal*`.** `Animal zoo[4]` is
  impossible for the same reason a variable is. Section 11 sorts `Animal* [4]`
  with a selection sort built from nothing but `::min` and `::swap`: `min`'s
  reference return answers *which* argument won by address, and `::swap`
  instantiates `swap<Animal*>`, moving two pointers while the animals stay put.
  The sort comes out stable for free — same species compares equal, and `min`
  returns the second on a tie.
- **`::min(zoo[0], zoo[1])` compiles and compares addresses.** `T` deduces to
  `Animal*`, pointers have `operator<`, and the answer is which allocation sits
  lower in memory. Nothing can warn you. `min`/`max` want the object, `swap`
  wants the pointer, and the `*` is the entire difference.
- **`::min(rex, mia)` does not compile** — a `Dog&` and a `Cat&` deduce `T` as
  two different types and deduction stops, exactly like `::min(1, 2.5)`.
  `::min<Animal>(rex, mia)` names `T` and both convert.
- **`Dog::operator=` is copy-and-swap, written with `::swap` itself.** A deep
  copy is made first, so a throw leaves `*this` untouched; then `swap<Brain*>`
  moves one pointer rather than 100 strings, and `tmp`'s destructor frees the
  brain we used to own. Freeing is never written down — it falls out of who
  holds what when `tmp` leaves scope.

Both leak checks are in the program, not delegated to valgrind: `Animal::live()`
and `Brain::live()` count live instances, and each section asserts they return
to zero. Remove the `virtual` from `~Animal` and `Brain::live()` stays at 4
after the `delete` loop — the `OK` becomes a `BAD` and the exit code follows.
Under valgrind the run is 199 allocations, 199 frees, no leaks, no errors.

Three layers of checking, because a passing suite proves nothing on its own:

```sh
make verify               # all three layers, stops on the first failure
make run                  # the tour; non-zero exit if a verdict line turned
make test                 # 78 assertions, fixed cases + fuzz
./build/bin/test 1234     # same, different fuzz seed
./tests/mutants.sh        # 13 injected defects; all must be caught
./tests/compile_fail.sh   # 21 cases: 17 that must NOT compile, 4 controls
```

The fuzz section checks *identity* invariants as well as values, and asserts
its own coverage (that the run actually produced ties, and both strict
orderings) — a fuzz run that never generated a tie would prove nothing about
the rule that matters most here.

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`; also builds warning-free
under `g++`. Valgrind-clean.
