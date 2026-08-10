# ex01 — I don't want to set the world on fire

## The concept: deep copy vs shallow copy

The compiler-generated copy constructor and copy assignment operator copy
members *memberwise*. For a class holding a raw pointer (`Brain*`), that means
the copy shares the same heap object as the original — a **shallow copy**.
Two problems follow:

1. Mutating the copy's `Brain` mutates the original's.
2. When both objects are destroyed, the same `Brain` is `delete`d twice —
   undefined behavior.

A **deep copy** allocates a fresh `Brain` and copies its contents, so each
object owns its own resource. This is the Rule of Three in action: a class
that needs a custom destructor (to `delete` its pointer) also needs a custom
copy constructor and copy assignment operator.

The exercise also reuses ex00's lesson: `Animal::~Animal()` is `virtual`, so
`delete` through an `Animal*` runs the full chain
(`Dog dtor` → `Brain` freed → `Animal dtor`). Without it, deleting a `Dog`
as an `Animal*` would skip `Dog::~Dog()` and leak the `Brain`.

## How the code demonstrates it

- **`Brain`** — plain class with `std::string ideas[100]`. Its copy ctor and
  `operator=` copy all 100 slots.
- **`Dog` / `Cat`** — each holds a private `Brain* _brain`, created with
  `new Brain()` in every constructor and `delete`d in the destructor.
  - Copy ctor: `_brain = new Brain(*src._brain)` — new allocation, copied
    contents.
  - `operator=`: self-assignment guard, then `delete` the old brain and
    deep-copy the new one. The guard is what prevents `d = d` from reading
    a freed `Brain`.
- **`main.cpp`** — the subject's verbatim `new Dog / new Cat / delete` test,
  an array of 10 `Animal*` (5 Dogs, 5 Cats) deleted through the base pointer,
  and a deep-copy demo showing the original's idea survives the copy's
  mutation and destruction.
- **`WrongAnimal` / `WrongCat`** — carried over from ex00 (files from the
  previous exercise are part of the turn-in).
- Every constructor/destructor prints a message, so the destruction order is
  visible in the output.

## Build / run / test

```sh
make            # builds build/bin/ex01
./build/bin/ex01
make test       # builds and runs tests/test.cpp (exits non-zero on failure)
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=42 ./build/bin/ex01
make fclean
```

`tests/test.cpp` asserts the deep-copy contract: distinct `Brain` pointers
after copy/assignment, copied contents, mutation isolation, safe
self-assignment, and non-crashing polymorphic destruction. Valgrind reports
zero leaks and zero errors on both binaries.


## Naming conventions..

we'll see several styles in C++ projects.

1. Keep the same name(most common)
Many projects simply write:
```cpp
class Animal{
  public:
    virtual void makeSound() const = 0;
};

```
Everyone knows it's abstract because of the `=0`

----
## 2. Prefix with `A`
```cpp
class AAnimal {};
class AShape {};
class AWeapon {};
```
Meaning:
- Abstract Animal
- Abstract Shape
- Abstract Weapon

This style is fairly common in some codebases

-----
## 3. Suffix bases
```cpp

class Animalbase {};
class ShapeBase{};
class CharacterBase{};
```

This emphasized tha the classes is intented as a base class.

----
## 4. Prefix with `I` (Interfaces)

Borrowed from language like C#
```cpp
class IAnimal {};
class ILogger {};
class IRenderer {};
```

In C++, this usually reserved fro classes taht are pure interfaces (only pure virtual functiosn and a virtual destructor)

