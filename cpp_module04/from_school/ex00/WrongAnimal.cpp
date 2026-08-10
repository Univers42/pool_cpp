/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongAnimal.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:49:03 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:23:36 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WrongAnimal.hpp"
#include "Logger.hpp"
#include <iostream>
#include <string>

WrongAnimal::WrongAnimal() : type("WrongAnimal") {
  LOG_CTOR();
}

WrongAnimal::WrongAnimal(const WrongAnimal& src) : type(src.type) {
  LOG_COPY();
}

WrongAnimal& WrongAnimal::operator=(const WrongAnimal& rhs) {
  LOG_ASSIGN();
  if (this != &rhs) {
    this->type = rhs.type;
  }
  return *this;
}

WrongAnimal::~WrongAnimal() {
  LOG_DTOR();
}

std::string WrongAnimal::getType() const { return this->type; }

void WrongAnimal::makeSound() const {
  std::cout << "* Wrong generic noise *" << std::endl;
}
