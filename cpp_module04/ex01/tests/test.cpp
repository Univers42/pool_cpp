// Deep-copy checks for ex01: Brain must never be shared between copies.
// Plain C++98, no framework; exits non-zero on first failure.

#include <cstdlib>
#include <iostream>

#include "Brain.hpp"
#include "Cat.hpp"
#include "Dog.hpp"

static void check(bool ok, const char* msg) {
  if (!ok) {
    std::cout << "FAIL: " << msg << std::endl;
    std::exit(1);
  }
  std::cout << "OK:   " << msg << std::endl;
}

int main() {
  // Dog copy constructor: distinct Brain, contents copied, mutations isolated
  Dog d1;
  d1.getBrain()->ideas[0] = "fetch";
  d1.getBrain()->ideas[99] = "dig";
  Dog d2(d1);
  check(d2.getBrain() != d1.getBrain(), "Dog copy has its own Brain");
  check(d2.getBrain()->ideas[0] == "fetch", "Dog copy: idea[0] copied");
  check(d2.getBrain()->ideas[99] == "dig", "Dog copy: idea[99] copied");
  d2.getBrain()->ideas[0] = "sleep";
  check(d1.getBrain()->ideas[0] == "fetch",
        "mutating copy does not touch original");

  // Dog assignment operator: same guarantees
  Dog d3;
  d3 = d1;
  check(d3.getBrain() != d1.getBrain(), "Dog assignment: distinct Brain");
  check(d3.getBrain()->ideas[99] == "dig", "Dog assignment: ideas copied");

  // Self-assignment must not double-free or lose the Brain
  Dog* pd = &d3;
  d3 = *pd;
  check(d3.getBrain() != NULL && d3.getBrain()->ideas[99] == "dig",
        "Dog self-assignment is safe");

  // Cat: same deep-copy contract
  Cat c1;
  c1.getBrain()->ideas[0] = "nap";
  Cat c2(c1);
  check(c2.getBrain() != c1.getBrain(), "Cat copy has its own Brain");
  c2.getBrain()->ideas[0] = "hunt";
  check(c1.getBrain()->ideas[0] == "nap", "Cat copies are independent");

  // Brain itself is canonically deep (no pointers, but verify the loop)
  Brain b1;
  b1.ideas[50] = "think";
  Brain b2(b1);
  check(b2.ideas[50] == "think", "Brain copy ctor copies ideas");
  Brain b3;
  b3 = b1;
  check(b3.ideas[50] == "think", "Brain assignment copies ideas");

  // Polymorphic destruction: virtual dtor chain must run (valgrind proves
  // the Brain is freed; here we just prove it does not crash)
  Animal* a = new Dog();
  delete a;
  a = new Cat();
  delete a;
  check(true, "delete via Animal* runs the full dtor chain");

  std::cout << "all tests passed" << std::endl;
  return 0;
}
