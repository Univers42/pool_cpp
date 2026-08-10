/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cat.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:46 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 00:09:35 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cat.hpp"

#include <iostream>

Cat::Cat() : AAnimal() {
  this->type = "Cat";
  this->_brain = new Brain();
  std::cout << GREEN << "Cat default constructor called" << RESET << std::endl;
}

Cat::Cat(const Cat& src) : AAnimal(src) {
  this->_brain = new Brain(*src._brain);
  std::cout << YELLOW << "Cat deep-copy constructor called" << RESET
            << std::endl;
}

Cat& Cat::operator=(const Cat& rhs) {
  std::cout << BLUE << "Cat deep-copy assignment operator called" << RESET
            << std::endl;
  if (this != &rhs) {
    AAnimal::operator=(rhs);
    delete this->_brain;
    this->_brain = new Brain(*rhs._brain);
  }
  return *this;
}

Cat::~Cat() {
  delete this->_brain;
  std::cout << RED << "Cat destructor called" << RESET << std::endl;
}

void Cat::makeSound() const { std::cout << "Meow! Purrrrr..." << std::endl; }

Brain* Cat::getBrain() const { return this->_brain; }
