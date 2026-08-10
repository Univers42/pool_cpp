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
  for (int i = 0; i < 4; i++) {
    if (src._inventory[i])
      this->_inventory[i] = src._inventory[i]->clone();  // DEEP COPY
    else
      this->_inventory[i] = NULL;
  }
}

Character& Character::operator=(const Character& rhs) {
  if (this != &rhs) {
    this->_name = rhs._name;
    for (int i = 0; i < 4; i++) {
      if (this->_inventory[i]) delete this->_inventory[i];  // DELETE OLD
      if (rhs._inventory[i])
        this->_inventory[i] = rhs._inventory[i]->clone();  // DEEP COPY NEW
      else
        this->_inventory[i] = NULL;
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
  for (int i = 0; i < 5; i++) {
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
