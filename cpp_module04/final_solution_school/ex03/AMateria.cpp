/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMateria.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:15:41 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 18:02:48 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AMateria.hpp"
#include <iostream>
#include <string>

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

/**
  The confusion comes from **who owns the spell** versus who performs the spell

  Character (Mage)
    │
    ├── Inventory
    │      │
    │      ├── Ice
    │      └── Cure
    │
    └── chooses which spell to cast
  The **character** owns the spell
  the **spell itself** knows how it does

  think of it like a `remote control`
  Character
   │
press button
   │
   ▼
Remote Control
   │
sends infrared signal
   ▼
TV

The person uess the rmote
but the remote knows how to send the infrared signal
similarly
Character
    │
uses
    ▼
AMateria
    │
implements
    ▼
Ice::use()

The character decides when to use it.
The materia decides what using it means.

That's why ICharacter also has a use() fnction

That's a classsi object-oriented principle called encapsulation:each object is responsible
for its own behavior. The character manages an inventoryy, and decides which materia to actiate, 
while each AMateria subcless defines what happened when it's activated-

*/
void AMateria::use(ICharacter& target) {
  std::cout << "* uses generic materia on " << target.getName() << " *"
            << std::endl;
}
