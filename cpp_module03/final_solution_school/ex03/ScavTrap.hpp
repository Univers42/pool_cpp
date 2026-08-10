/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScavTrap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:12:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:40:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE03_EX03_SCAVTRAP_HPP_
#define CPP_MODULE03_EX03_SCAVTRAP_HPP_

#include <iostream>
#include <string>

#include "ClapTrap.hpp"

// NOTICE THE 'virtual' KEYWORD HERE!
class ScavTrap : virtual public ClapTrap {
 public:
  // Orthodox Canonical Form
  ScavTrap();
  ScavTrap(const ScavTrap& src);
  ScavTrap& operator=(const ScavTrap& rhs);
  ~ScavTrap();

  // Parameterized Constructor
  explicit ScavTrap(const std::string& name);

  // Overridden attack function
  void attack(const std::string& target);

  // Special ability
  void guardGate();
};

#endif  // CPP_MODULE03_EX03_SCAVTRAP_HPP_
