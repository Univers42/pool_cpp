/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FragTrap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:27:25 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/05 13:40:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE03_EX02_FRAGTRAP_HPP_
#define CPP_MODULE03_EX02_FRAGTRAP_HPP_

#include <iostream>
#include <string>

#include "ClapTrap.hpp"

/**
 * HIERARCHICAL_INHERITANCE: (SIBLING RELATIONSHIP)
 * We are now creating a second class taht inherits from ClapTrap
 * Concept: both `ScavTrap` and `Fragtrap` are children of ClapTrap. They are
 * siblings.
 * significance: This  demonstrates that one base class can support multiple
 * different derived classes, each with its own specific behavior and stats
 * 
 */
class FragTrap : public ClapTrap {
 public:
  // Orthodox Canonical Form
  FragTrap();
  FragTrap(const FragTrap& src);
  FragTrap& operator=(const FragTrap& rhs);
  ~FragTrap();

  // Parameterized Constructor
  explicit FragTrap(const std::string& name);

  // Special ability
  void highFivesGuys(void);
};

#endif  // CPP_MODULE03_EX02_FRAGTRAP_HPP_
