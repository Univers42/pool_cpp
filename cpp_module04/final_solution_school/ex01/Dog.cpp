/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:29 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/03 20:59:13 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dog.hpp"

#include <iostream>

Dog::Dog() : Animal("Dog") {
  this->_brain = new Brain();
  #ifdef DEBUG
    std::cout << "Dog constructor :" << this << std::endl;
  #endif
}

Dog::Dog(const Dog& src) : Animal(src) {
  this->_brain = new Brain(*src._brain);
  //this->_brain = src._brain;
  #ifdef DEBUG
    std::cout << "Dog copy constructor called" << std::endl;
  #endif
}

Dog& Dog::operator=(const Dog& rhs) {
  #ifdef DEBUG
    std::cout << "Dog copy assignment operator called" << std::endl;
  #endif
  if (this != &rhs) {
    Animal::operator=(rhs);
    // clone first: if new throws, _brain must not be left dangling
    Brain* fresh = new Brain(*rhs._brain);
    delete this->_brain;
    this->_brain = fresh;
  }
  return (*this);
}

Dog::~Dog() {
  delete this->_brain;
  #ifdef DEBUG
    std::cout << "Dog destructor : " << this << std::endl;
  #endif
}

void Dog::makeSound() const { std::cout << "Woof! Woof! Bark!" << std::endl; }

Brain* Dog::getBrain() const { return this->_brain; }

void Dog::setIdea(int idx, const std::string& idea) {
  this->_brain->setIdea(idx, idea);
}

std::string Dog::getIdea(int idx) const {
    return (this->_brain->getIdea(idx));
}