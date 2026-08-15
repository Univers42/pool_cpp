# Module 07 — C++ Templates

This is the module where the compiler stopped being a gatekeeper and became a
collaborator. A template is code I write once and the compiler rewrites per
type, at compile time, for free — no inheritance, no virtual dispatch, no
runtime cost. The three exercises escalate deliberately: generic *functions*
(`swap`/`min`/`max`), a generic *algorithm* (`iter`), and finally a generic
*class that owns memory* (`Array<T>`), which is where templates collided with
the Rule of Three and exception safety, and where this module genuinely hurt.

One rule I internalized on day one: templates live in headers. The compiler
needs the full definition at every instantiation point — it cannot generate
code for `Array<YourType>` from a declaration alone, so a template body hidden
in a `.cpp` links only for the types its author happened to think of. That is
why `whatever.hpp`, `iter.hpp` and `Array.hpp` *are* the implementation, and
why the school rule against function bodies in headers explicitly exempts
function templates.

## ex00 — whatever

The subject wants three function templates: `swap` exchanges two values,
`min`/`max` compare two and, on equality, return the **second** argument. My
`whatever.hpp` is exactly that: `swap` goes through a temporary copy (the
simplest thing that works in C++98), and `min`/`max` take and return
`const T&` so a `std::string` never gets copied just to be compared.

The equal-returns-second rule fell out of the ternary for free: `(a < b) ? a
: b` and `(a > b) ? a : b` both yield `b` when `a == b`, so there is no
special-case branch. The subtlety is that this requirement is *unverifiable by
value* — when `x` and `y` are both 5, comparing the result to 5 proves
nothing. The tests instead compare addresses: `&::min(x, y) == &y`. Returning
references is what makes that observable at all, and it is the first time a
test taught me to assert *identity* rather than value.

The other small scar: every call site says `::swap`, `::min`, `::max`. I used
to explain that as "otherwise it is ambiguous with `std::swap`", and writing
`tests/compile_fail.sh` proved me wrong — the truth is worse. `swap(str1,
str2)` **compiles**, and quietly calls `std::swap`: `<string>` declares a
`swap` overload just for `basic_string`, which is *more specialised* than my
generic `swap(T&, T&)`, so partial ordering prefers it. It swaps buffers in
O(1) instead of making my three copies, and nothing tells you your template was
never involved. The test now proves which one ran by watching the string's
buffer address move. A real ambiguity needs a candidate exactly as generic as
mine; that case lives in the compile-fail script. So the `::` is not decoration
— without it you get the wrong function or no function. The trade-off I
accepted is copy-based `swap` — heavy types pay a full copy cycle, but C++98
has no move semantics to offer anyway.

## ex01 — iter

The subject wants `iter(array, length, function)` applying the function to
every element, and it must accept an instantiated function template as the
callback. My whole answer is one loop in `iter.hpp`:

```cpp
template <typename T, typename F>
void iter(T* array, const size_t length, F f) {
  for (size_t i = 0; i < length; ++i) f(array[i]);
}
```

The dual template parameters are the entire design. `T` deduces the element
type *including const-ness*: pass a `const std::string[]` and `T` becomes
`const std::string`, so `array[i]` is a const lvalue. `F` deduces the whole
callable type instead of pinning a signature like `void (*)(T&)`, which is
what lets one template accept `void f(int&)`, `void f(const int&)`, and
`print<std::string>` — an instantiated function template decaying to a plain
function pointer.

The part I can prove rather than promise: feeding a mutating callback to a
const array does not compile. I kept the experiment — instantiating
`::iter(b, 3, increment)` on `const int b[]` dies with *binding reference of
type `int&` to `const int` discards qualifiers*. Const-correctness enforced by
deduction, no overloads written.

Where I got burned was the tests, not the template. My first suite checked
sums of visited values, and a mutant that iterated `length + 1` times slipped
through — it was only ever caught when adjacent stack garbage happened to
change the sum. Counting invocations fixed that, and then a *backwards* mutant
showed that a count is not enough either: reversing the loop preserves both the
count and the sum. The suite now records the whole visit **sequence** and
brackets the range with sentinel slots, so order, repeats and overruns in
either direction all fail deterministically.

The other thing `F` hides: it is a **by-value** parameter. A functor that
accumulates state accumulates into `iter`'s copy, which dies on return — the
caller's object is untouched. That is the same shape as `std::for_each`, which
is why the standard one *returns* the functor. `iter` does not, so state has to
live somewhere the copy still points at. The trade-off of an unconstrained `F`
is C++98 error novels when the callable does not fit; I took the flexibility.

## ex02 — Array

The subject wants `Array<T>`: `new[]`-owned storage, default construction to
an empty array, `Array(n)` with value-initialized elements (`new T[n]()`),
deep copies, and `operator[]` that throws on a bad index. `Array.hpp` does all
of it header-only; `OutOfBoundsException` answers `what()` with
`"Error: Array index is out of bounds!"`, and because the index parameter is
`unsigned int`, the demo's `numbers[-2]` wraps to a huge value and the same
`index >= _size` check catches it.

The saga was exception safety, and I only found it by building adversarial
element types in `tests/test.cpp`. `Bomb` is a struct whose copy-assignment
throws when a countdown hits zero. Copying a 4-element `Array<Bomb>` with the
old code detonated mid-copy inside the copy constructor, and valgrind reported
16 bytes definitely lost — four 4-byte `Bomb`s in a buffer whose owner never
finished constructing, so its destructor never ran. `Mine` throws from its
*default* constructor, and it exposed the mirror bug in the old `operator=`:
free the old buffer first, then allocate — so when `new T[n]()` threw,
`_array` was left dangling and the destructor freed it a second time.

Both bugs died with one refactor, the private static `cloneBuffer`: allocate
and copy into the fresh buffer *first*, `delete[]` it and rethrow if any
element copy throws, and only touch `*this` once nothing can fail. That is the
strong guarantee, and the `Mine` test pins it down: after a throwing
assignment, `x.size()` is still 3 and its contents are intact. The trade-off
inside `cloneBuffer` is `new T[n]()` followed by element assignment — `T` must
be default-constructible and every element is initialized twice — which I took
over placement-new gymnastics in C++98.

The other lesson came from mutation testing my own suite. A deliberately
broken `operator=` that early-returned on an empty right-hand side *passed
every test I had*. The suite grew accordingly: `Array(0)` (which still calls
`new T[0]()`, while copying it takes `cloneBuffer`'s `n == 0` path),
`b = empty` asserting the target really becomes size 0, and a const in-bounds
read so the const `operator[]` overload is actually executed, not just
compiled.

Mutation testing also handed me a result I did not expect: **the
`if (this != &rhs)` guard is not what makes self-assignment safe here.**
Because `cloneBuffer` builds the new buffer before `delete[] _array` runs, the
guard is redundant for correctness — deleting it is an *equivalent mutant*, and
no value-based test could ever kill it. Getting the ordering right retired the
guard's usual job. What it still buys is the pointless allocate-and-copy it
skips, so that is what the suite asserts: self-assignment constructs zero new
elements. `Bomb`, `Mine` and `Tracked` all count their live instances, which
turns "a half-built Array orphaned its buffer" into an unbalanced ledger the
tests can see without valgrind's help.

## What stuck with me

Templates move whole categories of bugs to compile time, but they also hide
whole categories from casual tests: I now assert identity where references
are the contract, exact invocation *sequences* where iteration is the contract,
and I let valgrind vote on every exception path. Exception safety was
invisible to me until I wrote types whose copies and constructors throw on
purpose — correct-looking code held a leak and a double free for weeks. And a
test suite is itself code that needs testing: the mutants I planted found more
holes in my tests than in my templates, and the compile-fail scripts found a
factual error in my own notes about `::` that had survived every review.

The last thing I learned is that half a template's contract is the code it
**refuses**, and no ordinary test can assert that — a test that fails to
compile is not a test. That half needs its own harness.

## Building and testing

Each exercise is self-contained, built as C++98 with
`-Wall -Wextra -Werror`:

```sh
cd ex00        # or ex01, ex02
make           # builds build/bin/exNN
make run       # builds and runs the guided tour in main.cpp
make test      # builds and runs tests/test.cpp (exits non-zero on failure)
make verify    # test + mutants.sh + compile_fail.sh, stops on first failure
make fclean    # removes all build artifacts
```

`make verify` is the one-command check. It lives in `autotools/Makefile.in`, so
it reaches any exercise on regeneration, and it degrades safely: an exercise
with no `tests/*.sh` scripts just runs `test` and says so.

Each `main.cpp` is a **guided tour**, not a bare demo: numbered sections, each
one a thing the subject teaches or a thing that bit me, with the claims printed
next to the values that back them. `make run` is the readable version of this
README.

Every exercise carries three layers of checking, because a passing suite proves
nothing until you show it can fail:

| | ex00 | ex01 | ex02 |
|---|---|---|---|
| `make test` assertions (+ fuzz, seeded) | 78 | 48 | 109 + 50 |
| `./tests/mutants.sh` injected defects, all killed | 13 | 11 | 20 |
| `./tests/compile_fail.sh` must-not-compile cases (+1 control each) | 8 | 9 | 10 |

The test binaries take an optional seed (`./build/bin/test 1234`); the fuzz
sections assert their own coverage, so a run that never generated the case that
matters fails rather than passing quietly. `mutants.sh` mirrors the exercise
into `build/mutants/` and drops a mutated header in place — the header-only
equivalent of swapping an object file at link time.

All three suites pass, build warning-free under both `clang++` and `g++`, and
are valgrind-clean (`--leak-check=full`) on the success *and* error paths —
which is the whole point of the `Bomb` and `Mine` scenarios. Each exercise
keeps its own `README.md` with the finer-grained design notes.
