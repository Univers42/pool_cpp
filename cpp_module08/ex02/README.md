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
  unchanged, and `std::stack<int> s(mstack);` compiles because the base-class
  copy constructor accepts it (the subject's main requires exactly this).
- `begin()`/`end()` (+ const and reverse forms) simply forward to
  `this->c.begin()` etc. The `this->` is required in C++98: `c` lives in a
  base class that depends on the template parameters, so unqualified lookup
  would not find it.
- Orthodox Canonical Form: default constructor, copy constructor, copy
  assignment, destructor — all trivial forwards to the base.

`main.cpp` is the subject's test main verbatim. Its output is identical to
the same scenario run through a `std::list` (the subject's acceptance
criterion); `tests/test.cpp` asserts that equivalence instead of asking a
human to diff the two outputs.

## Build / run / test

```sh
make            # builds build/bin/ex02
make run        # builds and runs the demo
make test       # builds tests/test.cpp against MutantStack.hpp and runs it
make fclean     # removes all artifacts
```

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`.

`tests/test.cpp` is a plain C++98 program using `assert` (exits non-zero on
failure). It checks:

1. the subject scenario yields the exact same value sequence through
   `MutantStack` and through `std::list`;
2. const and reverse iterators traverse bottom-to-top and top-to-bottom;
3. copy assignment produces an independent copy (OCF);
4. a different underlying container (`std::vector`) still iterates;
5. a 100k-push / 50k-pop stress run, verifying size and the sum of the
   surviving elements.

Valgrind (`--leak-check=full --show-leak-kinds=all --error-exitcode=42`) is
clean on both binaries: 0 leaks, 0 errors.
