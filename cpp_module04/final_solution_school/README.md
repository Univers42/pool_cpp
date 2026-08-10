# Module 04 — Subtype polymorphism, abstract classes, interfaces

This module is one hierarchy, refined four times. In ex00 I make a base-class
call dispatch to the derived type and build a control group that deliberately
doesn't. In ex01 the derived classes grow a heap resource and I learn what
copying really costs. In ex02 the base class is forbidden from existing at
all. In ex03 the same ideas are stripped down to pure interfaces and an
ownership protocol — and that is where this module stopped being theory and
started drawing blood. Module 04 is the one where valgrind taught me more
than the subject PDF did, so this README is partly a confession.

## ex00 — Polymorphism

The exercise wants proof that I understand what `virtual` buys, so it demands
two parallel hierarchies: one that works and one that is wrong on purpose.
`Animal` declares `virtual void makeSound() const` and a `virtual` destructor;
`Dog` prints `Woof! Woof!` and `Cat` prints `Meow!` through an `Animal*`.
`WrongAnimal` is the control group — its `makeSound()` has no `virtual`, and
the header says so out loud: "Deliberately flawed base". `WrongCat` does
override the method (`Wrong meow?!`), but through a `WrongAnimal*` you get
`* wrong generic noise *` every time, because the call was resolved at compile
time from the pointer's static type. `main.cpp` runs the subject's example,
then the Wrong hierarchy side by side, so the failure is visible, not
hypothetical.

What took me longest here was accepting that `WrongCat` is not broken code to
fix — it is an exhibit. The quieter half of the lesson is the destructor:
`delete j;` through an `Animal*` runs `Dog::~Dog()` first only because
`~Animal()` is virtual. Every constructor and destructor prints its own line
precisely so those chains can be read in the output. That is a conscious
trade-off — the program is noisy — but it made the tests honest: `test.cpp`
captures `std::cout` with an `rdbuf` swap and asserts the chains byte by byte.

## ex01 — Deep copy vs shallow copy

Now `Dog` and `Cat` each own a `Brain*` — a class whose only job is holding
`std::string ideas[100]` — and the compiler-generated copy becomes a trap:
memberwise copy shares the pointer, mutation bleeds through, destruction
frees the same `Brain` twice. So both classes implement the Rule of Three:
the copy constructor does `new Brain(*src._brain)`, the destructor deletes,
and `operator=` guards against self-assignment.

The subtle part is the *order* inside `operator=`. My first instinct was
delete-then-copy. But if `new Brain(...)` throws after the delete, the object
is left holding a freed pointer it will delete again in its destructor. The
shipped version in `Dog.cpp` does it the other way — `Brain* fresh = new
Brain(*rhs._brain); delete this->_brain; this->_brain = fresh;` — with a
comment I ended up copying across the whole module: "clone first: if new
throws, _brain must not be left dangling." The demo builds `Animal*
animals[10]`, five Dogs and five Cats, and deletes them all through the base
pointer; the deep-copy demo shows an original's idea surviving its copy's
mutation and destruction. Copying all 100 idea slots wholesale is not
efficient, and I kept it anyway: simple and provably correct beat clever here.

## ex02 — Abstract class

One line changes everything: `virtual void makeSound() const = 0;` in
`AAnimal.hpp`. The base can no longer be instantiated, which is the point —
in ex00, `new Animal()` compiled and produced a meaningless generic animal
(my own ex00 main does exactly that with `meta`). The embarrassing part of
this exercise's history is that an earlier version kept the old concrete
`Animal` class sitting next to `AAnimal`, which quietly defeated the whole
exercise: the forbidden `new Animal()` path was still alive, just under its
old name. Deleting that class was the actual fix; renaming was cosmetic.

Non-instantiability is a compile-time property, so `main.cpp` and
`tests/test.cpp` prove it with commented-out `AAnimal a;` lines rather than a
compile-fail harness — a trade-off I accepted because a second build target
just to watch a compile error felt like ceremony. Everything from before
still holds and is re-tested: dispatch through `AAnimal*`, the `Brain` deep
copy (with the same clone-first `operator=` in `Cat.cpp` and `Dog.cpp`), the
self-assignment guard.

## ex03 — Interfaces and the bugs that earned their comments

The capstone: `ICharacter` and `IMateriaSource` are pure interfaces,
`AMateria` adds `virtual clone()` so objects can copy themselves through a
base pointer, `Ice` and `Cure` print the subject's exact strings
(`* shoots an ice bolt at bob *`, `* heals bob's wounds *`), and `Character`
manages a four-slot `AMateria* _inventory[4]` under explicit ownership rules.

This is where I got burned, twice. The bug that taught me the most was
embarrassing in hindsight: my `equip()` loop ran to five on a four-slot
inventory. Nothing crashed — it quietly wrote past the array until valgrind
pointed at it. Bounds are contracts, not suggestions; every loop in
`Character.cpp` now says `i < 4` and I read them differently than I used to.
The second bug needed an adversarial mindset to find: equip the *same*
`AMateria*` twice and it landed in two slots, so the destructor deleted it
twice. The fix is a pointer-identity guard at the top of `equip()` — scan the
inventory and refuse if `_inventory[i] == m` — with a comment explaining the
double-delete it prevents. The test suite now does exactly that attack
(`dup.equip(once); dup.equip(once);`) on purpose.

Those two bugs changed how the whole exercise is written. Copying a
`Character` clones slot by slot, and a throwing `clone()` must not leak the
clones already made — so the copy constructor wraps its loop in `try/catch`
and unwinds, and `operator=` clones all four into a local `fresh[4]` *before*
deleting anything, so a throw leaves the old inventory untouched.
`MateriaSource` follows the identical pattern. Two more ownership rules came
straight from reading the subject slowly: `unequip()` must set the slot to
`NULL` and *not* delete — the Materia is on the floor and the caller must
save the pointer or leak it (the code comment shouts "DO NOT DELETE as per
subject!") — and `learnMateria()` literally "copies the Materia passed as a
parameter", so it stores `m->clone()` and then deletes `m`, which is exactly
what keeps the subject's own `learnMateria(new Ice())` main leak-free. One
last subtlety: `AMateria::operator=` deliberately copies nothing — the
subject says copying the type "doesn't make sense", because type is identity,
not state.

## What stuck with me

Virtual dispatch and virtual destructors are the visible lessons, but the
ones I actually carry are quieter. Bounds are contracts. Ownership is a
protocol you write down, not a vibe — equip transfers it, unequip hands it
back, the floor is the caller's problem. Clone before you delete, always,
because `new` can throw and dangling pointers don't announce themselves.
And the happy-path tests I wrote first found none of this; the hostile ones —
equip the same pointer twice, unequip into the void, copy and then destroy
the original — found all of it. Valgrind is not a formality, it is the
grader that never gets tired.

## Building and testing

Each exercise is self-contained under `c++ -std=c++98 -Wall -Wextra -Werror`:

```sh
cd exNN
make        # build build/bin/exNN
make run    # build and run the demo
make test   # build and run tests/test.cpp — exits non-zero on failure
make fclean
```

`make test` really runs the suites (output captured via `rdbuf`, assertions
on exact strings and pointer identity), and every binary in this module is
valgrind-clean — I checked, because this module is why I check.


Yes. Module 04 is much less about animals or magic spells than it is about **object-oriented programming concepts**. If you truly understand these concepts, you've understood the module.

Here's the progression.

---

# Ex00 — Polymorphism

**Goal:** Understand runtime polymorphism.

You should know:

* ✅ Virtual functions
* ✅ Method overriding
* ✅ Dynamic dispatch
* ✅ Base pointer → derived object
* ✅ Base reference → derived object
* ✅ Object slicing
* ✅ Why `WrongAnimal` fails

Example:

```cpp
Animal* a = new Dog();
a->makeSound();
```

Question:

> Why does this call `Dog::makeSound()`?

---

# Ex01 — Rule of Three / Deep Copy

**Goal:** Learn resource management.

You should know:

* ✅ Heap allocation (`new`)
* ✅ `delete`
* ✅ Deep copy
* ✅ Shallow copy
* ✅ Copy constructor
* ✅ Copy assignment operator
* ✅ Destructor
* ✅ Self-assignment

Question:

> Why can't two Dogs share the same Brain?

---

# Ex02 — Abstract Classes

**Goal:** Prevent invalid objects.

You should know:

```cpp
virtual AMateria* clone() const = 0;
```

means

> "Every derived class must implement this."

You should understand:

* ✅ Pure virtual functions
* ✅ Abstract classes
* ✅ Why `Animal`/`AMateria` shouldn't exist by themselves

Question:

> Why can't we instantiate `AMateria`?

---

# Ex03 — Interfaces & Design Patterns

This is the biggest exercise.

You should understand:

## Interfaces

```cpp
class ICharacter
```

contains only behavior.

It says

> "Every Character must know how to equip, unequip and use."

Not **how**, only **what**.

---

## Polymorphism through interfaces

```cpp
ICharacter& target
```

instead of

```cpp
Character&
```

Why?

Because tomorrow you could have

```text
Character
Enemy
NPC
Boss
```

All implementing

```cpp
ICharacter
```

without changing your code.

---

## Factory / Prototype Pattern

This is what `MateriaSource` teaches.

Instead of

```cpp
new Ice()
```

everywhere,

you do

```cpp
source.createMateria("ice");
```

The source manufactures the object.

---

## clone()

The hardest concept.

```cpp
prototype->clone();
```

means

> "Duplicate yourself."

It is used because

```cpp
AMateria*
```

doesn't know whether it's pointing to

* Ice
* Cure
* Fire
* etc.

Only virtual dispatch can figure that out.

---

## Dynamic dispatch everywhere

These are all equivalent ideas:

```cpp
Animal* a = new Dog();
a->makeSound();
```

```cpp
AMateria* m = new Ice();
m->use(target);
```

```cpp
ICharacter& c = alice;
c.use(0, bob);
```

Same mechanism.

Different classes.

---

# Big picture

```text
Module 04

                 OOP
                  │
      ┌───────────┴───────────┐
      │                       │
 Polymorphism           Resource Management
      │                       │
 virtual               Rule of Three
      │
      ▼
Dynamic Dispatch
      │
      ▼
Abstract Classes
      │
      ▼
Interfaces
      │
      ▼
Prototype Pattern (clone)
      │
      ▼
Factory (MateriaSource)
```

---

## If you can answer these questions without hesitation, you've mastered Module 04:

1. Why must a base class destructor be `virtual`?
2. Why does `Animal a = Dog();` call `Animal::makeSound()`?
3. Why does `Animal* a = new Dog();` call `Dog::makeSound()`?
4. What is object slicing?
5. Why is a shallow copy dangerous when a class owns heap memory?
6. What are the Rule of Three (or Rule of Five in modern C++)?
7. Why is `AMateria` abstract?
8. Why is `ICharacter` an interface instead of using `Character` directly?
9. Why does `MateriaSource` call `clone()` instead of constructing an `Ice` or `Cure` directly?
10. Why is `clone()` virtual?

If you can explain all ten in your own words, you're in excellent shape for Module 04.


## [QUESTION] : The destructors in Animall must be virtual ask an explanation of what will happen without the virtual keywork over Animal destructor.

The detructor of `Animal` must be `virtual`because we want ot be able to delete a derived object (`Dog`, `Cat`, etc.) through an `Animal*`
When the destructor is virtual, C++ uses the virtual table (vtable) to determine the real type of teh object at runtime, it calls the most-derived destructor first `Dog::~Dog()`, the automatically continues with `Animal::~Animal()`

This ensures that every part of the object is destroyed correctly