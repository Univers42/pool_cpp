/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:47:51 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:59:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Animal.hpp"
#include "Logger.hpp"

#include <iostream>
#include <string>

/**
 * NOTICE: The virtual key doens't appear from the .cpp because it's a 
 * declaration specifier. it belogs only in the .hpp putting it into the
 * *.cpp will throw an error
 */
// throw the assignment by default of animal throwed into the list memnber
// initializer
Animal::Animal() : type("Generic Animal") {
  LOG_CTOR();
}


Animal::Animal(const Animal& src) : type(src.type) {
  LOG_COPY();
}

// assignmen operator
Animal& Animal::operator=(const Animal& rhs) {
  LOG_ASSIGN();
  // Self assignment Guard          
  if (this != &rhs) {
    this->type = rhs.type;
  }
  return *this;
}

Animal::~Animal() {
  LOG_DTOR();
}

std::string Animal::getType() const { return this->type; }

void Animal::makeSound() const {
  std::cout << "* Unidentifiable generic animal noise *" << std::endl;
}
