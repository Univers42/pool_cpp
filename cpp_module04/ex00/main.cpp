#include <iostream>

#include "Animal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"

int main() {
  // Subject example: virtual dispatch through Animal* picks the real type.
  const Animal* meta = new Animal();
  const Animal* j = new Dog();
  const Animal* i = new Cat();

  std::cout << j->getType() << " " << std::endl;
  std::cout << i->getType() << " " << std::endl;
  i->makeSound();  // will output the cat sound!
  j->makeSound();
  meta->makeSound();

  delete meta;
  delete j;  // virtual dtor: Dog then Animal destructor run
  delete i;

  // Same code with the Wrong hierarchy: makeSound() is not virtual, so the
  // WrongAnimal version is called even though the object is a WrongCat.
  std::cout << "--- Wrong hierarchy ---" << std::endl;
  const WrongAnimal* wrong = new WrongCat();
  std::cout << wrong->getType() << " " << std::endl;
  wrong->makeSound();  // outputs the WrongAnimal sound
  delete wrong;

  return 0;
}
