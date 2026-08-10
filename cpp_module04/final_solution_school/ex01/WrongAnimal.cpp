/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongAnimal.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:49:03 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/03 19:28:30 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WrongAnimal.hpp"

#include <iostream>
#include <string>

WrongAnimal::WrongAnimal() : type("Wrong Generic") {
  #ifdef DEBUG
    std::cout << "WrongAnimal constructor called" << std::endl;
  #endif
}

WrongAnimal::WrongAnimal(const std::string& s): type(s) {}

WrongAnimal::WrongAnimal(const WrongAnimal& src) : type(src.type) {
  #ifdef DEBUG
    std::cout << "WrongAnimal copy constructor called" << std::endl;
  #endif
}

WrongAnimal& WrongAnimal::operator=(const WrongAnimal& rhs) {
  #ifdef DEBUG
    std::cout << "WrongAnimal copy assignment operator called" << std::endl;
  #endif
  if (this != &rhs) this->type = rhs.type;
  return *this;
}

WrongAnimal::~WrongAnimal() {
  #ifdef DEBUG
    std::cout << "WrongAnimal destructor called" << std::endl;
  #endif
}

std::string WrongAnimal::getType() const { return this->type; }

void WrongAnimal::makeSound() const {
  std::cout << "* Wrong generic noise *" << std::endl;
}
