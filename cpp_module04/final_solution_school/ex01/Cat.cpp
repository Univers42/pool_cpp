/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cat.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:46 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/03 20:59:31 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cat.hpp"

#include <iostream>

Cat::Cat() : Animal("Cat") {
  this->_brain = new Brain();
  #ifdef DEBUG
    std::cout << "Cat constructor : " << this << std::endl;
  #endif
}

Cat::Cat(const Cat& src) : Animal(src) {
  this->_brain = new Brain(*src._brain);
  #ifdef DEBUG
    std::cout << "Cat copy constructor called" << std::endl;
  #endif
}

Cat& Cat::operator=(const Cat& rhs) {
  #ifdef DEBUG
    std::cout << "Cat copy assignment operator called" << std::endl;
  #endif
  if (this != &rhs) {
    Animal::operator=(rhs);
    // clone first: if new throws, _brain must not be left dangling
    Brain* fresh = new Brain(*rhs._brain);
    delete this->_brain;
    this->_brain = fresh;
  }
  return *this;
}

Cat::~Cat() {
  delete this->_brain;
  #ifdef DEBUG
    std::cout << "Cat destructor" << this << std::endl;
  #endif
}

void Cat::makeSound() const {
  std::cout << "Meow! Purrrrr..." << std::endl;
}

Brain* Cat::getBrain() const { return this->_brain; }