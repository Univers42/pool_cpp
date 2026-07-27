# ex02 — Identify real type

## The concept

RTTI-driven downcasting with `dynamic_cast`, without the `<typeinfo>`
machinery. Given only a `Base*` or `Base&`, recover the *dynamic* type of a
polymorphic object at runtime. This works because `Base` has a virtual
destructor, which gives every derived object a vtable that `dynamic_cast`
can interrogate.

The exercise's real lesson is the asymmetry between the two forms of
`dynamic_cast`:

- **Pointer form** — `dynamic_cast<A*>(p)` returns `NULL` on mismatch.
  Failure is a testable value; no exceptions involved.
- **Reference form** — `dynamic_cast<A&>(r)` has no "null reference" to
  return, so failure **throws** `std::bad_cast`. Failure is an exception.

The subject forbids `<typeinfo>` (so no `typeid`, and no naming
`std::bad_cast` in a catch clause) and forbids using any pointer inside
`identify(Base&)` — forcing you to use both failure modes honestly.

## How this code demonstrates it

- `Base.hpp` / `Base.cpp` — `Base` with a public virtual destructor only
  (destructor body in the `.cpp`: 42 rules forbid function bodies in
  headers). `A.hpp`, `B.hpp`, `C.hpp` — empty publicly-derived classes.
- `Functions.cpp`
  - `generate()` — `std::rand() % 3` picks A, B, or C, returned as `Base*`.
    It announces what it built so the demo output is verifiable by eye.
  - `identify(Base*)` — three pointer-form casts; first non-NULL wins.
  - `identify(Base&)` — three reference-form casts, each in a `try` block.
    A successful cast prints and returns; a failed one throws and falls
    through to the next candidate. The handler is `catch (...)` because
    naming `std::bad_cast` would require the forbidden `<typeinfo>`
    header. No pointer appears anywhere in the function.
- `main.cpp` — seeds `rand`, then five times: generate, identify by
  pointer, identify by reference, delete. The generated tag and the two
  identify lines must always agree.

Casting choice for the defense: `dynamic_cast` is the *only* cast that
checks the dynamic type at runtime (`static_cast` would "succeed"
unconditionally and lie). The one `static_cast` in `main.cpp` is the
standard `time_t` → `unsigned int` seed conversion.

## Build / run / test

```sh
make            # builds build/bin/ex02
make run        # build + run the demo
make test       # builds tests/test.cpp -> build/bin/test and runs it
make fclean     # remove all build artifacts
```

`tests/test.cpp` is a standalone C++98 program: it redirects `std::cout`
into a `stringstream`, asserts that both overloads print exactly `A`, `B`,
or `C` for known objects (including through a `Base*` upcast), and that
both overloads agree on 20 randomly generated objects. Any failure exits
non-zero, which fails `make test`.

Valgrind (`--leak-check=full --show-leak-kinds=all`) reports zero leaks
and zero errors on both the demo and the test binary.
