/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongCat.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:49:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:57:49 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WrongCat.hpp"

#include <iostream>

WrongCat::WrongCat() : WrongAnimal() {
  this->type = "WrongCat";
  std::cout << "WrongCat default constructor" << std::endl;
}

WrongCat::WrongCat(const WrongCat& src) : WrongAnimal(src) {
  std::cout << "WrongCat copy constructor" << std::endl;
}

WrongCat& WrongCat::operator=(const WrongCat& rhs) {
  if (this != &rhs) {
    WrongAnimal::operator=(rhs);
    this->type = rhs.type;
  }
  return *this;
}

WrongCat::~WrongCat() { std::cout << "WrongCat destructor" << std::endl; }

void WrongCat::makeSound() const { std::cout << "Wrong Meow!" << std::endl; }
