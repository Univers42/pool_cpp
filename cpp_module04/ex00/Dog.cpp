#include "Dog.hpp"

#include <iostream>

Dog::Dog() : Animal() {
  type = "Dog";
  std::cout << "Dog constructor called" << std::endl;
}

Dog::Dog(const Dog& src) : Animal(src) {
  std::cout << "Dog copy constructor called" << std::endl;
}

Dog& Dog::operator=(const Dog& rhs) {
  std::cout << "Dog copy assignment called" << std::endl;
  Animal::operator=(rhs);
  return *this;
}

Dog::~Dog() { std::cout << "Dog destructor called" << std::endl; }

void Dog::makeSound() const { std::cout << "Woof! Woof!" << std::endl; }
