# ex00 — Polymorphism

## The concept

Subtype polymorphism via **virtual dispatch**. When a member function is
declared `virtual` in a base class, a call through a base pointer or
reference is resolved at *runtime* against the object's real type (dynamic
binding, via the vtable). Without `virtual`, the compiler resolves the call
at *compile time* from the static type of the pointer — the derived override
is never reached (static binding / method hiding).

The same rule applies to destructors: `delete` through a base pointer only
runs the derived destructor if the base destructor is `virtual`. A
non-virtual destructor in a polymorphic base is undefined behavior and the
classic source of leaks.

## How this code demonstrates it

Two parallel hierarchies, identical except for one keyword:

| | `Animal` → `Dog`, `Cat` | `WrongAnimal` → `WrongCat` |
|---|---|---|
| `makeSound()` | `virtual` | **not** virtual |
| Call via base pointer | derived sound (`Meow!`, `Woof! Woof!`) | base sound, always |

- `Animal` holds the single protected attribute `std::string type;`.
  `Dog`/`Cat` set it to `"Dog"`/`"Cat"` in their constructors; `getType()`
  exposes it.
- Every class is in Orthodox Canonical Form (default ctor, copy ctor, copy
  assignment, dtor), each printing its own distinct message so construction
  and destruction chains are visible (base ctor first, derived dtor first).
- `Animal::~Animal()` is `virtual`, so `delete` via `Animal*` runs the
  derived destructor then the base one — visible in the program output.
- `main.cpp` runs the subject's example verbatim, then repeats it with the
  Wrong hierarchy to show the failure mode side by side.

## Build / run / test

```sh
make            # builds build/bin/ex00
./build/bin/ex00
make test       # builds and runs tests/test.cpp (exits non-zero on failure)
make fclean
```

`tests/test.cpp` captures `std::cout` (rdbuf swap) and asserts that: a call
through `Animal*` produces the derived sound; deleting through `Animal*`
runs the derived destructor; copies preserve `type`; and a call through
`WrongAnimal&` produces the base sound even on a `WrongCat`.
