/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cat.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:46 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/06 15:12:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cat.hpp"
#include "Logger.hpp"
#include <iostream>

Cat::Cat() : Animal() {
  this->type = "Cat";
  LOG_CTOR();
}

Cat::Cat(const Cat& src) : Animal(src) {
  LOG_COPY();
  *this = src;
}

Cat& Cat::operator=(const Cat& rhs) {
  LOG_ASSIGN();
  if (this != &rhs) {
    this->type = rhs.type;
  }
  return *this;
}

Cat::~Cat() {
  LOG_DTOR();
}

void Cat::makeSound() const {
  std::cout << "Meow! Purrrrr..."
            << std::endl;  // Appropriate sound
}
