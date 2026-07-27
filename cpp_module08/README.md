# Module 08 — Templated containers, iterators, algorithms

After seven modules of being forbidden to touch it, the STL becomes
mandatory here — and the exercises are really about learning to *trust* it.
The arc: first use the library through its two pillars, iterators and
`<algorithm>` (`easyfind`), then wrap a standard container behind my own
invariants (`Span`), and finally extend an adapter that deliberately hides
its container (`MutantStack`). Everything is C++98
(`-Wall -Wextra -Werror -std=c++98`), and every class template lives
entirely in its header, as templates require.

The recurring surprise was how often the honest answer to "how do I
implement this?" turned out to be "you don't — `std::find`, `std::sort` or
`std::max_element` already did, and better." My job shrank to choosing the
right call and defending the edges the library leaves to me.

## ex00 — Easy find

The subject wants `easyfind(container, int)`: find the first occurrence of a
value in any container of ints, or complain. `easyfind.hpp` is iterator
thinking distilled: `std::find(container.begin(), container.end(), value)`,
and one function serves `std::vector`, `std::list` and a const `std::deque`
because the only assumed vocabulary is `begin()`/`end()`. Two overloads —
`T&` returning `typename T::iterator` and `const T&` returning
`typename T::const_iterator` — mirror the standard library's own
`begin()`/`begin() const` duality.

`std::find` reports a miss by returning the end sentinel, which callers can
silently ignore; I convert it into a thrown `std::out_of_range`, the exact
exception `vector::at` uses for a failed lookup, so a successful return is
always dereferenceable. There is a ponytail comment in the header owning the
trade-off: no custom exception class, because a non-template `what()` body
may not live in a header under school rules and a lone class does not
justify an extra `.cpp` — with the upgrade path written down if an evaluator
insists. The tests pin the details I would otherwise hand-wave: first
occurrence on duplicates (index 3, by iterator arithmetic), throws on miss
and on an empty container, and the const overload through a
`const std::list<int>&`.

## ex01 — Span

The subject wants a container of at most N ints answering two queries:
`shortestSpan` and `longestSpan`. Storage is a `std::vector<int>` reserved
up front; longest span is `std::max_element` minus `std::min_element`, and
shortest sorts a *copy* and scans adjacent gaps once — O(n log n) instead of
the naive all-pairs O(n²), with both queries `const` because the stored
sequence is never mutated.

The trick I am most fond of is the overflow one. The span between `INT_MIN`
and `INT_MAX` is 4294967295 — it does not fit in *any* signed 32-bit type,
and C++98 has no `int64_t`. But casting both operands to `unsigned int` and
subtracting wraps mod 2^32 and yields the exact distance for every pair of
ints. The suite verifies the extremes: a Span holding `{INT_MIN, INT_MAX}`
reports `UINT_MAX` for both spans. A ponytail comment in `Span.cpp` records
why and the upgrade path if ints ever grow.

The member template `addNumber(first, last)` is where I got burned once:
capacity is checked with `std::distance(first, last)` *before*
`vector::insert`, so an oversized range throws `SpanFullException` and
inserts nothing. That atomicity has a regression test that proves it without
a size getter — after a failed 5-element insert into a `Span(4)` holding one
value, exactly three more singles fit and the fourth throws. The documented
ceiling of that design: distance-then-insert is two passes, so the range
overload needs forward iterators; a single-pass input iterator would be
consumed by the counting walk. I also once built a four-billion-element Span
under a lowered `ulimit -v` just to watch `reserve` throw `std::bad_alloc` —
an experiment that stayed at the terminal, but it settled what "full" means
when the allocator gives up before I do.

Tests beyond the subject's `2` then `14`: every exception path, a
20,000-element deterministic range insert with known answers (shortest 3),
a 100,000-element random fill, and deep-copy semantics of the canonical
form.

## ex02 — Mutated abomination (MutantStack)

The subject asks for a `std::stack` with iterators, and the exercise is
really one revelation: `std::stack` is not a container, it is an *adapter*.
It wraps a real container in a member named `c`, narrows the interface to
`push`/`pop`/`top`/`size`/`empty` — and the standard declares `c`
`protected` precisely so derived classes can widen it again.

So `MutantStack<T, Container = std::deque<T> >` publicly inherits from
`std::stack<T, Container>`, re-exports `Container`'s four iterator typedefs,
and its `begin()` is literally `return this->c.begin();`. The iterators are
not mine — they are the underlying container's, re-exposed. The one C++98
scar: the `this->` is mandatory, because `c` lives in a base class that
depends on the template parameters and unqualified lookup will not enter it.
No hacks, no reimplementation; a `MutantStack` still *is* a `std::stack`,
which is why `std::stack<int> s(mstack);` compiles in the tests.

The subject's acceptance criterion is that replacing `MutantStack` with
`std::list` in its main produces the same output. Instead of eyeballing two
terminals, `tests/test.cpp` runs the subject scenario through both, captures
every printed value, and asserts the two sequences are identical element for
element — plus const and reverse iteration, an independent copy, a
`std::vector` backing container, and a 100k-push/50k-pop stress run. The
trade-off here was accepting inheritance from a class with no virtual
destructor; it is the standard's own intended extension point, and nothing
deletes a `MutantStack` through a base pointer.

## What stuck with me

Iterators are the STL's real interface: once `easyfind` spoke
`begin()`/`end()`, containers became interchangeable, and once `MutantStack`
exposed `c`'s iterators, `<algorithm>` worked on a stack. I stopped fearing
edge cases I can turn into arithmetic — the unsigned-wrap distance trick is
two casts replacing a type that C++98 does not have. And "strong guarantee"
became a habit rather than a phrase: check capacity before mutating, and
write the regression that proves a failed operation changed nothing.

## Building and testing

Each exercise is self-contained:

```sh
cd ex00        # or ex01, ex02
make           # builds build/bin/exNN
make run       # builds and runs the demo main
make test      # builds and runs tests/test.cpp (exits non-zero on failure)
make fclean    # removes all build artifacts
```

All three suites currently pass. Each exercise keeps its own `README.md`
with the full design rationale, including the ponytail comments' context.
