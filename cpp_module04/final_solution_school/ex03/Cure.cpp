/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cure.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:16:38 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 17:13:33 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cure.hpp"
#include <iostream>

Cure::Cure() : AMateria("cure") {}  // Type set to "cure"
Cure::Cure(const Cure& src) : AMateria(src) {}
Cure& Cure::operator=(const Cure& rhs) {
  (void)rhs;
  return *this;
}
Cure::~Cure() {}

AMateria* Cure::clone() const { return new Cure(*this); }

void Cure::use(ICharacter& target) {
  std::cout << "* heals " << target.getName() << "'s wounds *" << std::endl;
}
