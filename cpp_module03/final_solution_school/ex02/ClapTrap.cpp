#include "ClapTrap.hpp"

#include <iostream>
#include <string>

/* -------------------------------------------------------------------------- */
/* Orthodox Canonical Form                                                    */
/* -------------------------------------------------------------------------- */

ClapTrap::ClapTrap()
    : _name("Default"), _hitPoints(10), _energyPoints(10), _attackDamage(0) {
  std::cout << GREEN << "ClapTrap Default constructor called for " << _name
            << RESET << std::endl;
}

ClapTrap::ClapTrap(const ClapTrap& src)
    : _name(src._name),
      _hitPoints(src._hitPoints),
      _energyPoints(src._energyPoints),
      _attackDamage(src._attackDamage) {
  std::cout << YELLOW << "ClapTrap Copy constructor called for " << _name
            << RESET << std::endl;
}

ClapTrap& ClapTrap::operator=(const ClapTrap& rhs) {
  std::cout << CYAN << "ClapTrap Copy assignment operator called for "
            << rhs._name << RESET << std::endl;
  if (this != &rhs) {
    this->_name = rhs._name;
    this->_hitPoints = rhs._hitPoints;
    this->_energyPoints = rhs._energyPoints;
    this->_attackDamage = rhs._attackDamage;
  }
  return *this;
}

ClapTrap::~ClapTrap() {
  std::cout << RED << "ClapTrap Destructor called for " << _name << RESET
            << std::endl;
}

/* -------------------------------------------------------------------------- */
/* Parameterized Constructor                                                  */
/* -------------------------------------------------------------------------- */

ClapTrap::ClapTrap(const std::string& name)
    : _name(name), _hitPoints(10), _energyPoints(10), _attackDamage(0) {
  std::cout << GREEN << "ClapTrap Parameterized constructor called for "
            << _name << RESET << std::endl;
}

/* -------------------------------------------------------------------------- */
/* Member Functions                                                           */
/* -------------------------------------------------------------------------- */

void ClapTrap::attack(const std::string& target) {
  // Cannot do anything if dead or exhausted
  if (_hitPoints == 0) {
    std::cout << MAGENTA << "ClapTrap " << _name
              << " cannot attack because it has no hit points left!" << RESET
              << std::endl;
    return;
  }
  if (_energyPoints == 0) {
    std::cout << MAGENTA << "ClapTrap " << _name
              << " cannot attack because it has no energy points left!" << RESET
              << std::endl;
    return;
  }

  _energyPoints--;

  // Required output format
  std::cout << YELLOW << "ClapTrap " << _name << " attacks " << target
            << ", causing " << _attackDamage << " points of damage!" << RESET
            << std::endl;
}

void ClapTrap::takeDamage(unsigned int amount) {
  if (_hitPoints == 0) {
    std::cout << BLUE << "ClapTrap " << _name
              << " is already dead and cannot take more damage!" << RESET
              << std::endl;
    return;
  }

  // Prevent unsigned integer underflow
  if (amount >= _hitPoints) {
    _hitPoints = 0;
  } else {
    _hitPoints -= amount;
  }

  std::cout << RED << "ClapTrap " << _name << " takes " << amount
            << " points of damage! Hit points left: " << _hitPoints << RESET
            << std::endl;
}

void ClapTrap::beRepaired(unsigned int amount) {
  if (_hitPoints == 0) {
    std::cout << MAGENTA << "ClapTrap " << _name
              << " cannot be repaired because it has no hit points left!"
              << RESET << std::endl;
    return;
  }
  if (_energyPoints == 0) {
    std::cout << MAGENTA << "ClapTrap " << _name
              << " cannot be repaired because it has no energy points left!"
              << RESET << std::endl;
    return;
  }

  _energyPoints--;
  _hitPoints += amount;

  std::cout << CYAN << "ClapTrap " << _name << " repairs itself, regaining "
            << amount << " hit points! Hit points left: " << _hitPoints << RESET
            << std::endl;
}

/* -------------------------------------------------------------------------- */
/* Getters                                                                    */
/* -------------------------------------------------------------------------- */

std::string ClapTrap::getName() const { return _name; }
unsigned int ClapTrap::getHitPoints() const { return _hitPoints; }
unsigned int ClapTrap::getEnergyPoints() const { return _energyPoints; }
unsigned int ClapTrap::getAttackDamage() const { return _attackDamage; }

/* -------------------------------------------------------------------------- */
/* Stream Overload                                                            */
/* -------------------------------------------------------------------------- */

std::ostream& operator<<(std::ostream& o, const ClapTrap& i) {
  o << "ClapTrap[" << i.getName() << " | HP: " << i.getHitPoints()
    << " | EP: " << i.getEnergyPoints() << " | AD: " << i.getAttackDamage()
    << "]";
  return o;
}
