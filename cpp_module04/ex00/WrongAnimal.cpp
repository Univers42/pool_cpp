#include "WrongAnimal.hpp"

#include <iostream>

WrongAnimal::WrongAnimal() : type("WrongAnimal") {
  std::cout << "WrongAnimal constructor called" << std::endl;
}

WrongAnimal::WrongAnimal(const WrongAnimal& src) : type(src.type) {
  std::cout << "WrongAnimal copy constructor called" << std::endl;
}

WrongAnimal& WrongAnimal::operator=(const WrongAnimal& rhs) {
  std::cout << "WrongAnimal copy assignment called" << std::endl;
  if (this != &rhs) type = rhs.type;
  return *this;
}

WrongAnimal::~WrongAnimal() {
  std::cout << "WrongAnimal destructor called" << std::endl;
}

std::string WrongAnimal::getType() const { return type; }

void WrongAnimal::makeSound() const {
  std::cout << "* wrong generic noise *" << std::endl;
}
