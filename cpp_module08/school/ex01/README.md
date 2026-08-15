# ex01 — Span

## The concept

Module 08 is about using the STL instead of reinventing it. `Span` is a
bounded container of ints whose two queries — shortest and longest distance
between any two stored values — are classic "don't write the loop yourself"
problems:

- **longest span** = max element − min element → `std::min_element` /
  `std::max_element` from `<algorithm>`.
- **shortest span** = minimum gap between *adjacent* values once sorted →
  `std::sort` a copy, then fold the adjacent pairs. Sorting turns an O(n²)
  all-pairs comparison into O(n log n).
- **bulk insertion** via an iterator range → `std::vector::insert(pos,
  first, last)`, exposed through a member *function template* so any
  iterator type works (the template lives in the header, as C++98
  requires).

The evaluation sheet is unusually specific about this exercise: member
functions should use STL algorithms "as much as possible", and finding the
shortest span "can't be done only by subtracting the two lowest numbers".
Both are addressed head-on below.

## Design decisions

- **Storage**: `std::vector<int>`, capacity `reserve()`d up front since N
  is known at construction. Insertion order is preserved; sorting happens
  on a copy inside `shortestSpan()` so the stored sequence is never
  mutated (both queries are `const`).
- **The adjacent-gap pass is `std::inner_product`**, not a loop.
  `inner_product` is misnamed: it is the general two-range fold, computing
  `init = combine(init, pairwise(*first1, *first2))` at each step. Feeding it
  the sorted range offset by one against itself, with `combine` = minimum and
  `pairwise` = unsigned distance, reduces to "the smallest gap between
  neighbours" in a single pass — no temporary vector of differences, and not
  a hand-written loop anywhere in the class. The two functors live in an
  unnamed namespace in `Span.cpp`.
- **Overflow**: the span between `INT_MIN` and `INT_MAX` does not fit in
  an `int`. Differences are computed in `unsigned int`, whose modular
  arithmetic yields the exact distance for any pair of 32-bit ints —
  no `int64_t` needed (it is not C++98 anyway). Signed overflow, by
  contrast, is undefined behaviour: the compiler may assume it never
  happens.
- **Exceptions**: two nested exception classes derived from
  `std::exception` — `SpanFullException` (adding past N, single or range)
  and `NotEnoughElementsException` (querying with fewer than 2 values).
  The range overload checks `std::distance(first, last)` *before*
  inserting, so a rejected insert adds nothing.
- **Orthodox Canonical Form**: default ctor (N = 0, a span that is always
  full), copy ctor, copy assignment, destructor. `std::vector` handles the
  deep copy, so all of them are one-liners.
- **A read-only STL surface**: `size()`, `maxSize()`, `empty()`, `full()`,
  `begin()`, `end()` and a `const_iterator` typedef, plus a free
  `operator<<`. Only *const* iteration is exposed — a mutable iterator would
  let a caller resize the vector behind the class's back, and N is the one
  thing `Span` exists to enforce. `operator<<` is a free function and
  emphatically not a `friend` (which the evaluation sheet flags): `begin()`
  and `end()` are public, so it needs no privileged access.
- That same typedef is why **ex00's `easyfind` accepts a `Span` unchanged**.
  Nothing was written for it on either side; `easyfind`'s entire vocabulary
  is `begin()`, `end()` and `T::const_iterator`.

## The walkthrough

`main.cpp` runs the scenario ex00 started — the swipe log of the same badge
registry — in eleven sections. Section 1 is the subject's example verbatim
(`2` then `14`); section 2 puts the naive "two lowest, subtracted" answer
next to the real one on the subject's own numbers (3 versus 2); the rest
covers the capacity invariant, the fewer-than-two case, bulk insertion from
four different iterator sources, the `INT_MIN`/`INT_MAX` trick, `easyfind`
applied to a `Span`, a tailgating alarm built out of `shortestSpan()`,
100,000-element runs, the canonical form, and the handover to ex02.
`make run` prints it.

One section is worth calling out because it started as a bug in the
walkthrough rather than in the code: filling a `Span(100000)` from a random
32-bit generator reports a shortest span of **0**, and that is the data being
right, not `Span` being wrong. With 100,000 draws from a range of ~2³⁰ the
expected number of colliding pairs is about 4.7 — the birthday paradox. The
section now says so and confirms the duplicate independently with
`std::adjacent_find` before showing a collision-free fill where the answer is
knowable in advance.

## Build / run / test

```sh
make            # builds build/bin/ex01
./build/bin/ex01
make run        # the walkthrough
make test       # everything below, in order; non-zero on the first failure
make quick      # just the test binaries, without the check scripts
make fclean     # removes all build artifacts
```

`make test` runs:

| what | where | covers |
| --- | --- | --- |
| 107 assertions | `tests/test.cpp` | the subject example, shortest-is-not-the-two-lowest across four hand-picked cases, `max - min`, the capacity ceiling, `Span(0)` and the default `Span` being born full, both under-populated cases, the exception hierarchy, range insertion from four sources, atomicity, `INT_MIN`/`INT_MAX`, query purity, the canonical form, the accessors and `operator<<`, `<algorithm>` compatibility, and 10k/100k runs with constructed answers |
| 11 property checks | `tests/fuzz.cpp` | 4,000 rounds against an O(n²) oracle, a random-operation state machine against a `std::vector` shadow, atomicity fuzzing, copy independence, and shuffled arithmetic progressions at 10k–40k elements |
| 19 requirement checks | `tests/subject.cpp` | the subject's six requirements plus the sheet's two, with the example main's output captured and compared byte for byte against `"2\n14\n"` |
| 38 checklist items | `tests/eval.sh` | the 42 evaluation sheet, mechanised |
| 27 mutants | `tests/mutants.sh` | injects one defect at a time; all 27 must be caught |
| 18 compile cases | `tests/compile_fail.sh` | code that must **not** compile |

The parts worth knowing about:

- **The two oracles in `tests/fuzz.cpp`** are both deliberately dumber than
  `Span`. The first is the O(n²) definition of "shortest/longest distance
  between any two stored numbers", straight from the subject's wording — the
  sort-then-fold is an optimisation of exactly that, and an optimisation is
  only worth having if it can be shown to agree with what it replaced. The
  second is a `std::vector` shadow replayed through the same random sequence
  of operations, which catches state bugs a query-only test never would:
  capacity accounting drifting after a rejected insert, order lost, a copy
  quietly sharing storage.
- **`tests/mutants.sh`'s first mutant is the evaluation sheet's named
  mistake** — replacing the fold with `sorted[1] - sorted[0]`. It is written
  so that it still *compiles*, because the kill has to come from the
  assertions rather than from the compiler. The other 26 cover both
  functors, the fold's identity value, the range offset, both `< 2` guards,
  the capacity checks, the canonical members and every part of the range
  overload. One defect deliberately has no mutant and the script says why:
  removing the `if (this != &rhs)` guard from `operator=` is an *equivalent
  mutant* here, since both members are self-assignment-safe — it is not
  equivalent for a class owning raw memory, which is why module 07's `Array`
  can kill the same mutation and this cannot.
- **`tests/eval.sh`** checks the flags, a clean build under the evaluator's
  own command line, `-pedantic-errors`, the absence of `using namespace` /
  `friend` / C functions / non-C++98 spellings, that both `what()` bodies
  live in `Span.cpp` rather than the header (the trap in this exercise), that
  all four canonical members are both declared and defined, and the sheet's
  two ex01-specific items: no hand-written loop inside either query, and no
  `sorted[1] - sorted[0]`. Its forbidden-keyword greps run against
  comment-stripped copies of the sources — a comment saying "not a friend" is
  not a use of `friend`, and a checker that cannot tell the difference trains
  you to stop explaining yourself.
- **`tests/compile_fail.sh`** covers `explicit` blocking `int → Span` at a
  declaration and at a call site, the const-only iteration (including
  `std::sort(sp.begin(), sp.end())` being rejected), the private storage, the
  deliberately absent `Span::iterator`, and — as a positive control —
  ex00's `easyfind` compiling against a `Span` with no adaptation.

Valgrind is clean on all four binaries: 0 leaks, 0 errors.
