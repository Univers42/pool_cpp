// Plain C++98 checks for ex02: abstract base + polymorphism + deep copy.
// Exits non-zero on any failure.
#include <iostream>
#include <sstream>
#include <string>

#include "AAnimal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"

static int g_fail = 0;

#define CHECK(cond)                                        \
  do {                                                     \
    if (!(cond)) {                                         \
      std::cout << "FAIL line " << __LINE__ << ": " #cond  \
                << std::endl;                              \
      ++g_fail;                                            \
    }                                                      \
  } while (0)

// ponytail: "AAnimal a;" not compiling is a compile-time fact — provable here
// only by a commented line; a compile-fail harness is overkill for 42.
// AAnimal a;  // must NOT compile: makeSound() is pure virtual

static std::string soundOf(const AAnimal& a) {
  std::ostringstream oss;
  std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
  a.makeSound();
  std::cout.rdbuf(old);
  return oss.str();
}

static void testVirtualDispatch() {
  const AAnimal* d = new Dog();
  const AAnimal* c = new Cat();
  CHECK(d->getType() == "Dog");
  CHECK(c->getType() == "Cat");
  CHECK(soundOf(*d) == "Woof! Woof! Bark!\n");
  CHECK(soundOf(*c) == "Meow! Purrrrr...\n");
  delete d;
  delete c;
}

static void testDeepCopy() {
  Dog d1;
  d1.getBrain()->ideas[0] = "bone";
  d1.getBrain()->ideas[99] = "nap";
  Dog d2(d1);
  CHECK(d2.getBrain() != d1.getBrain());
  CHECK(d2.getBrain()->ideas[0] == "bone");
  CHECK(d2.getBrain()->ideas[99] == "nap");
  d2.getBrain()->ideas[0] = "ball";
  CHECK(d1.getBrain()->ideas[0] == "bone");

  Cat c1;
  c1.getBrain()->ideas[0] = "sunbeam";
  Cat c2;
  c2 = c1;
  CHECK(c2.getBrain() != c1.getBrain());
  CHECK(c2.getBrain()->ideas[0] == "sunbeam");
  c2.getBrain()->ideas[0] = "wall";
  CHECK(c1.getBrain()->ideas[0] == "sunbeam");
}

static void testSelfAssign() {
  Dog d;
  d.getBrain()->ideas[0] = "guard";
  Dog* p = &d;
  d = *p;
  CHECK(d.getBrain() != 0);
  CHECK(d.getBrain()->ideas[0] == "guard");
}

int main() {
  testVirtualDispatch();
  testDeepCopy();
  testSelfAssign();
  if (g_fail) {
    std::cout << g_fail << " check(s) FAILED" << std::endl;
    return 1;
  }
  std::cout << "all checks passed" << std::endl;
  return 0;
}
