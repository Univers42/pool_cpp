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
