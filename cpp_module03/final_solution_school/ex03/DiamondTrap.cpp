/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DiamondTrap.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:35:48 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/05 14:48:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DiamondTrap.hpp"

#include <iostream>
#include <string>

/* -------------------------------------------------------------------------- */
/* Orthodox Canonical Form                                                    */
/* -------------------------------------------------------------------------- */

DiamondTrap::DiamondTrap()
    : ClapTrap("Default_clap_name"), ScavTrap(), FragTrap(), _name("Default") {
  this->_hitPoints = 100;    // FragTrap HP
  this->_energyPoints = 50;  // ScavTrap EP
  this->_attackDamage = 30;  // FragTrap AD
  std::cout << GREEN << "DiamondTrap Default constructor called for " << _name
            << RESET << std::endl;
}

DiamondTrap::DiamondTrap(const DiamondTrap& src)
    : ClapTrap(src), ScavTrap(src), FragTrap(src), _name(src._name) {
  std::cout << YELLOW << "DiamondTrap Copy constructor called for " << _name
            << RESET << std::endl;
}

DiamondTrap& DiamondTrap::operator=(const DiamondTrap& rhs) {
  std::cout << CYAN << "DiamondTrap Copy assignment operator called for "
            << rhs._name << RESET << std::endl;
  if (this != &rhs) {
    ClapTrap::operator=(rhs);
    this->_name = rhs._name;
  }
  return *this;
}

DiamondTrap::~DiamondTrap() {
  std::cout << RED << "DiamondTrap Destructor called for " << _name << RESET
            << std::endl;
}

/* -------------------------------------------------------------------------- */
/* Parameterized Constructor                                                  */
/* -------------------------------------------------------------------------- */

/**
 * The order is extremely important because it dictates the sequence in which
 * our object is constructed and how the compiler resolve naming conflicts
 * SHARED_FOUNDATION: RULE 
 * in a normal (non-virtual) inheritance, DiamondTRap would indeed have
 * two separate "ClapTrap" parts --one inside is `FragTrap` side and one inside
 * its `Scavtrap` side
 * 
 * By using **Virtual inheritance**, we are telling the computer: "all
 * paths to Claptrap must lead to the exact same memory address"
 * 
 * ## The responsibility f the "most derived" class
 * in C++, there is a very specific rule for virtual inheritance: The class
 * at the very bottom (the "Most Derived" class) is the only one allowed
 * to initialize the virtual base.
 * 
 * The problem::  if `fragtrap` tried to initialize `ClapTrap` and `ScavTrap`
 * also tried to initialize it, they might provide different nae of valuues
 * The solution: The compiler ignnores teh calls to `ClapTrap` from the
 * parents and forces `DiamondTRap` to call the `ClapTrap` constructor
 * directly
 */
DiamondTrap::DiamondTrap(const std::string& name)
    : ClapTrap(name + "_clap_name"),
      ScavTrap(name),
      FragTrap(name),
      _name(name) {
  this->_hitPoints = 100;    // FragTrap HP
  this->_energyPoints = 50;  // ScavTrap EP
  this->_attackDamage = 30;  // FragTrap AD
  std::cout << GREEN << "DiamondTrap Parameterized constructor called for "
            << _name << RESET << std::endl;
}

std::string DiamondTrap::getName() const {
  return  (this->_name);
}

/* -------------------------------------------------------------------------- */
/* Member Functions                                                           */
/* -------------------------------------------------------------------------- */

void DiamondTrap::whoAmI() {
  std::cout << BLUE << "🦇 I am DiamondTrap " << _name << "!"
            << " But my ClapTrap core is known as " << ClapTrap::_name << "!"
            << RESET << std::endl;
}
