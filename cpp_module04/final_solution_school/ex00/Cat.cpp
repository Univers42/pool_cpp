#include "Cat.hpp"

#include <iostream>

Cat::Cat() : Animal("Cat") {
  #ifdef DEBUG
  std::cout << "Cat constructor called" << std::endl;
  #endif
}

Cat::Cat(const Cat& src) : Animal(src) {
  #ifdef DEBUG
    std::cout << "Cat copy constructor called" << std::endl;
  #endif
}

Cat& Cat::operator=(const Cat& rhs) {
  #ifdef DEBUG
    std::cout << "Cat copy assignment called" << std::endl;
  #endif
  Animal::operator=(rhs);
  return *this;
}

Cat::~Cat() {
  #ifdef DEBUG
    std::cout << "Cat destructor called" << std::endl;
  #endif  
}

void Cat::makeSound() const { std::cout << "Meow!" << std::endl; }
