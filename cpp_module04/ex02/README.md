# ex02 — Abstract class

## The concept

An **abstract class** is a class that cannot be instantiated. In C++ you get
one by declaring at least one **pure virtual function**:

```cpp
virtual void makeSound() const = 0;
```

The `= 0` means the base class provides no implementation and the compiler
refuses to construct the class directly. Only derived classes that override
every pure virtual function are concrete (instantiable). This turns the base
class into a pure *interface + shared state* contract: "every animal makes a
sound, but there is no such thing as a generic animal sound".

Why it matters after ex00/ex01: in ex00 `Animal` was concrete, so
`new Animal()` compiled and gave a meaningless "generic animal". The subject
asks us to close that hole while keeping everything else (polymorphism,
virtual destructor chain, `Brain` deep copy) working unchanged.

## How this code demonstrates it

- **`AAnimal`** (renamed from `Animal` with the subject-allowed `A` prefix)
  declares `makeSound() const = 0`. `AAnimal a;` or `new AAnimal()` no longer
  compile — see the commented lines in `main.cpp` and `tests/test.cpp`.
  It still implements the Orthodox Canonical Form and a **virtual destructor**
  so deleting a `Dog`/`Cat` through an `AAnimal*` runs the full chain
  (Brain → Dog/Cat → AAnimal).
- **`Dog` / `Cat`** override `makeSound()`, so they are concrete. Each owns a
  heap-allocated `Brain*` with deep copy semantics (copy ctor and assignment
  allocate a new `Brain`; self-assignment guarded) carried over from ex01.
- **`WrongAnimal` / `WrongCat`** are kept as "files from previous exercise":
  the counter-example with a non-virtual `makeSound()`.
- `main.cpp` is the subject-style demo: build a `Dog` and a `Cat` through
  `const AAnimal*`, call `makeSound()` polymorphically, show deep copy, and
  delete through the base pointer.

Key design decisions:

- Abstract-ness via pure virtual `makeSound()` — the smallest change that
  satisfies "not instantiable"; a protected constructor would also work but
  the subject explicitly points at the interface method.
- The old concrete `Animal` class was **deleted**: keeping it alongside
  `AAnimal` would leave the forbidden `new Animal()` path alive.
- Non-instantiability is a compile-time property, so the test file proves it
  with a commented `AAnimal a;` line rather than a compile-fail harness.

## Build / run / test

```sh
make            # builds ./build/bin/ex02
./build/bin/ex02
make test       # builds tests/test.cpp against exercise objects and runs it
make fclean     # remove all artifacts
```

Tests (`tests/test.cpp`, plain C++98, exits non-zero on failure) check:
virtual dispatch through `AAnimal*` (output captured via `rdbuf`), deep copy
of `Brain` on copy-construction and assignment, and the self-assignment guard.

Valgrind is clean on both binaries:

```sh
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=42 ./build/bin/ex02
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=42 ./build/bin/test
```
