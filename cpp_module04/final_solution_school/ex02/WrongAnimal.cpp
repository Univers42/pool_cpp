/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongAnimal.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:49:03 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:59:51 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WrongAnimal.hpp"

#include <iostream>
#include <string>

WrongAnimal::WrongAnimal() : type("Wrong Generic") {
  std::cout << "WrongAnimal default constructor" << std::endl;
}

WrongAnimal::WrongAnimal(const WrongAnimal& src) : type(src.type) {
  std::cout << "WrongAnimal copy constructor" << std::endl;
}

WrongAnimal& WrongAnimal::operator=(const WrongAnimal& rhs) {
  if (this != &rhs) this->type = rhs.type;
  return *this;
}

WrongAnimal::~WrongAnimal() {
  std::cout << "WrongAnimal destructor" << std::endl;
}

std::string WrongAnimal::getType() const { return this->type; }

void WrongAnimal::makeSound() const {
  std::cout << "* Wrong generic noise *" << std::endl;
}
