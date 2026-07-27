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

#include <iostream>

Cat::Cat() : Animal() {
  this->type = "Cat";
  this->_brain = new Brain();
  std::cout << "Cat constructor called" << std::endl;
}

Cat::Cat(const Cat& src) : Animal(src) {
  this->_brain = new Brain(*src._brain);
  std::cout << "Cat copy constructor called" << std::endl;
}

Cat& Cat::operator=(const Cat& rhs) {
  std::cout << "Cat copy assignment operator called" << std::endl;
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
  std::cout << "Cat destructor called" << std::endl;
}

void Cat::makeSound() const {
  std::cout << "Meow! Purrrrr..." << std::endl;
}

Brain* Cat::getBrain() const { return this->_brain; }