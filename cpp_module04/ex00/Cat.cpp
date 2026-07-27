#include "Cat.hpp"

#include <iostream>

Cat::Cat() : Animal() {
  type = "Cat";
  std::cout << "Cat constructor called" << std::endl;
}

Cat::Cat(const Cat& src) : Animal(src) {
  std::cout << "Cat copy constructor called" << std::endl;
}

Cat& Cat::operator=(const Cat& rhs) {
  std::cout << "Cat copy assignment called" << std::endl;
  Animal::operator=(rhs);
  return *this;
}

Cat::~Cat() { std::cout << "Cat destructor called" << std::endl; }

void Cat::makeSound() const { std::cout << "Meow!" << std::endl; }
