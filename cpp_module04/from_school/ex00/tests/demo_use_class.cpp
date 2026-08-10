#include "../../libcpp/TermWriter.hpp"
#include "../../libcpp/TermStyle.hpp"
#include "../Animal.hpp"
#include "../Cat.hpp"
#include "../Dog.hpp"
#include <iostream>

int main() {
  TermStyle ts;
  TermWriter w(ts, std::cout);

  w << "# Demo: Auto-Logging Macros in Action"
    << "This demonstrates the auto-logging system that dynamically extracts class names."
    << "---";
  w.flush();

  w << "## Section 1: Creating Instances"
    << ">![info] Creating an Animal, Dog, and Cat with auto-detected logging";
  {
    Animal a;
    Dog d;
    Cat c;
    w.flush();

    w << "## Section 2: Type Information "
      << "- Animal type: " + a.getType()
      << "- Dog type: " + d.getType()
      << "- Cat type: " + c.getType();
    w.flush();

    w << "## Section 3: Methods"
      << "Calling makeSound() on each:";
    a.makeSound();
    d.makeSound();
    c.makeSound();
    w.flush();
  }

  w << "---"
    << ">![success] Destructors automatically logged as objects go out of scope";
  w.flush();

  return 0;
}
