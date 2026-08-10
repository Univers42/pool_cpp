/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMateria.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:15:41 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:28:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AMateria.hpp"
#include <iostream>
#include <string>

AMateria::AMateria() : type("generic") {}

AMateria::AMateria(std::string const& type) : type(type) {}

AMateria::AMateria(const AMateria& src) : type(src.type) {}

AMateria& AMateria::operator=(const AMateria& rhs) {
  (void)rhs;
  // The subject strictly states: "While assigning a Materia to another,
  // copying the type doesn't make sense."
  return *this;
}

AMateria::~AMateria() {}

std::string const& AMateria::getType() const { return this->type; }

void AMateria::use(ICharacter& target) {
  std::cout << "* uses generic materia on " << target.getName() << " *"
            << std::endl;
}
