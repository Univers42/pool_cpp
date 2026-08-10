#include "WrongCat.hpp"

#include <iostream>

WrongCat::WrongCat() : WrongAnimal() {
  type = "WrongCat";
  #ifdef DEBUG
    std::cout << "WrongCat constructor called" << std::endl;
  #endif
}

WrongCat::WrongCat(const WrongCat& src) : WrongAnimal(src) {
  #ifdef DEBUG
    std::cout << "WrongCat copy constructor called" << std::endl;
  #endif
}

WrongCat& WrongCat::operator=(const WrongCat& rhs) {
  #ifdef DEBUG
    std::cout << "WrongCat copy assignment called" << std::endl;
  #endif
  WrongAnimal::operator=(rhs);
  return *this;
}

WrongCat::~WrongCat() {
  #ifdef DEBUG
    std::cout << "WrongCat destructor called" << std::endl;
  #endif
}

void WrongCat::makeSound() const { std::cout << "Wrong meow?!" << std::endl; }
