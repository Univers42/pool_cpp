/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClapTrap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:03:39 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:42:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE03_EX02_CLAPTRAP_HPP_
#define CPP_MODULE03_EX02_CLAPTRAP_HPP_

#include <iostream>
#include <string>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"

class ClapTrap {
 protected:  // CHANGED FROM PRIVATE TO PROTECTED
  std::string _name;
  unsigned int _hitPoints;
  unsigned int _energyPoints;
  unsigned int _attackDamage;

 public:
  ClapTrap();
  ClapTrap(const ClapTrap& src);
  ClapTrap& operator=(const ClapTrap& rhs);
  ~ClapTrap();

  explicit ClapTrap(const std::string& name);

  void attack(const std::string& target);
  void takeDamage(unsigned int amount);
  void beRepaired(unsigned int amount);

  std::string getName() const;
  unsigned int getHitPoints() const;
  unsigned int getEnergyPoints() const;
  unsigned int getAttackDamage() const;
};

std::ostream& operator<<(std::ostream& o, const ClapTrap& i);

#endif  // CPP_MODULE03_EX02_CLAPTRAP_HPP_
