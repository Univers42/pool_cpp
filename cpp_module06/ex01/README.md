# ex01 — Serialization

## The concept

`reinterpret_cast` is the C++ cast for conversions the type system cannot
check: reinterpreting a bit pattern as an unrelated type. Its canonical safe
use is exactly this exercise — converting a pointer to an integer wide enough
to hold it (`uintptr_t`) and back. The standard guarantees that
`reinterpret_cast<Data*>(reinterpret_cast<uintptr_t>(p)) == p`, so the
round-trip is lossless. Anything fancier (casting between unrelated object
types, then dereferencing) is undefined behavior — the exercise teaches where
the safe boundary lies.

The exercise also shows the C++98 idiom for a non-instantiable utility class:
declare every constructor, the copy assignment operator, and the destructor
`private` and never define them. Any attempt to create a `Serializer` fails at
compile time (or link time from within the class itself). This is what C++11
later spelled `= delete`.

## The code

- `Data.hpp` — a plain non-empty struct (`int`, `double`, `std::string`), so
  the test can verify the pointed-to data survives the round-trip untouched.
- `Serializer.hpp/.cpp` — private OCF, two public static methods:
  - `uintptr_t serialize(Data* ptr)` — `reinterpret_cast` pointer to integer.
  - `Data* deserialize(uintptr_t raw)` — `reinterpret_cast` integer back.
- `main.cpp` — creates a `Data`, serializes its address, deserializes the
  result, and verifies the returned pointer compares equal to the original
  (and that the members read through it are intact).

Design decision: `reinterpret_cast` is the module-mandated "specific cast" for
this exercise — `static_cast` refuses pointer↔integer conversions, and a
C-style cast would hide which conversion is happening.

## Build / run / test

```sh
make            # builds build/bin/ex01
make run        # builds and runs the demo
make test       # builds and runs tests/test.cpp (exits non-zero on failure)
make fclean     # removes all artifacts
```

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`.
