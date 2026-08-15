# ex02 — Array

## The concept

A class template `Array<T>` is a container written once and instantiated per
type: `Array<int>`, `Array<std::string>`, etc. The exercise teaches:

- **Class templates**: the whole class lives in the header (`Array.hpp`),
  because the compiler must see the full definition at each instantiation.
- **Value-initialization**: `new T[n]()` — the trailing `()` default-initializes
  every element, so `Array<int>(5)` holds five zeros (same reason
  `int* a = new int();` gives `*a == 0`).
- **Deep copy / Rule of Three**: a class owning `new[]` memory needs a copy
  constructor, copy assignment operator, and destructor that manage that
  memory, otherwise copies share one buffer and `delete[]` runs twice.
- **Bounds checking with exceptions**: `operator[]` throws an
  `std::exception`-derived type on an out-of-range index instead of reading
  unallocated memory.

## How this code demonstrates it

`Array.hpp` is header-only because it must be — a class template's definition
has to be visible at every instantiation point, which is the one case the 42
rule "no function implementations in headers" exempts. The exercise also
carries `iter.hpp` (a function template, same exemption) plus `vect2` and
`bigint` as ordinary `.hpp`/`.cpp` pairs in `SRC_LIST`, since that exemption
does not reach them. They are the complex element types sections 9 and 10 need.

`Array.hpp` (C++98):

- `Array()` — empty array, no allocation (`_array = NULL`, size 0); the
  subject forbids preventive allocation.
- `Array(unsigned int n)` — `new T[n]()`, elements default-initialized.
- Copy constructor and `operator=` allocate a fresh buffer and copy element by
  element; mutating one array never affects the other. Assignment is
  self-assignment safe.
- `operator[]` (const and non-const overloads) throws the nested
  `OutOfBoundsException : public std::exception` when `index >= _size`.
  A negative index like `numbers[-2]` converts to a huge `unsigned int` and is
  caught by the same check.
- `size() const` returns the element count.

## Exception safety: the part that took the longest

The subject never mentions it, and it is where this exercise actually hurt.
Both bugs were found by writing element types that throw on purpose:

- **`Bomb`**'s copy-assignment throws, detonating partway through a buffer
  copy. The old copy constructor allocated into `_array` and copied in place,
  so a throw left a buffer owned by an object whose constructor never
  completed — no destructor ever runs for it. Valgrind: *16 bytes definitely
  lost*, four `Bomb`s nobody owned.
- **`Mine`**'s *default* constructor throws, detonating inside `new T[n]()`.
  That exposed the mirror bug in the old `operator=`: it freed the old buffer
  first and allocated second, so a throw left `_array` dangling and the
  destructor freed it a second time.

Both died with one refactor — the private static `cloneBuffer`: build the new
buffer completely, `delete[]` it and rethrow if any element copy throws, and
only touch `*this` once nothing can fail. That is the **strong guarantee**: the
operation either succeeds or changes nothing at all.

A consequence worth knowing: **that ordering makes the `if (this != &rhs)`
guard redundant for correctness.** `cloneBuffer` copies before
`delete[] _array` runs, so self-assignment reads live memory and is right
without it. Deleting the guard is an *equivalent mutant* — no value-based test
could ever kill it (`tests/mutants.sh` says so explicitly, and valgrind
agrees). What it actually buys is the pointless allocate-and-copy it skips,
which is what the suite asserts instead: self-assignment constructs zero new
elements. In a free-first implementation the guard would be load-bearing.

## Build / run / test

```sh
make            # builds build/bin/ex02
make run        # build + run the guided tour
make fclean     # removes all build artifacts
```

`main.cpp` runs the subject's evaluation main **verbatim** as section 1 — fill
an `Array<int>` and a raw mirror with the same random values, copy the Array in
an inner scope so the copies are destroyed there, verify the original still
matches, then trigger both out-of-bounds exceptions. The nine sections after it
cover what that main cannot show: `new T[n]()` value-initialisation, the Rule
of Three, both `operator[]` overloads and the unsigned-index wrap, the
exception-safety saga above, why each instantiation is a separate type
(including `Array<Array<int> >` and the mandatory space in `> >`), the two
different ways to get an empty array, and then the three described below.

Every `OK`/`BAD` line is counted and the program exits non-zero if one turns,
so `make run` is a check and not just a read.

### Sections 8–10: the eval sheet, complex types, and `iter`

**8 — the sheet, one line at a time.** Each grading question answered by a live
check rather than by prose. `Array<Tracked>(5)` brings exactly five objects into
existence and the scope end destroys exactly five, which is what `new T[5]()`
and `delete[]` do and what a single `new T` or a `malloc` could not; the empty
and sized constructors; `operator[]` reading and writing on a non-const and
reading only on a const; and the out-of-range throw caught as
`const std::exception&` — the base the subject actually names, not
`OutOfBoundsException`, so nothing in the caller has to know the exception's
real name.

**9 — simple and complex types**, which the sheet demands by name. `Array<vect2>`
and `Array<bigint>` come out of `new T[n]()` already value-initialised, and
`bigint` is the type that separates a real deep copy from a `memcpy`: each one
owns a `std::string`, so a shared buffer would show up as changed data first and
as a double free second. `Array<Array<int> >` nests the whole thing — the outer
`[]` hands back an `Array<int>&`, the inner one indexes it, both are
bounds-checked, and every inner buffer is freed by `~Array` running as
`delete[]` destroys the outer one. None of that was written for the nested case;
it falls out of the Rule of Three being right.

**10 — `iter` over an `Array`, and where the checking stops.** `&a[0]` is the
whole bridge, and it works only because `new T[n]` gives one contiguous block —
a container built from nodes could not hand `iter` a pointer at all. What is
worth noticing is what happens either side of it: every access through `Array`
is bounds-checked and throws, and the moment `iter` has the pointer, none of
them are. `iter` walks the length it was handed, and `a[0]` is the last check it
will ever see.

```cpp
template <typename T, typename F>
void iterArray(Array<T>& a, F f) {
  if (a.size() == 0) return;   // &a[0] would throw: there is no element 0
  ::iter(&a[0], a.size(), f);
}
```

The `const` overload is where the deduction chain is worth watching: a
`const Array&` selects the const `operator[]`, so `&a[0]` is a `const T*`, so
`iter` deduces `T = const T`, so a mutating callback will not bind. Four
deductions in a row, none of them written down, and the error lands on the one
line that asked for the write. The section fills an `Array<bigint>(60)` with
Fibonacci in one pass and sums it in another through that const overload:
`Array` does the owning and the checking, `iter` does the walking, and neither
template knows the other exists.

Three layers of checking, because a passing suite proves nothing on its own:

```sh
make verify               # all three layers, stops on the first failure
make run                  # the tour; non-zero exit if a verdict line turned
make test                 # 129 assertions + 50 conformance checks
./build/bin/subject       # the subject's bullets, one section each
./build/bin/test 1234     # same, different fuzz seed
./tests/mutants.sh        # 20 injected defects; all must be caught
VG=1 ./tests/mutants.sh   # same, with each mutant run under valgrind
./tests/compile_fail.sh   # 16 cases: 13 that must NOT compile, 3 controls
valgrind --leak-check=full ./build/bin/test
```

`Bomb`, `Mine` and `Tracked` all **count their live instances**, so a buffer
orphaned by a half-built `Array` shows up as an unbalanced ledger — the leak is
caught in-process, without needing valgrind to notice. Run under valgrind
anyway: a double free is invisible to a counter. One fuzz section detonates at
*every* position in arrays of every size from 1 to 8 and requires the strong
guarantee each time; another checks random operation sequences against a plain
C-array model (STL containers are off-limits until module 08).

`tests/subject.cpp` is a separate, narrower audit: one section per subject
bullet, in the subject's own order, so each requirement can be pointed at
directly at defence. It is where the *allocation* rules get checked — "you MUST
use `operator new[]`" and "preventive allocation is forbidden" are claims no
amount of poking at values can confirm, so the element type carries a
class-level `operator new[]`/`delete[]` that counts calls. The default
constructor performs **zero** array allocations; `Array(4)` performs exactly
one, through `new[]`; and every `new[]` is matched by exactly one `delete[]`.

`tests/compile_fail.sh` covers what the type system enforces and no runtime
test can reach: `explicit` blocking `Array<int> a = 5;`, the const `operator[]`
refusing writes, `Array<int>` and `Array<double>` having no conversion between
them, and `Array<Array<int>>` failing to parse in C++98 without the space.

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`; also builds warning-free
under `g++`. Valgrind-clean (`--leak-check=full`) on both binaries, on the
success *and* error paths.
