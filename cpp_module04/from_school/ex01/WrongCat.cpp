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
#include "Logger.hpp"

#include <iostream>

WrongCat::WrongCat() : WrongAnimal() {
  this->type = "WrongCat";
  LOG_CTOR();
}

WrongCat::WrongCat(const WrongCat& src) : WrongAnimal(src) {
  LOG_COPY();
}

WrongCat& WrongCat::operator=(const WrongCat& rhs) {
  LOG_ASSIGN();
  if (this != &rhs) {
    WrongAnimal::operator=(rhs);
    this->type = rhs.type;
  }
  return *this;
}

WrongCat::~WrongCat() { LOG_DTOR(); }

void WrongCat::makeSound() const { std::cout << "Wrong Meow!" << std::endl; }
