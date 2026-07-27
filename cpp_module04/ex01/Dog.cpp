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

#include <iostream>

Dog::Dog() : Animal() {
  this->type = "Dog";
  this->_brain = new Brain();
  std::cout << "Dog constructor called" << std::endl;
}

Dog::Dog(const Dog& src) : Animal(src) {
  this->_brain = new Brain(*src._brain);
  std::cout << "Dog copy constructor called" << std::endl;
}

Dog& Dog::operator=(const Dog& rhs) {
  std::cout << "Dog copy assignment operator called" << std::endl;
  if (this != &rhs) {
    Animal::operator=(rhs);
    // clone first: if new throws, _brain must not be left dangling
    Brain* fresh = new Brain(*rhs._brain);
    delete this->_brain;
    this->_brain = fresh;
  }
  return *this;
}

Dog::~Dog() {
  delete this->_brain;
  std::cout << "Dog destructor called" << std::endl;
}

void Dog::makeSound() const { std::cout << "Woof! Woof! Bark!" << std::endl; }

Brain* Dog::getBrain() const { return this->_brain; }
