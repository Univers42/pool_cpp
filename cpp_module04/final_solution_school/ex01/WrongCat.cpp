/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongCat.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:49:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/03 19:25:27 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WrongCat.hpp"

#include <iostream>

WrongCat::WrongCat() : WrongAnimal("WrongCat") {
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
    std::cout << "WrongCat copy assignment operator called" << std::endl;
  #endif
  if (this != &rhs) {
    WrongAnimal::operator=(rhs);
    this->type = rhs.type;
  }
  return *this;
}

WrongCat::~WrongCat() {
  #ifdef DEBUG
    std::cout << "WrongCat destructor called" << std::endl;
  #endif
}

void WrongCat::makeSound() const { std::cout << "Wrong Meow!" << std::endl; }
