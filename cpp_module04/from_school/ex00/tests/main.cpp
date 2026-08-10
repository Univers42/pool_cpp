#include <iostream>
#include <string>

#include "Animal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include "postman.hpp"
#include "LeakGuard.hpp"

#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

// ── Banner: registers the suite name with TestReport ─────────────────────────
static void printBanner(const std::string& title) {
  TestReport::instance().beginSuite(title);
  std::cout << "\n\n\n"
            << CYAN << BOLD << std::string(70, '=') << RESET << "\n"
            << CYAN << BOLD << "  >>> " << RESET
            << BOLD << WHITE << title << RESET << "\n"
            << CYAN << BOLD << std::string(70, '=') << RESET << "\n"
            << std::endl;
}

static void printSeparator() {
  std::cout << "\n" << DIM << std::string(70, '-') << RESET << "\n\n";
}

/**
 * ═══════════════════════════════════════════════════════════════════════════
 * TEST 1 – Subject-mandated baseline
 * ═══════════════════════════════════════════════════════════════════════════
 * @brief Tests dynamic polymorphism via base class pointers.
 * * **Concept Tested:** Dynamic Binding (Runtime Polymorphism).
 * * **What it does:** Allocates `Dog` and `Cat` objects on the heap but stores 
 * them inside `const Animal*` pointers. It then calls `getType()` and `makeSound()`.
 * * **Expectation:** Because `makeSound()` is marked `virtual` in the `Animal` 
 * base class, calling it on an `Animal*` that points to a `Dog` should execute 
 * `Dog::makeSound()`, not `Animal::makeSound()`. The program dynamically 
 * determines the actual object type at runtime.
 */
static void test_subjectBaseline() {
  printBanner("TEST 1 – Subject-mandated baseline (polymorphic pointers)");

  // allocate view new
  const Animal* meta = new Animal();
  const Animal* j    = new Dog();
  const Animal* i    = new Cat();

  printSeparator();
  // Sample referenced
  std::cout << BOLD << "  getType() checks\n" << RESET;
  std::cout << "  Dog type    : " << j->getType()    << "\n";
  std::cout << "  Cat type    : " << i->getType()    << "\n";
  std::cout << "  Animal type : " << meta->getType() << "\n";

  // Assesment if type is correct
  ASSERT_R(j->getType() == "Dog",    "Dog::getType() returns \"Dog\"");
  ASSERT_R(i->getType() == "Cat",    "Cat::getType() returns \"Cat\"");
  ASSERT_R(meta->getType() == "Generic Animal",
                                     "Animal::getType() returns \"Generic Animal\"");

  printSeparator();
  std::cout << BOLD << "  makeSound() via Animal* (virtual dispatch)\n" << RESET;
  std::cout << "  Cat sound   : "; i->makeSound();
  std::cout << "  Dog sound   : "; j->makeSound();
  std::cout << "  Animal sound: "; meta->makeSound();

  printSeparator();
  std::cout << DIM << "  Deleting heap objects...\n" << RESET;
  delete meta;
  delete j;
  delete i;
}

/**
 * ═══════════════════════════════════════════════════════════════════════════
 * TEST 2 – Stack objects & copy semantics
 * ═══════════════════════════════════════════════════════════════════════════ 
 * @brief Tests the Orthodox Canonical Class Form components.
 * * **Concept Tested:** Copy Constructors and Copy Assignment Operators.
 * * **What it does:** Instantiates objects on the stack, then creates new objects 
 * using the copy constructor (`Dog d2(d1)`) and copy assignment operator (`d3 = d1`).
 * * **Expectation:** The copied objects must retain the correct types and states 
 * of their source objects. This ensures that the base class parts of the objects 
 * are correctly copied alongside the derived class parts.
 */
static void test_stackAndCopy() {
  printBanner("TEST 2 – Stack objects & copy/assignment semantics");

  std::cout << DIM << "  Constructing Dog and Cat on the stack...\n" << RESET;
  Dog d1;
  Cat c1;

  // Copy constructor
  printSeparator();
  std::cout << BOLD << "  Copy constructor\n" << RESET;
  Dog d2(d1);
  Cat c2(c1);
  ASSERT_R(d2.getType() == "Dog", "Dog copy constructor preserves type");
  ASSERT_R(c2.getType() == "Cat", "Cat copy constructor preserves type");

  // Assignment operaotr
  printSeparator();
  std::cout << BOLD << "  Assignment operator\n" << RESET;
  Dog d3;
  Cat c3;
  d3 = d1;
  c3 = c1;
  ASSERT_R(d3.getType() == "Dog", "Dog assignment preserves type");
  ASSERT_R(c3.getType() == "Cat", "Cat assignment preserves type");

  // makeSound 
  printSeparator();
  std::cout << BOLD << "  makeSound() on stack copies\n" << RESET;
  std::cout << "  d2 : "; d2.makeSound();
  std::cout << "  c2 : "; c2.makeSound();
}

/**
 * ═══════════════════════════════════════════════════════════════════════════
 * TEST 3 – Polymorphic array
 * ═══════════════════════════════════════════════════════════════════════════
 * @brief Tests storing multiple derived types in a uniform base container.
 * * **Concept Tested:** Array of Base Pointers.
 * * **What it does:** Creates an array of `Animal*` and populates it alternatingly 
 * with `Dog*` and `Cat*`. It then loops through the array to call methods.
 * * **Expectation:** Demonstrates the power of OOP: a single loop treating all 
 * objects as `Animal`s will still execute the specific `Dog` or `Cat` behavior 
 * automatically due to virtual dispatch.
 */
static void test_polymorphicArray() {
  printBanner("TEST 3 – Polymorphic array (Animal* array of Dogs & Cats)");

  const int SIZE = 6;
  Animal* zoo[SIZE];

  std::cout << DIM << "  Filling array: even=Dog, odd=Cat\n" << RESET;
  for (int i = 0; i < SIZE; ++i)
    zoo[i] = (i % 2 == 0) ? static_cast<Animal*>(new Dog())
                           : static_cast<Animal*>(new Cat());

  printSeparator();
  std::cout << BOLD << "  Type & sound via Animal*\n" << RESET;
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
  std::cout << DIM << "  Deleting array...\n" << RESET;
  for (int i = 0; i < SIZE; ++i)
    delete zoo[i];
}

/**
 * ═══════════════════════════════════════════════════════════════════════════
 * TEST 4 – Self-assignment guard
 * ═══════════════════════════════════════════════════════════════════════════
 * @brief Verifies safety mechanisms in the assignment operator.
 * * **Concept Tested:** Self-Assignment (`if (this != &rhs)`).
 * * **What it does:** Forces an object to assign itself to itself (`d = *pd;`).
 * * **Expectation:** The program must not crash or wipe its own data. If the 
 * self-assignment guard is missing, an object might delete its own resources 
 * before trying to copy them, leading to undefined behavior or segfaults.
 */
static void test_selfAssignment() {
  printBanner("TEST 4 – Self-assignment guard");

  Dog d;
  Dog* pd = &d;
  d = *pd;
  ASSERT_R(d.getType() == "Dog", "Dog type intact after self-assignment");

  Cat c;
  Cat* pc = &c;
  c = *pc;
  ASSERT_R(c.getType() == "Cat", "Cat type intact after self-assignment");

  std::cout << DIM << "  No crash → self-assignment guard works.\n" << RESET;
}



/**
 * ═══════════════════════════════════════════════════════════════════════════
 * TEST 5 – WrongAnimal / WrongCat
 * ═══════════════════════════════════════════════════════════════════════════
 * @brief Demonstrates the failure of polymorphism without the virtual keyword.
 * * **Concept Tested:** Static Binding (Early Binding).
 * * **What it does:** Uses `WrongAnimal` and `WrongCat` classes where `makeSound()` 
 * is intentionally NOT marked as virtual. It calls methods through pointers and references.
 * * **Expectation:** When calling `wrongMeta->makeSound()` on a `WrongAnimal*` 
 * pointing to a `WrongCat`, it will output the *base* class sound. Without 
 * `virtual`, the compiler decides which function to call based on the pointer 
 * type (`WrongAnimal*`), completely ignoring the actual object type in memory.
 */
static void test_wrongHierarchy() {
  printBanner("TEST 5 – WrongAnimal / WrongCat (non-virtual makeSound)");

  const WrongAnimal* wrongMeta = new WrongAnimal();
  const WrongAnimal* wrongCat  = new WrongCat();

  std::cout << "  wrongCat->getType()   : " << wrongCat->getType() << "\n";
  std::cout << "  wrongCat->makeSound() : "; wrongCat->makeSound();
  std::cout << "  wrongMeta->makeSound(): "; wrongMeta->makeSound();

  ASSERT_R(wrongCat->getType() == "WrongCat",
           "WrongCat::getType() returns \"WrongCat\"");
  ASSERT_R(wrongMeta->getType() == "WrongAnimal",
           "WrongAnimal::getType() returns \"WrongAnimal\"");

  printSeparator();
  // Non-virtual: WrongAnimal* calls WrongAnimal::makeSound even on a WrongCat
  WrongCat wc;
  const WrongAnimal& waRef = wc;
  std::cout << "  WrongCat direct call  : "; wc.makeSound();    // WrongCat::makeSound
  std::cout << "  WrongAnimal& ref call : "; waRef.makeSound(); // WrongAnimal::makeSound (static binding)

  ASSERT_R(wc.getType() == "WrongCat",
           "Stack WrongCat has correct type");

  printSeparator();
  delete wrongMeta;
  delete wrongCat;
}

/**
 * ═══════════════════════════════════════════════════════════════════════════
 * TEST 6 – Animal base class directly
 * ═══════════════════════════════════════════════════════════════════════════
 * @brief Tests the standalone stability of the base class.
 * * **Concept Tested:** Base Class Instantiation.
 * * **What it does:** Directly creates, copies, and assigns pure `Animal` objects.
 * * **Expectation:** Ensures that the base class itself is well-formed, fully 
 * implements the Orthodox Canonical form, and can exist independently without 
 * crashing.
 */
static void test_baseAnimalDirect() {
  printBanner("TEST 6 – Animal base class used directly");

  Animal a1;
  std::cout << "  a1.getType()  : " << a1.getType() << "\n";
  std::cout << "  a1.makeSound(): "; a1.makeSound();
  ASSERT_R(a1.getType() == "Generic Animal",
           "Base Animal getType() is \"Generic Animal\"");

  Animal a2(a1);
  ASSERT_R(a2.getType() == "Generic Animal",
           "Copied Animal has correct type");

  Animal a3;
  a3 = a1;
  ASSERT_R(a3.getType() == "Generic Animal",
           "Assigned Animal has correct type");
}

/**
 * ═══════════════════════════════════════════════════════════════════════════
 * TEST 7 – const-correctness
 * ═══════════════════════════════════════════════════════════════════════════
 * @brief Validates the strict use of the `const` qualifier on member functions.
 * * **Concept Tested:** Const-Correctness.
 * * **What it does:** Instantiates `const Dog` and `const Cat` objects, and accesses 
 * them via `const Animal&` references.
 * * **Expectation:** The code will only compile and run successfully if `getType()` 
 * and `makeSound()` are properly appended with the `const` keyword in their 
 * declarations. It proves these methods do not modify the object's internal state.
 */
static void test_constCorrectness() {
  printBanner("TEST 7 – const-correctness (getType/makeSound on const refs)");

  const Dog cd;
  const Cat cc;

  std::cout << "  const Dog type  : " << cd.getType() << "\n";
  std::cout << "  const Cat type  : " << cc.getType() << "\n";
  std::cout << "  const Dog sound : "; cd.makeSound();
  std::cout << "  const Cat sound : "; cc.makeSound();

  ASSERT_R(cd.getType() == "Dog", "const Dog getType()");
  ASSERT_R(cc.getType() == "Cat", "const Cat getType()");

  const Animal& refDog = cd;
  const Animal& refCat = cc;
  std::cout << "  Animal& ref Dog sound: "; refDog.makeSound();
  std::cout << "  Animal& ref Cat sound: "; refCat.makeSound();
  ASSERT_R(refDog.getType() == "Dog", "Dog accessible via const Animal&");
  ASSERT_R(refCat.getType() == "Cat", "Cat accessible via const Animal&");
}

/**
 * ═══════════════════════════════════════════════════════════════════════════
 * TEST 8 – Virtual destructor chain
 * ═══════════════════════════════════════════════════════════════════════════
 * @brief Ensures dynamic memory is safely returned to the system.
 * * **Concept Tested:** Virtual Destructors.
 * * **What it does:** Deletes a derived object (`Dog` or `Cat`) through a base 
 * class pointer (`Animal*`). 
 * * **Expectation:** Because `Animal::~Animal()` is virtual, deleting `d` triggers 
 * the `Dog` destructor *first*, and then automatically chains up to the `Animal` 
 * destructor. If it were not virtual, only the `Animal` destructor would fire, 
 * causing a memory leak for any `Dog`-specific allocations.
 */
static void test_destructorOrder() {
  printBanner("TEST 8 – Virtual destructor chain");

  TestReport::instance().snapshotMemory();
  {
    Animal* d = new Dog();
    Animal* c = new Cat();
    delete d;
    delete c;
  }
  ASSERT_NO_LEAKS("Zero memory leaks detected in destructor chain");
}

// ═══════════════════════════════════════════════════════════════════════════
//  MAIN
// ═══════════════════════════════════════════════════════════════════════════
int main() {
  /* Enable leak tracking from the very start so that ALL allocations
   * (including lazy static inits from the verbose framework) are counted
   * and their matching deletes are also tracked.  */
  LeakGuard::enable();

  test_subjectBaseline();
  test_stackAndCopy();
  test_polymorphicArray();
  test_selfAssignment();
  test_wrongHierarchy();
  test_baseAnimalDirect();
  test_constCorrectness();
  test_destructorOrder();

  LeakGuard::disable();
  TestReport::instance().print();
  return 0;
}