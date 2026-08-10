/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:16:46 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 17:22:37 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Character.hpp"
#include <string>
#include <iostream>

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
    if (this == &rhs) {
#ifdef DEBUG
        std::cerr << "[Character] Self-assignment detected.\n";
#endif
        return *this;
    }

#ifdef DEBUG
    std::cerr << "\n========== Character::operator= ==========\n";
    std::cerr << "Destination : " << this << " (" << this->_name << ")\n";
    std::cerr << "Source      : " << &rhs << " (" << rhs._name << ")\n";
#endif

    this->_name = rhs._name;

    AMateria* fresh[4];
    int i = 0;

    try {
        for (; i < 4; i++) {
#ifdef DEBUG
            std::cerr << "Cloning slot [" << i << "]... ";
#endif

            if (rhs._inventory[i]) {
                fresh[i] = rhs._inventory[i]->clone();
#ifdef DEBUG
                std::cerr << "OK ("
                          << rhs._inventory[i]->getType()
                          << ") new ptr=" << fresh[i] << '\n';
#endif
            } else {
                fresh[i] = NULL;
#ifdef DEBUG
                std::cerr << "EMPTY\n";
#endif
            }
        }
    }
    catch (...) {
#ifdef DEBUG
        std::cerr << "Exception while cloning!\n";
        std::cerr << "Cleaning already cloned Materias...\n";
#endif

        while (i-- > 0) {
#ifdef DEBUG
            std::cerr << "Deleting cloned slot [" << i << "] "
                      << fresh[i] << '\n';
#endif
            delete fresh[i];
        }

        throw;
    }

#ifdef DEBUG
    std::cerr << "\nReplacing old inventory...\n";
#endif

    for (int j = 0; j < 4; j++) {

#ifdef DEBUG
        std::cerr << "Slot [" << j << "] : ";

        if (_inventory[j])
            std::cerr << "delete old (" << _inventory[j]->getType()
                      << ") ptr=" << _inventory[j];
        else
            std::cerr << "old EMPTY";

        std::cerr << " --> ";
#endif

        delete _inventory[j];
        _inventory[j] = fresh[j];

#ifdef DEBUG
        if (_inventory[j])
            std::cerr << "new (" << _inventory[j]->getType()
                      << ") ptr=" << _inventory[j];
        else
            std::cerr << "EMPTY";

        std::cerr << '\n';
#endif
    }

#ifdef DEBUG
    std::cerr << "Assignment complete.\n";
    std::cerr << "==========================================\n\n";
#endif

    return *this;
}


Character::~Character() {
  for (int i = 0; i < 4; i++) {
    if (this->_inventory[i]) delete this->_inventory[i];
  }
}

std::string const& Character::getName() const { return this->_name; }

void Character::equip(AMateria* m) {
  if (!m){
    #ifdef DEBUG
      std::cerr << "[CHARACTER] Cannot equip a NULL materia." << std::endl;
    #endif
    return ;
    
  }

  for (int i = 0; i < 4; ++i) {
    if (_inventory[i] == m){
      #ifdef DEBUG
        std::cerr << "[CHARACTER] Materia already equipped." << std::endl;
      #endif
      return ;
    }
  }

    for (int i = 0; i < 4; ++i) {
    if (_inventory[i] == NULL) {
      _inventory[i] = m;
      return ;
    }
  }
}

void Character::unequip(int idx)
{
    if (idx < 0 || idx >= 4)
    {
#ifdef DEBUG
        std::cout << "[Character::unequip] Invalid index "
                  << idx << ". Valid range is [0,3].\n";
#endif
        return;
    }

    if (!_inventory[idx])
    {
#ifdef DEBUG
        std::cout << "[Character::unequip] Slot "
                  << idx << " is already empty.\n";
#endif
        return;
    }

#ifdef DEBUG
    std::cout << "[Character::unequip] Unequipping "
              << _inventory[idx]->getType()
              << " from slot "
              << idx << ".\n";
#endif

    _inventory[idx] = NULL;
}

void Character::use(int idx, ICharacter& target)
{
    if (idx < 0 || idx >= 4)
    {
#ifdef DEBUG
        std::cout << "[Character::use] Invalid index "
                  << idx << ". Valid range is [0,3].\n";
#endif
        return;
    }

    if (!_inventory[idx])
    {
#ifdef DEBUG
        std::cout << "[Character::use] Slot "
                  << idx << " is empty.\n";
#endif
        return;
    }

#ifdef DEBUG
    std::cout << "[Character::use] Using "
              << _inventory[idx]->getType()
              << " from slot "
              << idx << ".\n";
#endif

    _inventory[idx]->use(target);
}

void Character::printInventory() const {
#ifdef DEBUG
    std::cout << "\nCharacter \"" << _name << "\" inventory\n";
    std::cout << "--------------------------------\n";

    for (int i = 0; i < 4; i++) {
        std::cout << "[" << i << "] ";

        if (_inventory[i])
            std::cout << _inventory[i]->getType()
                      << " (" << _inventory[i] << ")";
        else
            std::cout << "EMPTY";

        std::cout << '\n';
    }

    std::cout << std::endl;
#endif
return ;
}