/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:49:24 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/06 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <string>

#include "AAnimal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include "postman.hpp"

#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

static void printBanner(const std::string& title) {
  TestReport::instance().beginSuite(title);
  std::cout << "\n\n\n"
            << CYAN << BOLD << std::string(70, '=') << RESET << "\n"
            << CYAN << BOLD << "  >>> " << RESET
            << BOLD << WHITE << title << RESET << "\n"
            << CYAN << BOLD << std::string(70, '=') << RESET << "\n"
            << std::endl;
}

static void printSep() {
  std::cout << "\n" << DIM << std::string(70, '-') << RESET << "\n\n";
}

static void test_abstractNotInstantiable() {
  printBanner("TEST 1 - AAnimal is abstract (cannot instantiate)");
  // AAnimal a;  // <-- must produce a compile error
  std::cout << DIM
    << "  AAnimal has pure virtual makeSound() = 0.\n"
    << "  Uncommenting 'AAnimal a;' would cause a compile error.\n"
    << "  We verify Dog & Cat ARE instantiable instead.\n"
    << RESET;
  Dog d;
  Cat c;
  ASSERT_R(d.getType() == "Dog", "Dog is instantiable (overrides makeSound)");
  ASSERT_R(c.getType() == "Cat", "Cat is instantiable (overrides makeSound)");
  AAnimal* pa = &d;
  ASSERT_R(pa->getType() == "Dog", "AAnimal* pointing to Dog works");
  pa = &c;
  ASSERT_R(pa->getType() == "Cat", "AAnimal* pointing to Cat works");
}

static void test_subjectBaseline() {
  printBanner("TEST 2 - Subject baseline (AAnimal* heap pointers)");
  const AAnimal* j = new Dog();
  const AAnimal* i = new Cat();
  std::cout << "  Dog type: " << j->getType() << "\n";
  std::cout << "  Cat type: " << i->getType() << "\n";
  std::cout << "  Dog sound: "; j->makeSound();
  std::cout << "  Cat sound: "; i->makeSound();
  ASSERT_R(j->getType() == "Dog", "Dog::getType() returns Dog");
  ASSERT_R(i->getType() == "Cat", "Cat::getType() returns Cat");
  printSep();
  std::cout << DIM << "  Deleting via AAnimal* (virtual dtor)...\n" << RESET;
  delete j;
  delete i;
}

static void test_brainLifecycle() {
  printBanner("TEST 3 - Brain lifecycle (new/delete in ctor/dtor)");
  Dog d;
  Cat c;
  ASSERT_R(d.getBrain() != NULL, "Dog::getBrain() non-NULL after construction");
  ASSERT_R(c.getBrain() != NULL, "Cat::getBrain() non-NULL after construction");
}

static void test_deepCopyDog() {
  printBanner("TEST 4 - Deep copy Dog (Brain NOT shared)");
  Dog d1;
  d1.getBrain()->ideas[0] = "chase squirrel";
  d1.getBrain()->ideas[1] = "bark at mailman";
  Dog d2(d1);
  ASSERT_R(d2.getBrain() != d1.getBrain(), "Dog copy: different Brain pointer");
  ASSERT_R(d2.getBrain()->ideas[0] == "chase squirrel", "Dog copy Brain idea[0] matches");
  ASSERT_R(d2.getBrain()->ideas[1] == "bark at mailman", "Dog copy Brain idea[1] matches");
  d2.getBrain()->ideas[0] = "fetch ball";
  ASSERT_R(d1.getBrain()->ideas[0] == "chase squirrel", "Original Brain[0] unchanged after copy mutation");
  ASSERT_R(d2.getBrain()->ideas[0] == "fetch ball", "Copy Brain[0] holds new value");
  printSep();
  Dog d3;
  d3 = d1;
  ASSERT_R(d3.getBrain() != d1.getBrain(), "Dog assigned: different Brain pointer");
  ASSERT_R(d3.getBrain()->ideas[1] == "bark at mailman", "Assigned Brain idea[1] matches original");
  d3.getBrain()->ideas[1] = "howl at moon";
  ASSERT_R(d1.getBrain()->ideas[1] == "bark at mailman", "Original Brain[1] unchanged after assign mutation");
}

static void test_deepCopyCat() {
  printBanner("TEST 5 - Deep copy Cat (Brain NOT shared)");
  Cat c1;
  c1.getBrain()->ideas[0]  = "nap in sunbeam";
  c1.getBrain()->ideas[99] = "knock glass off table";
  Cat c2(c1);
  ASSERT_R(c2.getBrain() != c1.getBrain(), "Cat copy: different Brain pointer");
  ASSERT_R(c2.getBrain()->ideas[0] == "nap in sunbeam", "Cat copy Brain idea[0] matches");
  ASSERT_R(c2.getBrain()->ideas[99] == "knock glass off table", "Cat copy Brain idea[99] (last slot) matches");
  c2.getBrain()->ideas[0] = "scratch furniture";
  ASSERT_R(c1.getBrain()->ideas[0] == "nap in sunbeam", "Original Brain[0] unchanged after copy mutation");
  printSep();
  Cat c3;
  c3 = c1;
  ASSERT_R(c3.getBrain() != c1.getBrain(), "Cat assigned: different Brain pointer");
  c3.getBrain()->ideas[99] = "stare at wall";
  ASSERT_R(c1.getBrain()->ideas[99] == "knock glass off table", "Original Brain[99] unchanged after assign mutation");
}

static void test_brainCanonical() {
  printBanner("TEST 6 - Brain canonical form (direct copy/assign)");
  Brain b1;
  b1.ideas[0]  = "alpha";
  b1.ideas[50] = "omega";
  Brain b2(b1);
  ASSERT_R(&b2 != &b1, "b2 is a different Brain object");
  ASSERT_R(b2.ideas[0]  == "alpha", "Brain copy idea[0] matches");
  ASSERT_R(b2.ideas[50] == "omega", "Brain copy idea[50] matches");
  b2.ideas[0] = "beta";
  ASSERT_R(b1.ideas[0] == "alpha", "Original Brain[0] intact after copy mutation");
  Brain b3;
  b3 = b1;
  ASSERT_R(&b3 != &b1, "b3 is a different Brain object");
  ASSERT_R(b3.ideas[50] == "omega", "Assigned Brain idea[50] matches");
  Brain* pb3 = &b3;
  b3 = *pb3;
  ASSERT_R(b3.ideas[50] == "omega", "Brain[50] intact after self-assignment");
}

static void test_animalArray() {
  printBanner("TEST 7 - Array 10 AAnimal* (5 Dogs + 5 Cats), delete all");
  const int SIZE = 10;
  AAnimal* zoo[SIZE];
  std::cout << DIM << "  Filling: even=Dog, odd=Cat\n" << RESET;
  for (int i = 0; i < SIZE; ++i)
    zoo[i] = (i % 2 == 0) ? static_cast<AAnimal*>(new Dog())
                           : static_cast<AAnimal*>(new Cat());
  printSep();
  for (int i = 0; i < SIZE; ++i) {
    if (i % 2 == 0)
      ASSERT_R(zoo[i]->getType() == "Dog", "Even slot is a Dog");
    else
      ASSERT_R(zoo[i]->getType() == "Cat", "Odd slot is a Cat");
  }
  printSep();
  std::cout << DIM << "  Deleting all via AAnimal*...\n" << RESET;
  for (int i = 0; i < SIZE; ++i)
    delete zoo[i];
}

static void test_selfAssignment() {
  printBanner("TEST 8 - Self-assignment guard (no double-free)");
  Dog d;
  d.getBrain()->ideas[0] = "guard bone";
  Dog* pd = &d;
  d = *pd;
  ASSERT_R(d.getType() == "Dog", "Dog type intact after self-assign");
  ASSERT_R(d.getBrain() != NULL, "Dog Brain pointer still valid");
  ASSERT_R(d.getBrain()->ideas[0] == "guard bone", "Dog Brain[0] intact after self-assign");
  Cat c;
  c.getBrain()->ideas[0] = "ignore human";
  Cat* pc = &c;
  c = *pc;
  ASSERT_R(c.getType() == "Cat", "Cat type intact after self-assign");
  ASSERT_R(c.getBrain() != NULL, "Cat Brain pointer still valid");
  ASSERT_R(c.getBrain()->ideas[0] == "ignore human", "Cat Brain[0] intact after self-assign");
}

static void test_brain100Slots() {
  printBanner("TEST 9 - Brain 100 slots + deep copy of full Brain");
  Dog d;
  for (int i = 0; i < 100; ++i) {
    std::ostringstream oss;
    oss << "idea_" << i;
    d.getBrain()->ideas[i] = oss.str();
  }
  ASSERT_R(d.getBrain()->ideas[0]  == "idea_0", "Brain idea[0] (first slot) writable");
  ASSERT_R(d.getBrain()->ideas[99] == "idea_99", "Brain idea[99] (last slot) writable");
  Dog d2(d);
  ASSERT_R(d2.getBrain() != d.getBrain(), "Deep copy: distinct Brain ptr");
  ASSERT_R(d2.getBrain()->ideas[0]  == "idea_0", "Copy Brain idea[0] matches");
  ASSERT_R(d2.getBrain()->ideas[99] == "idea_99", "Copy Brain idea[99] matches");
}

static void test_constCorrectness() {
  printBanner("TEST 10 - const-correctness (pointers & references)");
  const Dog cd;
  const Cat cc;
  ASSERT_R(cd.getType() == "Dog", "const Dog getType()");
  ASSERT_R(cc.getType() == "Cat", "const Cat getType()");
  const AAnimal& refDog = cd;
  const AAnimal& refCat = cc;
  ASSERT_R(refDog.getType() == "Dog", "Dog via const AAnimal& getType()");
  ASSERT_R(refCat.getType() == "Cat", "Cat via const AAnimal& getType()");
  ASSERT_R(cd.getBrain() != NULL, "const Dog getBrain() non-null");
  ASSERT_R(cc.getBrain() != NULL, "const Cat getBrain() non-null");
}

static void test_virtualDtorChain() {
  printBanner("TEST 11 - Virtual dtor chain (AAnimal/Dog/Cat/Brain all freed)");
  AAnimal* d = new Dog();
  AAnimal* c = new Cat();
  bool noLeak = (d != NULL && c != NULL);
  ASSERT_R(noLeak, "Virtual dtor chain executed without crash");
  std::cout << DIM << "\n  delete Dog:\n" << RESET;
  delete d;
  std::cout << DIM << "\n  delete Cat:\n" << RESET;
  delete c;
}

static void test_polymorphicSound() {
  printBanner("TEST 12 - Polymorphic makeSound via AAnimal*");
  Dog d;
  Cat c;
  AAnimal* animals[2];
  animals[0] = &d;
  animals[1] = &c;
  std::cout << BOLD << "  Sound via AAnimal*:\n" << RESET;
  std::cout << "  animals[0] (" << animals[0]->getType() << "): ";
  animals[0]->makeSound();
  std::cout << "  animals[1] (" << animals[1]->getType() << "): ";
  animals[1]->makeSound();
  ASSERT_R(animals[0]->getType() == "Dog", "AAnimal*[0] dispatches to Dog");
  ASSERT_R(animals[1]->getType() == "Cat", "AAnimal*[1] dispatches to Cat");
}

int main() {
  test_abstractNotInstantiable();
  test_subjectBaseline();
  test_brainLifecycle();
  test_deepCopyDog();
  test_deepCopyCat();
  test_brainCanonical();
  test_animalArray();
  test_selfAssignment();
  test_brain100Slots();
  test_constCorrectness();
  test_virtualDtorChain();
  test_polymorphicSound();
  TestReport::instance().print();
  return 0;
}
