#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include "Logger.hpp"

#include <iostream>

int main() {
    // =========================================================
    // PART 1: Correct behavior - virtual destructor chain
    // =========================================================
    PRINT_HEADER("PART 1: Virtual Destructor Chain");

    PRINT_LOG(InfoLog("Creating Dog and Cat via Animal* base pointers"));
    Animal* a1 = new Dog();
    Animal* a2 = new Cat();

    PRINT_SEP();
    PRINT_H3("Runtime types");
    PRINT_QUOTE(a1->getType());
    PRINT_QUOTE(a2->getType());

    PRINT_SEP();
    PRINT_H3("Calling makeSound() via base pointer");
    a1->makeSound();
    a2->makeSound();

    PRINT_SEP();
    PRINT_LOG(WarnLog("Deleting via Animal* - derived destructors WILL be called"));
    delete a1;
    delete a2;
    PRINT_LOG(SuccessLog("Dog::~Dog then Animal::~Animal => full cleanup"));
    PRINT_LOG(SuccessLog("Cat::~Cat then Animal::~Animal => full cleanup"));

    // =========================================================
    // PART 2: Wrong behavior - non-virtual destructor chain
    // =========================================================
    PRINT_HEADER("PART 2: Non-Virtual Destructor (WRONG)");

    PRINT_LOG(InfoLog("Creating WrongCat via WrongAnimal* base pointer"));
    WrongAnimal* w1 = new WrongCat();

    PRINT_SEP();
    PRINT_H3("Runtime type");
    PRINT_QUOTE(w1->getType());

    PRINT_SEP();
    PRINT_H3("Calling makeSound() via base pointer (method hiding)");
    w1->makeSound();

    PRINT_SEP();
    PRINT_LOG(ErrorLog("Deleting via WrongAnimal* - WrongCat destructor SKIPPED!"));
    delete w1;
    PRINT_LOG(ErrorLog("Only WrongAnimal::~WrongAnimal called => INCOMPLETE cleanup"));

    // =========================================================
    // Conclusion
    // =========================================================
    PRINT_HEADER("CONCLUSION");
    PRINT_SECTION("Virtual", "Dog/Cat destructors called correctly via Animal*");
    PRINT_SECTION("Non-Virtual", "WrongCat destructor SKIPPED when deleted via WrongAnimal*");

    return 0;
}