// Using makeSound() function always called the appropriate makeSound()
// function. makeSound() should be virtual! Verify it in the code.
// virtual void makeSound() const;
// The return value is not important but virtual keyword is mandatory.

// There should be an example with a WrongAnimal and WrongCat that don't
// use the virtual keyword (see subject).
// The WrongCat must output the WrongCat makeSound() only when used as a
// wrongCat.

#include "../../libcpp/TermWriter.hpp"
#include "../../libcpp/TermStyle.hpp"
#include "../Animal.hpp"
#include "../Cat.hpp"
#include "../Dog.hpp"
#include <iostream>


int main(void)
{
    TermStyle ts;
    TermWriter w(ts, std::cout);

    w << "# Demo: Virtual Functions in Action"
      << "This demonstrates how virtual functions enable dynamic dispatch."
      << "---";
    w.flush();

    w << "## Creating Animals via Base Pointers"
      << ">![info] We create `Animal*` pointers that actually point to `Dog` and `Cat` objects.";
    w.flush();

    const Animal* dog = new Dog();
    const Animal* cat = new Cat();

    w << "## Calling makeSound() on Base Pointers"
      << "> Calling `makeSound()` on an `Animal*` that points to a `Dog` or `Cat` should execute the derived class's version of the function.";
    w.flush();

    w << "!v Output for Dog pointer:";
    w.flush();
    dog->makeSound(); // Should call Dog::makeSound()

    w << "!v Output for Cat pointer:";
    w.flush();
    cat->makeSound(); // Should call Cat::makeSound()

    w << "---"
      << ">![success] The correct sounds are produced, demonstrating that `makeSound()` is virtual and dynamic dispatch is working.";
    w.flush();

    delete dog;
    delete cat;

    return 0;
}