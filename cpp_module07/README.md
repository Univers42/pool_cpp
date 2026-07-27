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

The other small scar: every call site says `::swap`, `::min`, `::max`. Without
the global-namespace qualifier, headers that drag in `std::swap` and friends
make the call ambiguous. The trade-off I accepted is copy-based `swap` — heavy
types pay a full copy cycle, but C++98 has no move semantics to offer anyway.

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
change the sum. The fix in `tests/test.cpp` is a `countCall` callback and an
exact `g_calls == 3` assertion: value-independent, deterministic, off-by-one
in either direction fails. The trade-off of an unconstrained `F` is C++98
error novels when the callable does not fit; I took the flexibility.

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

## What stuck with me

Templates move whole categories of bugs to compile time, but they also hide
whole categories from casual tests: I now assert identity where references
are the contract, exact invocation counts where iteration is the contract,
and I let valgrind vote on every exception path. Exception safety was
invisible to me until I wrote types whose copies and constructors throw on
purpose — correct-looking code held a leak and a double free for weeks. And a
test suite is itself code that needs testing: the mutants I planted found
more holes in my tests than in my templates.

## Building and testing

Each exercise is self-contained, built as C++98 with
`-Wall -Wextra -Werror`:

```sh
cd ex00        # or ex01, ex02
make           # builds build/bin/exNN
make run       # builds and runs the demo main
make test      # builds and runs tests/test.cpp (exits non-zero on failure)
make fclean    # removes all build artifacts
```

All three suites currently pass; the ex02 suite is also valgrind-clean
(`--leak-check=full`), which is the whole point of the `Bomb` and `Mine`
scenarios. Each exercise keeps its own `README.md` with the finer-grained
design notes.
