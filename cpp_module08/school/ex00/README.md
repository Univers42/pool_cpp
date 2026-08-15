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
  they fit, and the evaluation sheet goes further: a manual iterator search
  is graded as *wrong*, even when it returns the right answer.
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
- Associative containers are not handled, as the subject allows — though
  `std::set` works anyway, because its `value_type` really is `int`.
  `std::map`'s is a pair, so it does not and cannot.

## The walkthrough

`main.cpp` tells the whole thing as one scenario — the badge registry of a
building's access-control system — in eleven sections: the contract, the same
call on four containers, the const overload, writing/erasing/inserting
through the returned iterator, first-match-wins with `std::distance`, the
cost difference between `std::find` on a `std::set` and `set::find`, a failed
lookup leaving the container untouched, the exception's place in the
`std::exception` hierarchy, what the template deliberately refuses, a badge
revocation loop that uses a miss as its exit condition, and the handover to
ex01. `make run` prints it.

The scenario continues into ex01 (`Span` over the same swipe timestamps) and
ex02 (`MutantStack` as the event log), and `ex02/tests/cumulative.cpp` runs
all three together.

## Build / run / test

```sh
make            # builds build/bin/ex00
make run        # the walkthrough
make test       # everything below, in order; non-zero on the first failure
make quick      # just the test binaries, without the check scripts
make fclean     # removes all build artifacts
```

`make test` runs four things:

| what | where | covers |
| --- | --- | --- |
| 62 assertions | `tests/test.cpp` | fixed cases: contract, first occurrence, search shape, exception type and message, empty/tiny containers, read-only behaviour, iterator-as-handle, const overload, five container types, `INT_MIN`/`INT_MAX`, a differential run against `std::find` over 60 needles, 200k elements, nested containers, and an element type that merely compares with an `int` |
| 8 property checks | `tests/fuzz.cpp` | 30,000 randomised rounds against a hand-written oracle (deliberately *not* `std::find`), across vector/list/deque/const and a `std::set`, plus a read-only invariant |
| 12 requirement checks | `tests/subject.cpp` | the subject's six requirements, one section each |
| 29 checklist items | `tests/eval.sh` | the 42 evaluation sheet, mechanised |
| 14 mutants | `tests/mutants.sh` | injects one defect at a time; all 14 must be caught |
| 17 compile cases | `tests/compile_fail.sh` | code that must **not** compile |

The parts worth knowing about:

- **`tests/test.cpp`'s "search shape" section** uses a `Probe` type that
  counts its own comparisons. That is how a test proves `easyfind` scans
  front to back and *stops at the first hit* — three comparisons to find the
  third element, six for a full miss — rather than merely producing the right
  answer by some other route.
- **`tests/eval.sh`** covers the rules that get a project flagged rather than
  marked down, and every one of them is a property of the source that no test
  binary can assert: the Makefile's compiler and flags, a clean build under
  the evaluator's own command line, `-pedantic-errors` as the real
  "C++98 only" gate, no `using namespace`, no `friend`, no C functions, only
  standard headers, and — the sheet's own words — that the search "HAS to use
  STL algorithms". A correct hand-written loop passes every runtime test ever
  written, so grep is the only instrument that can see it. The
  no-function-in-a-header rule is checked by *linking two translation units*
  that both include the header, which is the actual failure mode rather than
  a guess at it.
- **`tests/mutants.sh`** breaks the header fourteen ways — dropping the miss
  check from one overload at a time, inverting the condition, searching for
  `value + 1`, returning `++it`, taking the container by value so the
  returned iterator dangles — and requires `tests/test.cpp` to notice each
  one. Two defects deliberately have no mutant, and the script says why:
  replacing `std::find` with a *correct* manual loop is an equivalent mutant
  that no assertion could ever kill (it is `eval.sh`'s job), and dropping
  `#include <algorithm>` would test the test file's include order rather than
  the header.
- **`tests/compile_fail.sh`** covers what the type system enforces: writing
  through a `const_iterator`, a C array, `std::map`, an element type that
  does not compare with an `int`, a missing `typename` on a dependent name,
  and `>>` closing nested templates in C++98. It has three outcomes, not two
  — one case is legal C++98 that `-Werror` correctly rejects (`easyfind(v,
  3.9)` silently narrowing to `3`), which proves the required flags are doing
  work the standard alone would not.

Valgrind is clean on all four binaries: 0 leaks, 0 errors.
