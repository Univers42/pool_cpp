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

#include <iostream>
#include <string>
Animal::Animal() : type("Generic Animal") {
  std::cout << GREEN << "Animal default constructor called" << RESET
            << std::endl;
}

Animal::Animal(const Animal& src) : type(src.type) {
  std::cout << YELLOW << "Animal copy constructor called" << RESET << std::endl;
}

Animal& Animal::operator=(const Animal& rhs) {
  std::cout << BLUE << "Animal assignment operator called" << RESET
            << std::endl;
  if (this != &rhs) {
    this->type = rhs.type;
  }
  return *this;
}

Animal::~Animal() {
  std::cout << RED << "Animal destructor called" << RESET << std::endl;
}

std::string Animal::getType() const { return this->type; }

void Animal::makeSound() const {
  std::cout << "* Unidentifiable generic animal noise *" << std::endl;
}
