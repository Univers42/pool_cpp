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
`std::inner_product` already did, and better." My job shrank to choosing the
right call and defending the edges the library leaves to me.

## One scenario across three exercises

The three mains tell a single story — a building's access-control system —
so that each exercise picks up where the last one left off:

- **ex00** searches the badge registry. `easyfind` answers *is this exact
  value present*, on whatever container the subsystem happens to use.
- **ex01** takes the swipe timestamps of one badge and asks a question no
  search can answer: *how close together do any two of them get*. Three
  seconds apart is a tailgating alarm.
- **ex02** holds the whole event log as a stack, so the controller can undo
  the most recent event — and, because it is a `MutantStack`, still audit the
  history without consuming it, run `easyfind` over it, and hand
  `log.begin(), log.end()` to `Span::addNumber` for the alarm.

That last sentence is the module's thesis, so it is a test rather than a
claim: `ex02/tests/cumulative.cpp` runs all three exercises as one pipeline,
on a log that changes between queries, across three backing containers.

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
insists.

The test I am fondest of here uses a `Probe` type that counts its own
comparisons, which is how the suite proves `easyfind` scans front to back and
*stops at the first hit* — three comparisons for the third element, six for a
full miss — rather than merely landing on the right answer by some other
route.

## ex01 — Span

The subject wants a container of at most N ints answering two queries:
`shortestSpan` and `longestSpan`. Storage is a `std::vector<int>` reserved
up front; longest span is `std::max_element` minus `std::min_element`, and
shortest sorts a *copy* and folds the adjacent gaps — O(n log n) instead of
the naive all-pairs O(n²), with both queries `const` because the stored
sequence is never mutated.

That fold is `std::inner_product`, which is misnamed: it is the general
two-range fold, `init = combine(init, pairwise(*first1, *first2))`. Handed
the sorted range offset by one against itself, with `combine` = minimum and
`pairwise` = unsigned distance, it *is* "the smallest gap between
neighbours" — one pass, no temporary vector of differences, and no
hand-written loop anywhere in the class. The evaluation sheet asks for
exactly that ("member functions use STL algorithms to find their result as
much as possible"), and it is the kind of requirement no runtime test can
check, so `tests/eval.sh` greps for the loop instead.

The trick I am most fond of is the overflow one. The span between `INT_MIN`
and `INT_MAX` is 4294967295 — it does not fit in *any* signed 32-bit type,
and C++98 has no `int64_t`. But casting both operands to `unsigned int` and
subtracting wraps mod 2^32 and yields the exact distance for every pair of
ints. The suite verifies the extremes: a Span holding `{INT_MIN, INT_MAX}`
reports `UINT_MAX` for both spans.

The member template `addNumber(first, last)` is where I got burned once:
capacity is checked with `std::distance(first, last)` *before*
`vector::insert`, so an oversized range throws `SpanFullException` and
inserts nothing. The documented ceiling of that design: distance-then-insert
is two passes, so the range overload needs forward iterators; a single-pass
input iterator would be consumed by the counting walk.

`Span` also grew a small read-only STL surface — `size()`, `maxSize()`,
`empty()`, `full()`, `begin()`, `end()`, a `const_iterator` typedef and a
free `operator<<`. Only *const* iteration, because a mutable iterator would
let a caller resize the vector behind the class's back and N is the one thing
`Span` exists to enforce; and `operator<<` free rather than a `friend`, which
the evaluation sheet flags, because public `begin()`/`end()` make privileged
access unnecessary. The side effect is the nicest thing in the module:
**ex00's `easyfind` accepts a `Span` with nothing written on either side for
it.**

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
which is why `std::stack<int> s(mstack);` compiles. The trade-off accepted
here is inheritance from a class with no virtual destructor; it is the
standard's own intended extension point, and nothing deletes a `MutantStack`
through a base pointer.

The subject's acceptance criterion is that replacing `MutantStack` with
`std::list` in its main produces the same output. Instead of eyeballing two
terminals, `tests/subject.cpp` runs both versions with `std::cout` captured
and compares the text; `tests/fuzz.cpp` then generates thousands more scripts
and demands the same thing of each.

## What stuck with me

Iterators are the STL's real interface: once `easyfind` spoke
`begin()`/`end()`, containers became interchangeable; once `Span` spoke it,
code written before `Span` existed accepted one; and once `MutantStack`
exposed `c`'s iterators, `<algorithm>` worked on a stack. I stopped fearing
edge cases I can turn into arithmetic — the unsigned-wrap distance trick is
two casts replacing a type C++98 does not have. And "strong guarantee"
became a habit rather than a phrase: check capacity before mutating, and
write the regression that proves a failed operation changed nothing.

The other thing that stuck is that a passing suite proves nothing until it
has been shown it can fail. The mutation runs earned that twice over — ex02's
found two genuine blind spots in tests I had just written (a const `rend()`
never walked to the end, and an `operator=` whose return value nothing
checked), and the ex01 walkthrough's own "planted answer" turned out to be
wrong for a reason worth keeping: at 100,000 random 32-bit draws a duplicate
is not an edge case, it is the expected outcome.

## Building and testing

Each exercise is self-contained:

```sh
cd ex00        # or ex01, ex02
make           # builds build/bin/exNN
make run       # builds and runs the walkthrough main
make test      # the full check: test binaries, then the three scripts
make quick     # just the test binaries
make fclean    # removes all build artifacts
```

**Reading the output.** Every result line starts with `ok`, `FAIL` or `n/a`,
and that first word is the only one that says whether something is wrong. The
second column is what happened, in the vocabulary of that particular check,
and for a healthy run it is *supposed* to look alarming:

| second column | means | good? |
| --- | --- | --- |
| `rejected` | the compiler refused to build a case that must not build | **yes** |
| `accepted` | a control case built cleanly, as required | **yes** |
| `-Werror` | legal C++98 that the required flags correctly reject | **yes** |
| `killed` | an injected bug was caught by the tests | **yes** |
| `killed(c)` | an injected bug was caught at compile time | **yes** |
| `SURVIVED` | an injected bug got past the tests — a blind spot | no |
| `COMPILED` | something that must not compile did | no |
| `PATTERN` | a mutation did not apply — harness problem, not a result | no |

So a run of forty lines all reading `rejected` and `killed` is a clean run.
The scripts also drop their colours when stdout is not a terminal, so pasting
a log somewhere does not carry a wall of escape codes.

`make test` is the whole story and exits non-zero on the first failure. It
builds and runs every `tests/*.cpp` as its own binary, then runs, in order:

- **`tests/eval.sh`** — the 42 evaluation sheet, mechanised. Everything in it
  is a rule that gets a project *flagged or ungraded* rather than marked
  down, and every one is a property of the source that no test binary can
  assert: the Makefile's compiler and flags, a clean build under the
  evaluator's own command line, `-pedantic-errors` as the real "C++98 only"
  gate, no `using namespace`, no `friend`, no C functions, only standard
  headers, orthodox canonical form, and the per-exercise items the sheet
  singles out. "A function implemented in a header file" is checked by
  *linking two translation units* that both include it, rather than by
  guessing with grep, and the forbidden-keyword greps run against
  comment-stripped copies so that explaining a rule is not mistaken for
  breaking it.
- **`tests/mutants.sh`** — injects one defect at a time and requires
  `tests/test.cpp` to catch it. Where a defect is an *equivalent mutant*
  (identical observable behaviour, so no assertion could ever kill it) the
  script says so and says which other tool covers it instead.
  `VG=1 ./tests/mutants.sh` runs every mutant under valgrind.
- **`tests/compile_fail.sh`** — the code that must *not* compile, with
  positive controls that must. A test that fails to compile is not a failing
  test, it is a broken build, so these claims cannot live in the suite.

Current state, from a clean `make fclean && make test` in each directory:

| | assertions | eval | mutants | compile cases |
| --- | --- | --- | --- | --- |
| ex00 | 82 across 3 binaries | 29 | 14/14 killed | 17 |
| ex01 | 137 across 3 binaries | 38 | 27/27 killed | 18 |
| ex02 | 152 across 4 binaries | 37 | 20/20 killed | 17 |

### The lab

`all.cpp` is not turn-in code and is deliberately not built by any exercise.
It is the scratchpad that goes wider than the subject is allowed to — C++17,
so `std::array`, `std::forward_list` and the `unordered_` containers are on
the table — and it is one program in five parts: the container typedefs
(`value_type`, `size_type`, `difference_type`, `reference`, iterator
categories, each demonstrated and several pinned with `static_assert`);
`easyfind` in its C++98, C++14 `auto` and C++11 `decltype`/`declval`
spellings, asserted to return the same iterator *and* the same type; then
working copies of `Span` and `MutantStack`; then `easyfind` driven across ten
container types. Every answer is asserted, so a wrong one aborts the run
rather than scrolling past.

```sh
cd cpp_module08
make run        # the lab
make check      # everything below
```

The two newer spellings of `easyfind` live here rather than in
`ex00/easyfind.hpp` on purpose. Preprocessing them out of the exercise would
not have been enough: a grader reads the file, not the translation unit, and
the sheet treats a post-C++98 feature as a forbidden one wherever it appears.

### Whole-module verification

```sh
sh ../autotools/check_all.sh          # or: make check
```

Per exercise: a clean build, then the evaluator's own command line
(`-pedantic-errors` included, because a Makefile can lie about its flags),
then `make test`, then **valgrind on every binary**, then a rebuild under
`-fsanitize=address,undefined`, then the fuzzers on three further seeds.
Then the lab, then the class generator's self-test. 23 checks, all passing.

The sanitizer pass earns its place: valgrind sees the heap, while ASan also
sees stack and global overflows and UBSan catches signed overflow — the exact
class of bug the unsigned-distance trick in `Span` exists to avoid. Injecting
`maxVal - minVal` as signed ints into a throwaway copy left every assertion
passing and the fuzzer green; only UBSan noticed.
