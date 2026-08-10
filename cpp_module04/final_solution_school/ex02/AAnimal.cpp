/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAnimal.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:04:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 00:10:47 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AAnimal.hpp"
#include <iostream>
#include <string>

AAnimal::AAnimal() : type("Generic AAnimal") {
  std::cout << GREEN << "AAnimal default constructor called" << RESET
            << std::endl;
}

AAnimal::AAnimal(const AAnimal& src) : type(src.type) {
  std::cout << YELLOW << "AAnimal copy constructor called" << RESET
            << std::endl;
}

AAnimal& AAnimal::operator=(const AAnimal& rhs) {
  std::cout << BLUE << "AAnimal assignment operator called" << RESET
            << std::endl;
  if (this != &rhs) {
    this->type = rhs.type;
  }
  return *this;
}

AAnimal::~AAnimal() {
  std::cout << RED << "AAnimal destructor called" << RESET << std::endl;
}

std::string AAnimal::getType() const { return this->type; }
