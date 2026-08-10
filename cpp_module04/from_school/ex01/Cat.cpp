/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cat.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:46 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 18:14:14 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cat.hpp"
#include "Logger.hpp"

#include <iostream>

Cat::Cat() : Animal() {
  this->type = "Cat";
  this->_brain = new Brain();
  LOG_CTOR();
}

Cat::Cat(const Cat& src) : Animal(src) {
  this->_brain = new Brain(*src._brain);
  LOG_COPY();
}

Cat& Cat::operator=(const Cat& rhs) {
  LOG_ASSIGN();
  if (this != &rhs) {
    Animal::operator=(rhs);
    delete this->_brain;
    this->_brain = new Brain(*rhs._brain);
  }
  return *this;
}

Cat::~Cat() {
  delete this->_brain;
  LOG_DTOR();
}

void Cat::makeSound() const {
  std::cout << "Meow! Purrrrr..." << std::endl;
}

Brain* Cat::getBrain() const { return this->_brain; }