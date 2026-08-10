#include "../../libcpp/TermWriter.hpp"
#include "../../libcpp/TermStyle.hpp"
#include "../Animal.hpp"
#include "../Cat.hpp"
#include "../Dog.hpp"
#include <iostream>

/*
** cat_dog.cpp — constructor / destructor order demo
**
** The question: when a Cat is created, who is constructed first?
**               when it is destroyed, who goes first?
**
** The LOG_CTOR() and LOG_DTOR() macros inside each class answer
** that question live — watch the output order as each scope opens
** and closes.
*/

int main(void)
{
    TermStyle ts;
    TermWriter w(ts, std::cout);

    /* ── intro ────────────────────────────────────────────── */
    w << "# Constructor & Destructor Order: Cat and Dog"
      << "Cat and Dog both inherit from Animal."
      << ">![info]"
      << "> Question: when a derived object is created, which"
      << "> constructor runs first — base or derived?"
      << "> And when it is destroyed, which destructor runs first?"
      << ">"
      << "---";
    w.flush();

    /* ── EXPERIMENT 1: stack object — Cat ────────────────── */
    w << "## Experiment 1 — Stack Cat (observe ctor order)"
      << "!i Creating:  Cat c;";
    w.flush();

    {
        Cat c;

        w << "!v Cat is alive — type: " + c.getType();
        w.flush();

        w << "!i Calling makeSound():";
        w.flush();
        c.makeSound();

        w << "---"
          << "!i Leaving scope — watch destructor order:";
        w.flush();
    }   /* <- Cat::~Cat then Animal::~Animal */

    w << "!v Scope closed."
      << "---";
    w.flush();

    /* ── EXPERIMENT 2: stack object — Dog ────────────────── */
    w << "## Experiment 2 — Stack Dog (observe ctor order)"
      << "!i Creating:  Dog d;";
    w.flush();

    {
        Dog d;

        w << "!v Dog is alive — type: " + d.getType();
        w.flush();

        w << "!i Calling makeSound():";
        w.flush();
        d.makeSound();

        w << "---"
          << "!i Leaving scope — watch destructor order:";
        w.flush();
    }   /* <- Dog::~Dog then Animal::~Animal */

    w << "!v Scope closed."
      << "---";
    w.flush();

    /* ── EXPERIMENT 3: copy constructor ──────────────────── */
    w << "## Experiment 3 — Copy constructor"
      << "!i Creating original:  Cat original;"
      << "!i Then copying:       Cat copy(original);";
    w.flush();

    {
        Cat original;
        Cat copy(original);

        w << "!v Both alive — same type: " + copy.getType();
        w.flush();

        w << "---"
          << "!i Leaving scope — two full destructions incoming:";
        w.flush();
    }

    w << "!v Scope closed — copy destroyed first (LIFO)."
      << "---";
    w.flush();

    /* ── EXPERIMENT 4: polymorphism via base pointer ──────── */
    w << "## Experiment 4 — Polymorphism via Animal*"
      << "!i Animal* a = new Dog;  Animal* b = new Cat;"
      << "!! Virtual destructor ensures full cleanup when deleted via base ptr.";
    w.flush();

    {
        Animal* a = new Dog;
        Animal* b = new Cat;

        w << "!i Calling makeSound() via Animal*:";
        w.flush();
        a->makeSound();
        b->makeSound();

        w << "---"
          << "!i Deleting via Animal* — virtual ~Animal() chains down:";
        w.flush();

        delete a;   /* Dog::~Dog → Animal::~Animal */
        delete b;   /* Cat::~Cat → Animal::~Animal */
    }

    w << "!v All heap objects deleted cleanly."
      << "---";
    w.flush();

    /* ── conclusion ───────────────────────────────────────── */
    w << "## Conclusion"
      << ">![info]"
      << "> Construction order:   Base first, then Derived"
      << "> Destruction order:    Derived first, then Base  (LIFO)"
      << "> Copy constructor:     triggers Animal copy then Cat copy"
      << "> Heap polymorphism:    virtual destructor chains both dtors"
      << ">"
      << "---";
    w.flush();

    return 0;
}