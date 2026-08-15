# ex02 — Mutated abomination (MutantStack)

## The concept

`std::stack` is not a container — it is a **container adapter**. It wraps a
real sequence container (`std::deque` by default) in a member conventionally
named `c`, and deliberately narrows its interface to `push`/`pop`/`top`/
`size`/`empty`. That narrowing is why `std::stack` has no iterators: the
adapter hides the container that has them.

The exercise teaches two things:

1. **Adapters hide, they don't destroy.** The standard declares `c` as a
   `protected` member precisely so derived classes can extend the adapter.
   Inheriting from `std::stack` gives legitimate access to `c` — no hacks,
   no copies, no reimplementation.
2. **Iterator typedefs are the STL contract.** Exposing
   `typename Container::iterator` under the name `MutantStack<T>::iterator`
   (plus `const_`/`reverse_` variants) is all it takes for the class to work
   with iterator-based loops and `<algorithm>`.

## How this code demonstrates it

`MutantStack.hpp` is the whole implementation (a class template, so it lives
entirely in the header — allowed by the module rules):

- `template <typename T, typename Container = std::deque<T> > class
  MutantStack : public std::stack<T, Container>` — public inheritance means a
  `MutantStack` **is** a `std::stack`: every member function is inherited
  unchanged (which is what the evaluation sheet means by "offers all of its
  member functions" — none of them is rewritten here), and
  `std::stack<int> s(mstack);` compiles because the base-class copy
  constructor accepts it. The subject's main requires exactly this.
- `begin()`/`end()` (+ const and reverse forms — eight accessors in total)
  simply forward to `this->c.begin()` etc. The `this->` is required in C++98:
  `c` lives in a base class that depends on the template parameters, so at
  template-definition time the compiler does not know the base has a member
  called `c` and unqualified lookup refuses to search it. Bare `c.begin()`
  does not compile, and `tests/compile_fail.sh` proves it with a
  side-by-side broken copy of the class.
- `explicit MutantStack(const Container&)` re-exposes the `std::stack`
  constructor that takes a container to start from. Declaring *any*
  constructor hides every base-class one, so without this line
  `MutantStack<int> m(existingDeque)` stops working.
- Orthodox Canonical Form: default constructor, copy constructor, copy
  assignment, destructor — all trivial forwards to the base. The
  compiler-generated versions would have been correct too; they are written
  out because the sheet asks to see them.
- A ponytail comment owns the one real trade-off: `std::stack` has no virtual
  destructor, so deleting a `MutantStack` through a `std::stack*` is
  undefined behaviour. Nothing here ever does, and the trigger to revisit is
  the day something stores these polymorphically — which would mean
  composition rather than inheritance.

## The walkthrough

`main.cpp` closes the scenario the module has been building — the event log
of the access-control system from ex00 and ex01 — in ten sections: the
subject's main verbatim, its acceptance criterion checked against
`std::list`, why `std::stack` has no iterators and where these came from, all
four iterator flavours, ex00's `easyfind` running on a stack, the event log
as a live undo/audit structure, three different backing containers, the
canonical form and the deliberate slice to `std::stack`, a 100k/50k stress
run ending in `std::accumulate` over a stack, and a recap of what the three
exercises add up to. `make run` prints it.

## Build / run / test

```sh
make            # builds build/bin/ex02
make run        # the walkthrough
make test       # everything below, in order; non-zero on the first failure
make quick      # just the test binaries, without the check scripts
make fclean     # removes all artifacts
```

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`.

`make test` runs:

| what | where | covers |
| --- | --- | --- |
| 105 assertions | `tests/test.cpp` | every inherited `std::stack` member, the `std::list` equivalence, iteration order in all four flavours, empty ranges, writing through iterators, `<algorithm>`/`<numeric>`, slicing to the base, the canonical form, three backing containers, the container constructor, a non-`int` element type, element lifetime, and a 100k/50k stress run |
| 4 property checks | `tests/fuzz.cpp` | 2,000 randomised push/pop/write/copy scripts replayed against a `std::deque` shadow on all three backings, the subject's criterion generated rather than eyeballed, slicing, and assignment |
| 15 requirement checks | `tests/subject.cpp` | the subject's main and its `std::list` twin, both run with `std::cout` captured and compared as text |
| 28 pipeline checks | `tests/cumulative.cpp` | ex00 + ex01 + ex02 driven as one system |
| 37 checklist items | `tests/eval.sh` | the 42 evaluation sheet, mechanised |
| 20 mutants | `tests/mutants.sh` | injects one defect at a time; all 20 must be caught |
| 17 compile cases | `tests/compile_fail.sh` | code that must **not** compile |

The parts worth knowing about:

- **`tests/cumulative.cpp` is the point of the module.** It builds the event
  log as a `MutantStack`, asks ex00's `easyfind` whether a given swipe was
  recorded, and hands `log.begin(), log.end()` straight to ex01's
  `Span::addNumber(first, last)` to compute the tailgating alarm — over a
  live log that gets popped and pushed between queries, on all three backing
  containers. Each exercise ships alone, so `easyfind` is copied verbatim and
  `Span` is inlined into the test (a test binary links one translation unit
  and cannot compile `ex01/Span.cpp`); what is *not* vendored, and is the
  actual claim under test, is the joint. `Span::addNumber` names no container
  anywhere and was written before `MutantStack` existed. That these fit is a
  property to be asserted, not a coincidence to be admired.
- **`tests/mutants.sh` found two real blind spots** in `tests/test.cpp` on
  its first run, which is the entire reason it exists: one mutant broke only
  the *const* `rend()` (the suite checked `rbegin()` but never walked the
  const reverse range to its end), and another made `operator=` return the
  source instead of `*this` (every value came out right, and only the address
  differs). Both tests were added; all 20 mutants now die. Two defects
  deliberately have no mutant and the script says why: dropping `this->` is a
  compile error in every instantiation, so it proves nothing about the tests
  and belongs in `compile_fail.sh`; and removing the self-assignment guard is
  an equivalent mutant, since the only member is a `std::deque` held by the
  base.
- **`tests/compile_fail.sh` carries this exercise's most important claim**,
  which is a negative one: the identical `easyfind` template that compiles
  against a `MutantStack` is *rejected* against a plain `std::stack`. No test
  binary can assert that, because the code proving it does not compile. The
  same file covers `c` staying protected, the two-phase-lookup rule, const
  iteration being read-only, the one-way slice, and the iterator-category
  limits of a list-backed stack (`ms.begin() + 1` and `std::sort` both
  rejected — neither is a runtime error).
- **`tests/eval.sh`** checks the sheet's ex02 items directly: public
  inheritance from `std::stack`, that no inherited member is reimplemented,
  all four typedefs, all eight accessors, that no accessor uses bare `c.`,
  and that `main()` has more tests than the subject's own.

Valgrind is clean on all five binaries: 0 leaks, 0 errors.
