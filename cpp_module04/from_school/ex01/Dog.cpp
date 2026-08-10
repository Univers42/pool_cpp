/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:29 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 18:14:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dog.hpp"
#include "Logger.hpp"

#include <iostream>

Dog::Dog() : Animal() {
  this->type = "Dog";
  this->_brain = new Brain();
  LOG_CTOR();
}

Dog::Dog(const Dog& src) : Animal(src) {
  this->_brain = new Brain(*src._brain);
  LOG_COPY();
}

Dog& Dog::operator=(const Dog& rhs) {
  LOG_ASSIGN();
  if (this != &rhs) {
    Animal::operator=(rhs);
    delete this->_brain;
    this->_brain = new Brain(*rhs._brain);
  }
  return *this;
}

Dog::~Dog() {
  delete this->_brain;
  LOG_DTOR();
}

void Dog::makeSound() const { std::cout << "Woof! Woof! Bark!" << std::endl; }

Brain* Dog::getBrain() const { return this->_brain; }
