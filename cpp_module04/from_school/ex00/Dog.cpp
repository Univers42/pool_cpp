/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:29 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 18:13:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dog.hpp"
#include "Logger.hpp"
#include <iostream>

Dog::Dog() : Animal() {
  this->type = "Dog";
  LOG_CTOR();
}

Dog::Dog(const Dog& src) : Animal(src) {
  LOG_COPY();
  *this = src;
}

Dog& Dog::operator=(const Dog& rhs) {
  LOG_ASSIGN();
  if (this != &rhs) {
    this->type = rhs.type;
  }
  return *this;
}

Dog::~Dog() {
  LOG_DTOR();
}

void Dog::makeSound() const { std::cout << "Woof! Woof! Bark!" << std::endl; }
