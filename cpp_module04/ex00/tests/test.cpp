// ex00 test: proves virtual dispatch on Animal and its deliberate absence on
// WrongAnimal by capturing stdout (std::cout rdbuf swap). Exits non-zero on
// any failed check.
#include <iostream>
#include <sstream>
#include <string>

#include "Animal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"

static int g_fail = 0;

#define CHECK(cond)                                                       \
  do {                                                                    \
    if (!(cond)) {                                                        \
      std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << ": " << #cond \
                << std::endl;                                             \
      ++g_fail;                                                           \
    }                                                                     \
  } while (0)

int main() {
  const Animal* generic = new Animal();
  const Animal* dog = new Dog();
  const Animal* cat = new Cat();
  Cat directCat;

  // Derived classes set the protected type field.
  CHECK(generic->getType() == "Animal");
  CHECK(dog->getType() == "Dog");
  CHECK(cat->getType() == "Cat");

  // Virtual dispatch: makeSound() through Animal* runs the derived override.
  std::ostringstream out;
  std::streambuf* old = std::cout.rdbuf(out.rdbuf());
  generic->makeSound();
  std::string genericSound = out.str();
  out.str("");
  dog->makeSound();
  std::string dogSound = out.str();
  out.str("");
  cat->makeSound();
  std::string catSound = out.str();
  out.str("");
  directCat.makeSound();
  std::string directCatSound = out.str();
  out.str("");
  std::cout.rdbuf(old);

  CHECK(catSound == directCatSound);  // base ptr reached Cat::makeSound
  CHECK(dogSound != genericSound);
  CHECK(catSound != genericSound);
  CHECK(dogSound != catSound);

  // Virtual destructor: deleting via Animal* must run the Cat destructor.
  old = std::cout.rdbuf(out.rdbuf());
  delete cat;
  std::string dtorTrace = out.str();
  std::cout.rdbuf(old);
  CHECK(dtorTrace.find("Cat destructor") != std::string::npos);
  CHECK(dtorTrace.find("Animal destructor") != std::string::npos);

  // Copy semantics preserve type (Orthodox Canonical Form).
  Dog d1;
  Dog d2(d1);
  Dog d3;
  d3 = d1;
  CHECK(d2.getType() == "Dog");
  CHECK(d3.getType() == "Dog");

  // Wrong hierarchy: non-virtual makeSound() binds statically to the base.
  WrongAnimal wrongBase;
  WrongCat wrongCat;
  const WrongAnimal& wrongRef = wrongCat;
  CHECK(wrongCat.getType() == "WrongCat");

  old = std::cout.rdbuf(out.rdbuf());
  out.str("");
  wrongBase.makeSound();
  std::string wrongBaseSound = out.str();
  out.str("");
  wrongRef.makeSound();  // via base reference: base version
  std::string wrongRefSound = out.str();
  out.str("");
  wrongCat.makeSound();  // direct call: derived version
  std::string wrongCatSound = out.str();
  std::cout.rdbuf(old);

  CHECK(wrongRefSound == wrongBaseSound);
  CHECK(wrongCatSound != wrongBaseSound);

  delete generic;
  delete dog;

  if (g_fail) {
    std::cerr << g_fail << " check(s) failed" << std::endl;
    return 1;
  }
  std::cout << "all checks passed" << std::endl;
  return 0;
}
