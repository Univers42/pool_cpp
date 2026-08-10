/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:29 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 00:10:57 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dog.hpp"
#include <iostream>

Dog::Dog() : AAnimal() {
  this->type = "Dog";
  this->_brain = new Brain();
  std::cout << GREEN << "Dog default constructor called" << RESET << std::endl;
}

Dog::Dog(const Dog& src) : AAnimal(src) {
  this->_brain = new Brain(*src._brain);
  std::cout << YELLOW << "Dog deep-copy constructor called" << RESET
            << std::endl;
}

Dog& Dog::operator=(const Dog& rhs) {
  std::cout << BLUE << "Dog deep-copy assignment operator called" << RESET
            << std::endl;
  if (this != &rhs) {
    AAnimal::operator=(rhs);
    delete this->_brain;
    this->_brain = new Brain(*rhs._brain);
  }
  return *this;
}

Dog::~Dog() {
  delete this->_brain;
  std::cout << RED << "Dog destructor called" << RESET << std::endl;
}

void Dog::makeSound() const { std::cout << "Woof! Woof! Bark!" << std::endl; }

Brain* Dog::getBrain() const { return this->_brain; }
