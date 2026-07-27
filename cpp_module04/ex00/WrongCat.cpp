#include "WrongCat.hpp"

#include <iostream>

WrongCat::WrongCat() : WrongAnimal() {
  type = "WrongCat";
  std::cout << "WrongCat constructor called" << std::endl;
}

WrongCat::WrongCat(const WrongCat& src) : WrongAnimal(src) {
  std::cout << "WrongCat copy constructor called" << std::endl;
}

WrongCat& WrongCat::operator=(const WrongCat& rhs) {
  std::cout << "WrongCat copy assignment called" << std::endl;
  WrongAnimal::operator=(rhs);
  return *this;
}

WrongCat::~WrongCat() {
  std::cout << "WrongCat destructor called" << std::endl;
}

void WrongCat::makeSound() const { std::cout << "Wrong meow?!" << std::endl; }
