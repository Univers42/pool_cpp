/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ice.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:16:26 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 11:20:03 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ice.hpp"
#include <iostream>

Ice::Ice() : AMateria("ice") {}  // Type set to "ice"
Ice::Ice(const Ice& src) : AMateria(src) {}
Ice& Ice::operator=(const Ice& rhs) {
  (void)rhs;
  return *this;
}
Ice::~Ice() {}

AMateria* Ice::clone() const { return new Ice(*this); }

void Ice::use(ICharacter& target) {
  std::cout << "* shoots an ice bolt at " << target.getName() << " *"
            << std::endl;
}
