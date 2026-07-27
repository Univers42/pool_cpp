#include "Animal.hpp"

#include <iostream>

Animal::Animal() : type("Animal") {
  std::cout << "Animal constructor called" << std::endl;
}

Animal::Animal(const Animal& src) : type(src.type) {
  std::cout << "Animal copy constructor called" << std::endl;
}

Animal& Animal::operator=(const Animal& rhs) {
  std::cout << "Animal copy assignment called" << std::endl;
  if (this != &rhs) type = rhs.type;
  return *this;
}

Animal::~Animal() { std::cout << "Animal destructor called" << std::endl; }

std::string Animal::getType() const { return type; }

void Animal::makeSound() const {
  std::cout << "* generic animal noise *" << std::endl;
}
