#include "Dog.hpp"

#include <iostream>

Dog::Dog() : Animal("Dog") {
  #ifdef DEBUG
    std::cout << "Dog constructor called" << std::endl;
  #endif
}

Dog::Dog(const Dog& src) : Animal(src) {
  #ifdef DEBUG
    std::cout << "Dog copy constructor called" << std::endl;
  #endif 
}

Dog& Dog::operator=(const Dog& rhs) {
  #ifdef DEBUG
    std::cout << "Dog copy assignment called" << std::endl;
  #endif
  if (this != &rhs)
    Animal::operator=(rhs);
  return *this;
}

Dog::~Dog() {
  #ifdef DEBUG
    std::cout << "Dog destructor called" << std::endl;
  #endif
}

void Dog::makeSound() const { std::cout << "Woof! Woof!" << std::endl; }
