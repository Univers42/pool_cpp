/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:47:51 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/03 19:16:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Animal.hpp"

#include <iostream>

Animal::Animal() : type("Animal") {
  #ifdef DEBUG
    std::cout << "Animal constructor called" << std::endl;
  #endif
}
Animal::Animal(const std::string& s): type(s){}

Animal::Animal(const Animal& src) : type(src.type) {
  #ifdef DEBUG
    std::cout << "Animal copy constructor called" << std::endl;
  #endif
}

Animal& Animal::operator=(const Animal& rhs) {
  #ifdef DEBUG
    std::cout << "Animal copy assignment called" << std::endl;
  #endif
  if (this != &rhs) type = rhs.type;
  return (*this);
}

Animal::~Animal() { 
  #ifdef DEBUG
    std::cout << "Animal destructor called" << std::endl;
  #endif
}

std::string Animal::getType() const { return (type); }

void Animal::makeSound() const {
  std::cout << "* generic animal noise *" << std::endl;
}
