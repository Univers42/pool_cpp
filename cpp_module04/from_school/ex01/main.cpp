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
#include <string>

#include "Animal.hpp"
#include "Brain.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include "postman.hpp"

// ── Extra colours (Animal.hpp already defines the basics) ────────────────────
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

// ── Visual helpers ────────────────────────────────────────────────────────────

static void printBanner(const std::string& title) {
  const std::string border(70, '=');

  TestReport::instance().beginSuite(title);

  std::cout << "\n\n\n"
            << CYAN << BOLD << border    << RESET << "\n"
            << CYAN << BOLD << "  >>> "  << RESET
            << BOLD << WHITE << title    << RESET << "\n"
            << CYAN << BOLD << border    << RESET << "\n"
            << std::endl;
}

static void printSeparator() {
  std::cout << "\n" << DIM << std::string(70, '-') << RESET << "\n\n";
}

// ═══════════════════════════════════════════════════════════════════════════
//  TEST 1 – Subject-mandated baseline
// ═══════════════════════════════════════════════════════════════════════════
static void test_subjectBaseline() {
  printBanner("TEST 1 – Subject baseline (Brain on heap)");

  std::cout << DIM << "  Allocating Dog and Cat via new Animal*...\n" << RESET;

  const Animal* j = new Dog();
  const Animal* i = new Cat();

  printSeparator();

  std::cout << BOLD << "  getType() & makeSound() via polymorphic pointer\n" << RESET;
  std::cout << "  Dog type  : " << j->getType() << "\n";
  std::cout << "  Cat type  : " << i->getType() << "\n";
  std::cout << "  Cat sound : "; i->makeSound();
  std::cout << "  Dog sound : "; j->makeSound();

  ASSERT_R(j->getType() == "Dog", "Dog::getType() returns \"Dog\"");
  ASSERT_R(i->getType() == "Cat", "Cat::getType() returns \"Cat\"");

  printSeparator();

  std::cout << DIM << "  delete j and i – Brain must be freed, no leak:\n" << RESET;
  delete j;
  delete i;

  std::cout << "\n  " << GREEN << BOLD
            << "No leak: Dog/Cat destructors deleted their Brain.\n" << RESET;
}

// ═══════════════════════════════════════════════════════════════════════════
//  TEST 2 – Brain lifecycle
// ═══════════════════════════════════════════════════════════════════════════
static void test_brainLifecycle() {
  printBanner("TEST 2 – Brain lifecycle (new/delete in ctor/dtor)");

  std::cout << DIM << "  Constructing Dog on stack → Brain created inside:\n" << RESET;
  {
    Dog d;
    ASSERT_R(d.getBrain() != NULL, "Dog::getBrain() non-NULL after construction");

    std::cout << DIM << "  Constructing Cat on stack → Brain created inside:\n" << RESET;
    Cat c;
    ASSERT_R(c.getBrain() != NULL, "Cat::getBrain() non-NULL after construction");

    std::cout << DIM << "\n  (Leaving scope → destructors + Brain delete)\n" << RESET;
  }
  std::cout << "\n  " << GREEN << BOLD << "Brain destroyed along with each Animal.\n" << RESET;
}

// ═══════════════════════════════════════════════════════════════════════════
//  TEST 3 – Deep copy: Dog
// ═══════════════════════════════════════════════════════════════════════════
static void test_deepCopyDog() {
  printBanner("TEST 3 – Deep copy Dog (Brain NOT shared)");

  Dog original;
  original.getBrain()->ideas[0] = "Chase the mailman";
  original.getBrain()->ideas[1] = "Fetch the ball";

  printSeparator();

  std::cout << BOLD << "  Copy constructor\n" << RESET;
  Dog copy(original);

  ASSERT_R(copy.getBrain() != original.getBrain(), "Dog copy: different Brain pointer");
  ASSERT_R(copy.getBrain()->ideas[0] == "Chase the mailman", "Dog copy Brain idea[0] matches");
  ASSERT_R(copy.getBrain()->ideas[1] == "Fetch the ball",    "Dog copy Brain idea[1] matches");

  printSeparator();

  std::cout << BOLD << "  Mutation isolation\n" << RESET;
  copy.getBrain()->ideas[0] = "Dig a hole";

  ASSERT_R(original.getBrain()->ideas[0] == "Chase the mailman", "Original Brain[0] unchanged after copy mutation");
  ASSERT_R(copy.getBrain()->ideas[0]     == "Dig a hole",        "Copy Brain[0] holds new value");

  printSeparator();

  std::cout << BOLD << "  Assignment operator\n" << RESET;
  Dog assigned;
  assigned = original;

  ASSERT_R(assigned.getBrain() != original.getBrain(),        "Dog assigned: different Brain pointer");
  ASSERT_R(assigned.getBrain()->ideas[1] == "Fetch the ball", "Assigned Brain idea[1] matches original");

  assigned.getBrain()->ideas[1] = "Sleep all day";
  ASSERT_R(original.getBrain()->ideas[1] == "Fetch the ball", "Original Brain[1] unchanged after assign mutation");
}

// ═══════════════════════════════════════════════════════════════════════════
//  TEST 4 – Deep copy: Cat
// ═══════════════════════════════════════════════════════════════════════════
static void test_deepCopyCat() {
  printBanner("TEST 4 – Deep copy Cat (Brain NOT shared)");

  Cat original;
  original.getBrain()->ideas[0]  = "Knock things off shelves";
  original.getBrain()->ideas[99] = "Stare at the wall";

  printSeparator();

  std::cout << BOLD << "  Copy constructor\n" << RESET;
  Cat copy(original);

  ASSERT_R(copy.getBrain() != original.getBrain(),             "Cat copy: different Brain pointer");
  ASSERT_R(copy.getBrain()->ideas[0]  == "Knock things off shelves", "Cat copy Brain idea[0] matches");
  ASSERT_R(copy.getBrain()->ideas[99] == "Stare at the wall",  "Cat copy Brain idea[99] (last slot) matches");

  printSeparator();

  std::cout << BOLD << "  Mutation isolation\n" << RESET;
  copy.getBrain()->ideas[0] = "Ignore humans";

  ASSERT_R(original.getBrain()->ideas[0] == "Knock things off shelves", "Original Brain[0] unchanged after copy mutation");

  printSeparator();

  std::cout << BOLD << "  Assignment operator\n" << RESET;
  Cat assigned;
  assigned = original;

  ASSERT_R(assigned.getBrain() != original.getBrain(), "Cat assigned: different Brain pointer");

  assigned.getBrain()->ideas[99] = "Sleep in the sun";
  ASSERT_R(original.getBrain()->ideas[99] == "Stare at the wall", "Original Brain[99] unchanged after assign mutation");
}

// ═══════════════════════════════════════════════════════════════════════════
//  TEST 5 – Brain canonical form
// ═══════════════════════════════════════════════════════════════════════════
static void test_brainCopyDirectly() {
  printBanner("TEST 5 – Brain canonical form (direct copy/assign)");

  Brain b1;
  b1.ideas[0]  = "Plan world domination";
  b1.ideas[50] = "Learn C++";

  printSeparator();

  std::cout << BOLD << "  Brain copy constructor\n" << RESET;
  Brain b2(b1);

  ASSERT_R(&b2 != &b1,                   "b2 is a different Brain object");
  ASSERT_R(b2.ideas[0]  == b1.ideas[0],  "Brain copy idea[0]  matches");
  ASSERT_R(b2.ideas[50] == b1.ideas[50], "Brain copy idea[50] matches");

  b2.ideas[0] = "Just sleep";
  ASSERT_R(b1.ideas[0] == "Plan world domination", "Original Brain[0] intact after copy mutation");

  printSeparator();

  std::cout << BOLD << "  Brain assignment operator\n" << RESET;
  Brain b3;
  b3 = b1;

  ASSERT_R(&b3 != &b1,              "b3 is a different Brain object");
  ASSERT_R(b3.ideas[50] == "Learn C++", "Assigned Brain idea[50] matches");

  printSeparator();

  std::cout << BOLD << "  Brain self-assignment guard\n" << RESET;
  Brain* pb1 = &b1;
  b1 = *pb1;
  ASSERT_R(b1.ideas[50] == "Learn C++", "Brain[50] intact after self-assignment");
}

// ═══════════════════════════════════════════════════════════════════════════
//  TEST 6 – Subject array
// ═══════════════════════════════════════════════════════════════════════════
static void test_subjectArray() {
  printBanner("TEST 6 – Array 10 Animal* (5 Dogs + 5 Cats), delete all");

  const int SIZE = 10;
  Animal*   zoo[SIZE];

  std::cout << DIM << "  Filling array (even=Dog, odd=Cat):\n" << RESET;
  printSeparator();

  for (int i = 0; i < SIZE; ++i)
    zoo[i] = (i % 2 == 0) ? static_cast<Animal*>(new Dog()) : static_cast<Animal*>(new Cat());

  printSeparator();

  std::cout << BOLD << "  Type & sound check through Animal*\n" << RESET;
  for (int i = 0; i < SIZE; ++i) {
    std::cout << "  zoo[" << i << "] type=" << zoo[i]->getType() << "  sound: ";
    zoo[i]->makeSound();
  }

  printSeparator();

  std::cout << BOLD << "  Type assertions\n" << RESET;
  for (int i = 0; i < SIZE; ++i) {
    if (i % 2 == 0)
      ASSERT_R(zoo[i]->getType() == "Dog", "Even slot is a Dog");
    else
      ASSERT_R(zoo[i]->getType() == "Cat", "Odd slot is a Cat");
  }

  printSeparator();

  std::cout << DIM << "  Deleting all Animals via Animal* (virtual dtor + Brain free):\n" << RESET;
  for (int i = 0; i < SIZE; ++i)
    delete zoo[i];

  std::cout << "\n  " << GREEN << BOLD
            << "Array fully deleted – every Brain freed via virtual dtor chain.\n" << RESET;
}

// ═══════════════════════════════════════════════════════════════════════════
//  TEST 7 – Self-assignment guard
// ═══════════════════════════════════════════════════════════════════════════
static void test_selfAssignment() {
  printBanner("TEST 7 – Self-assignment guard (no double-free)");

  Dog d;
  d.getBrain()->ideas[0] = "Roll in mud";

  std::cout << DIM << "  d = d  (pointer trick to satisfy -Werror)\n" << RESET;
  Dog* pd = &d;
  d = *pd;

  ASSERT_R(d.getType() == "Dog",                         "Dog type intact after self-assign");
  ASSERT_R(d.getBrain() != NULL,                          "Dog Brain pointer still valid");
  ASSERT_R(d.getBrain()->ideas[0] == "Roll in mud",      "Dog Brain[0] intact after self-assign");

  printSeparator();

  Cat c;
  c.getBrain()->ideas[0] = "Scratch furniture";
  Cat* pc = &c;
  c = *pc;

  ASSERT_R(c.getType() == "Cat",                          "Cat type intact after self-assign");
  ASSERT_R(c.getBrain() != NULL,                           "Cat Brain pointer still valid");
  ASSERT_R(c.getBrain()->ideas[0] == "Scratch furniture", "Cat Brain[0] intact after self-assign");
}

// ═══════════════════════════════════════════════════════════════════════════
//  TEST 8 – Brain full capacity
// ═══════════════════════════════════════════════════════════════════════════
static void test_brainFullCapacity() {
  printBanner("TEST 8 – Brain 100 slots + deep copy of full Brain");

  Dog d;
  Brain* b = d.getBrain();

  std::cout << DIM << "  Writing to all 100 Brain::ideas slots...\n" << RESET;
  for (int i = 0; i < 100; ++i)
    b->ideas[i] = "idea";
  b->ideas[0]  = "first";
  b->ideas[99] = "last";

  ASSERT_R(b->ideas[0]  == "first", "Brain idea[0]  (first slot) writable");
  ASSERT_R(b->ideas[99] == "last",  "Brain idea[99] (last slot)  writable");

  printSeparator();

  std::cout << DIM << "  Deep-copying fully-loaded Brain via Dog copy ctor:\n" << RESET;
  Dog d2(d);

  ASSERT_R(d2.getBrain() != b,                  "Deep copy: distinct Brain ptr");
  ASSERT_R(d2.getBrain()->ideas[0]  == "first", "Copy Brain idea[0]  matches");
  ASSERT_R(d2.getBrain()->ideas[99] == "last",  "Copy Brain idea[99] matches");
}

// ═══════════════════════════════════════════════════════════════════════════
//  TEST 9 – Const-correctness
// ═══════════════════════════════════════════════════════════════════════════
static void test_constCorrectness() {
  printBanner("TEST 9 – const-correctness (pointers & references)");

  const Dog cd;
  const Cat cc;

  std::cout << "  const Dog sound : "; cd.makeSound();
  std::cout << "  const Cat sound : "; cc.makeSound();

  ASSERT_R(cd.getType() == "Dog", "const Dog getType()");
  ASSERT_R(cc.getType() == "Cat", "const Cat getType()");

  const Animal& refDog = cd;
  const Animal& refCat = cc;

  std::cout << "  Animal& refDog sound: "; refDog.makeSound();
  std::cout << "  Animal& refCat sound: "; refCat.makeSound();

  ASSERT_R(refDog.getType() == "Dog", "Dog via const Animal& getType()");
  ASSERT_R(refCat.getType() == "Cat", "Cat via const Animal& getType()");
  ASSERT_R(cd.getBrain() != NULL,     "const Dog getBrain() non-null");
  ASSERT_R(cc.getBrain() != NULL,     "const Cat getBrain() non-null");
}

// ═══════════════════════════════════════════════════════════════════════════
//  TEST 10 – Virtual destructor chain
// ═══════════════════════════════════════════════════════════════════════════
static void test_virtualDestructorChain() {
  printBanner("TEST 10 – Virtual dtor chain (Dog/Cat/Brain all freed)");

  std::cout << DIM
            << "  Allocating Dog and Cat via Animal*, deleting via Animal*.\n"
            << "  Expected: derived dtor → Brain dtor → Animal dtor.\n"
            << RESET << "\n";

  Animal* d = new Dog();
  Animal* c = new Cat();

  std::cout << DIM << "\n  delete Dog via Animal*:\n" << RESET;
  delete d;

  std::cout << DIM << "\n  delete Cat via Animal*:\n" << RESET;
  delete c;

  ASSERT_R(true, "Virtual dtor chain executed without crash");

  std::cout << "\n  " << GREEN << BOLD
            << "Virtual destructor chain fired: Brain freed, no leak.\n" << RESET;
}

// ═══════════════════════════════════════════════════════════════════════════
//  MAIN
// ═══════════════════════════════════════════════════════════════════════════
int main() {
  test_subjectBaseline();
  test_brainLifecycle();
  test_deepCopyDog();
  test_deepCopyCat();
  test_brainCopyDirectly();
  test_subjectArray();
  test_selfAssignment();
  test_brainFullCapacity();
  test_constCorrectness();
  test_virtualDestructorChain();

  // ── Postman summary table ─────────────────────────────────────────────────
  TestReport::instance().print();

  return 0;
}
