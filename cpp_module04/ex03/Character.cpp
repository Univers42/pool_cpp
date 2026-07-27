/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:16:46 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/06 19:42:07 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Character.hpp"
#include <string>

Character::Character() : _name("Default") {
  for (int i = 0; i < 4; i++) _inventory[i] = NULL;
}

Character::Character(std::string const& name) : _name(name) {
  for (int i = 0; i < 4; i++) _inventory[i] = NULL;
}

Character::Character(const Character& src) : _name(src._name) {
  // a throwing clone() must not leak the clones made before it
  int i = 0;
  try {
    for (; i < 4; i++)
      this->_inventory[i] =
          src._inventory[i] ? src._inventory[i]->clone() : NULL;  // DEEP COPY
  } catch (...) {
    while (i-- > 0) delete this->_inventory[i];
    throw;
  }
}

Character& Character::operator=(const Character& rhs) {
  if (this != &rhs) {
    this->_name = rhs._name;
    // clone ALL slots first: if one throws, our old inventory is untouched
    // (deleting first would leave freed pointers behind on a throw)
    AMateria* fresh[4];
    int i = 0;
    try {
      for (; i < 4; i++)
        fresh[i] = rhs._inventory[i] ? rhs._inventory[i]->clone() : NULL;
    } catch (...) {
      while (i-- > 0) delete fresh[i];
      throw;
    }
    for (int j = 0; j < 4; j++) {
      delete this->_inventory[j];  // DELETE OLD
      this->_inventory[j] = fresh[j];  // INSTALL DEEP COPY
    }
  }
  return *this;
}

Character::~Character() {
  for (int i = 0; i < 4; i++) {
    if (this->_inventory[i]) delete this->_inventory[i];
  }
}

std::string const& Character::getName() const { return this->_name; }

void Character::equip(AMateria* m) {
  if (!m) return;
  // refuse double ownership: equipping the same pointer twice would make
  // the destructor delete it twice
  for (int i = 0; i < 4; i++)
    if (this->_inventory[i] == m) return;
  for (int i = 0; i < 4; i++) {
    if (this->_inventory[i] == NULL) {
      this->_inventory[i] = m;
      return;
    }
  }
}

void Character::unequip(int idx) {
  if (idx >= 0 && idx < 4) {
    this->_inventory[idx] = NULL;  // DO NOT DELETE as per subject!
  }
}

void Character::use(int idx, ICharacter& target) {
  if (idx >= 0 && idx < 4 && this->_inventory[idx]) {
    this->_inventory[idx]->use(target);
  }
}
