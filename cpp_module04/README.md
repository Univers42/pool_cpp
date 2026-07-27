# C++ Module 04 — Subtype polymorphism, abstract classes, interfaces

The arc of this module is one hierarchy refined step by step:

1. **ex00** — make a base-class call dispatch to the derived type (`virtual`),
   and show what happens without it.
2. **ex01** — give derived classes a heap resource (`Brain*`) and get copying
   right (Rule of Three, deep copy, virtual destructor so nothing leaks).
3. **ex02** — forbid instantiating the base by making it abstract
   (pure virtual `makeSound()`).
4. **ex03** — take the idea to its limit: pure interfaces, virtual `clone()`,
   and explicit ownership rules in a small Materia/Character system.

By the end you can design a polymorphic hierarchy that is safe to copy,
safe to delete through a base pointer, and usable purely through interfaces.

## ex00 — Polymorphism

Virtual dispatch. Two parallel hierarchies differ by one keyword:
`Animal::makeSound()` is `virtual` (so `Animal*` on a `Cat` prints `Meow!`),
`WrongAnimal::makeSound()` is not (so a `WrongCat` through a `WrongAnimal&`
prints the base sound). `Animal`'s destructor is `virtual`, so `delete` via
`Animal*` runs the derived destructor first — every ctor/dtor prints a
message so the chains are visible. `main.cpp` runs the subject example, then
the Wrong hierarchy side by side. The tests capture `std::cout` and assert
derived vs base sounds, destructor chaining, and that copies preserve `type`.

## ex01 — Deep copy vs shallow copy

`Dog` and `Cat` each own a `Brain*` (an array of 100 idea strings). The
compiler-generated copy would share that pointer — mutation bleeds through
and destruction double-frees. So both classes implement the Rule of Three:
copy ctor does `new Brain(*src._brain)`, `operator=` guards against
self-assignment, deletes the old brain, and deep-copies. The demo builds an
array of 10 `Animal*` deleted through the base pointer and shows an
original's idea surviving its copy's mutation and destruction. Tests assert
distinct `Brain` pointers, mutation isolation, and safe self-assignment;
valgrind is clean.

## ex02 — Abstract class

`Animal` becomes `AAnimal` with `makeSound() const = 0`: the base can no
longer be instantiated (`AAnimal a;` fails to compile — shown as commented
lines, since non-instantiability is a compile-time property). Everything
from ex00/ex01 still holds: virtual destructor chain, `Brain` deep copy in
the concrete `Dog`/`Cat`. Tests re-verify dispatch through `AAnimal*`, deep
copy, and the self-assignment guard.

## ex03 — Interfaces & recap

The C++98 interface idiom: classes with only pure virtual functions
(`ICharacter`, `IMateriaSource`). `AMateria` adds the prototype pattern —
`virtual clone()` lets you copy an object through a base pointer without
knowing its dynamic type. `Ice` and `Cure` are concrete Materias;
`MateriaSource` learns up to 4 templates and hands out clones;
`Character` deep-copies its 4-slot inventory via `clone()` and follows
explicit ownership rules (`equip` transfers ownership, `unequip` leaves the
Materia alive for the caller, the dtor frees what remains). One subtlety:
`AMateria::operator=` does not copy the type — type is identity, not state.
The demo is the subject's main (`* shoots an ice bolt at bob *`, etc.);
tests cover clone independence, capacity/edge cases, floor behavior, and
deep copy surviving the original's destruction, all valgrind-clean.

## Commands

Each exercise is self-contained:

```sh
cd exNN
make        # build build/bin/exNN
make run    # build and run the demo
make test   # build and run tests/test.cpp (exits non-zero on failure)
make fclean
```

See each exercise's own `README.md` for details and design decisions.
