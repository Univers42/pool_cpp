/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScavTrap.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:18:34 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:18:36 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ScavTrap.hpp"

#include <iostream>
#include <string>

/* -------------------------------------------------------------------------- */
/* Orthodox Canonical Form                                                    */
/* -------------------------------------------------------------------------- */

ScavTrap::ScavTrap() : ClapTrap("Default_Scav") {
  this->_hitPoints = 100;
  this->_energyPoints = 50;
  this->_attackDamage = 20;
  std::cout << GREEN << "ScavTrap Default constructor called for " << _name
            << RESET << std::endl;
}

ScavTrap::ScavTrap(const ScavTrap& src) : ClapTrap(src) {
  std::cout << YELLOW << "ScavTrap Copy constructor called for " << _name
            << RESET << std::endl;
}

ScavTrap& ScavTrap::operator=(const ScavTrap& rhs) {
  std::cout << CYAN << "ScavTrap Copy assignment operator called for "
            << rhs._name << RESET << std::endl;
  if (this != &rhs) {
    ClapTrap::operator=(rhs);  // Call base class assignment operator
  }
  return *this;
}

ScavTrap::~ScavTrap() {
  std::cout << RED << "ScavTrap Destructor called for " << _name << RESET
            << std::endl;
}

/* -------------------------------------------------------------------------- */
/* Parameterized Constructor                                                  */
/* -------------------------------------------------------------------------- */

ScavTrap::ScavTrap(const std::string& name) : ClapTrap(name) {
  this->_hitPoints = 100;
  this->_energyPoints = 50;
  this->_attackDamage = 20;
  std::cout << GREEN << "ScavTrap Parameterized constructor called for "
            << _name << RESET << std::endl;
}

/* -------------------------------------------------------------------------- */
/* Member Functions                                                           */
/* -------------------------------------------------------------------------- */

void ScavTrap::attack(const std::string& target) {
  if (_hitPoints == 0) {
    std::cout << MAGENTA << "ScavTrap " << _name
              << " cannot attack because it is broken to pieces!" << RESET
              << std::endl;
    return;
  }
  if (_energyPoints == 0) {
    std::cout << MAGENTA << "ScavTrap " << _name
              << " cannot attack because its battery is completely drained!"
              << RESET << std::endl;
    return;
  }

  _energyPoints--;

  // ScavTrap has its own specific attack message
  std::cout << YELLOW << "ScavTrap " << _name << " aggressively strikes "
            << target << ", causing " << _attackDamage << " points of damage!"
            << RESET << std::endl;
}

void ScavTrap::guardGate() {
  if (_hitPoints == 0) {
    std::cout << MAGENTA << "ScavTrap " << _name
              << " cannot guard the gate because it is dead!" << RESET
              << std::endl;
    return;
  }
  std::cout << BLUE << "🛡️ ScavTrap " << _name << " is now in Gate keeper mode!"
            << RESET << std::endl;
}
