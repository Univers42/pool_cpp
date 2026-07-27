# ex03 — Interfaces & recap

## The concept

This exercise is the capstone of Module 04: **interfaces in C++98**. C++ has
no `interface` keyword; the idiom is a class with only pure virtual functions
and a virtual destructor (`ICharacter`, `IMateriaSource`). Client code
programs against the interface pointer and never needs to know the concrete
type — `src->createMateria("ice")` returns an `AMateria*`, and
`materia->use(target)` dispatches to `Ice::use` or `Cure::use` at runtime.

It also recaps the module's memory themes:

- **Virtual clone (prototype pattern).** You cannot copy an object through a
  base pointer with a copy constructor — you don't know its dynamic type.
  `virtual AMateria* clone() const = 0` lets each subclass copy itself;
  `MateriaSource` stores learned Materias and hands out clones of them.
- **Deep copy.** `Character` owns heap-allocated Materias, so its copy
  constructor and copy assignment must `clone()` every slot (deleting the
  old contents first on assignment) — otherwise two Characters would share
  and double-delete the same Materia.
- **Ownership rules.** `equip()` transfers ownership to the Character;
  `unequip()` gives it back to the caller *without deleting* (the Materia is
  "on the floor" and the caller must free it); the destructor deletes
  whatever is still equipped.

## Design decisions

- `AMateria::operator=` deliberately does **not** copy the type — the subject
  says copying the type "doesn't make sense" (an Ice assigned from a Cure is
  still an Ice; the type is identity, not state).
- `Character` uses a fixed `AMateria* _inventory[4]` — the subject caps the
  inventory at 4 and STL containers are forbidden in this module.
- `MateriaSource::learnMateria` takes ownership of the passed pointer (the
  caller writes `learnMateria(new Ice())` and keeps no handle). A 5th learn
  on a full source deletes the argument instead of leaking it.
- `equip` on a full inventory does nothing; the caller still owns that
  Materia. Out-of-range or empty-slot `use`/`unequip` are silent no-ops.

## Files

| File | Role |
|------|------|
| `ICharacter.hpp`, `IMateriaSource.hpp` | Pure interfaces (subject-given) |
| `AMateria.{hpp,cpp}` | Abstract Materia base: `type`, `getType`, pure `clone` |
| `Ice`, `Cure` | Concrete Materias; `use()` prints the subject strings |
| `Character.{hpp,cpp}` | 4-slot inventory, deep-copying owner of Materias |
| `MateriaSource.{hpp,cpp}` | Learns up to 4 template Materias, clones on demand |
| `main.cpp` | The subject's example main, verbatim |
| `tests/test.cpp` | Standalone assertion tests (see below) |

## Build / run / test

```sh
make            # builds build/bin/ex03
./build/bin/ex03 | cat -e
# * shoots an ice bolt at bob *$
# * heals bob's wounds *$

make test       # builds tests/test.cpp against the class objects and runs it
valgrind --leak-check=full --show-leak-kinds=all ./build/bin/ex03
valgrind --leak-check=full --show-leak-kinds=all ./build/bin/test
```

`tests/test.cpp` captures `std::cout` into a `stringstream` to assert the
exact `use()` output, and exercises: clone independence, the no-type-copy
assignment rule, MateriaSource capacity/unknown-type/deep-copy, full-inventory
equip, out-of-range `use`/`unequip`, unequip-leaves-it-alive floor behavior,
and Character deep copy surviving the original's destruction (valgrind
verifies no leak or double free in every path).
