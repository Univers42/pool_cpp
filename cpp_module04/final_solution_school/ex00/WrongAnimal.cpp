#include "WrongAnimal.hpp"

#include <iostream>

WrongAnimal::WrongAnimal() : type("WrongAnimal") {
  #ifdef DEBUG
    std::cout << "WrongAnimal constructor called" << std::endl;
  #endif
}

WrongAnimal::WrongAnimal(const WrongAnimal& src) : type(src.type) {
  #ifdef DEBUG
    std::cout << "WrongAnimal copy constructor called" << std::endl;
  #endif
}

WrongAnimal& WrongAnimal::operator=(const WrongAnimal& rhs) {
  #ifdef DEBUG
    std::cout << "WrongAnimal copy assignment called" << std::endl;
  #endif
  if (this != &rhs) type = rhs.type;
  return *this;
}

WrongAnimal::~WrongAnimal() {
  #ifdef DEBUG
    std::cout << "WrongAnimal destructor called" << std::endl;
  #endif
}

std::string WrongAnimal::getType() const { return type; }

void WrongAnimal::makeSound() const {
  std::cout << "* wrong generic noise *" << std::endl;
}
