/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FragTrap.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:29:54 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:31:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FragTrap.hpp"

#include <iostream>
#include <string>

/* -------------------------------------------------------------------------- */
/* Orthodox Canonical Form                                                    */
/* -------------------------------------------------------------------------- */

FragTrap::FragTrap() : ClapTrap("Default_Frag") {
  this->_hitPoints = 100;
  this->_energyPoints = 100;
  this->_attackDamage = 30;
  std::cout << GREEN << "FragTrap Default constructor called for " << _name
            << RESET << std::endl;
}

FragTrap::FragTrap(const FragTrap& src) : ClapTrap(src) {
  std::cout << YELLOW << "FragTrap Copy constructor called for " << _name
            << RESET << std::endl;
}

FragTrap& FragTrap::operator=(const FragTrap& rhs) {
  std::cout << CYAN << "FragTrap Copy assignment operator called for "
            << rhs._name << RESET << std::endl;
  if (this != &rhs) {
    ClapTrap::operator=(rhs);  // Call base class assignment operator
  }
  return *this;
}

FragTrap::~FragTrap() {
  std::cout << RED << "FragTrap Destructor called for " << _name << RESET
            << std::endl;
}

/* -------------------------------------------------------------------------- */
/* Parameterized Constructor                                                  */
/* -------------------------------------------------------------------------- */

FragTrap::FragTrap(const std::string& name) : ClapTrap(name) {
  this->_hitPoints = 100;
  this->_energyPoints = 100;
  this->_attackDamage = 30;
  std::cout << GREEN << "FragTrap Parameterized constructor called for "
            << _name << RESET << std::endl;
}

/* -------------------------------------------------------------------------- */
/* Member Functions                                                           */
/* -------------------------------------------------------------------------- */

void FragTrap::highFivesGuys(void) {
  if (_hitPoints == 0) {
    std::cout << MAGENTA << "FragTrap " << _name
              << " cannot high five because it is dead!" << RESET << std::endl;
    return;
  }
  std::cout << BLUE << "🙌 FragTrap " << _name
            << " enthusiastically asks everyone for a high five! UP TOP!"
            << RESET << std::endl;
}
