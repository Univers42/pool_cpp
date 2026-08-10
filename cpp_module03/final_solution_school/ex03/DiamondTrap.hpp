/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DiamondTrap.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:35:38 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/05 15:23:58 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE03_EX03_DIAMONDTRAP_HPP_
#define CPP_MODULE03_EX03_DIAMONDTRAP_HPP_

#include <iostream>
#include <string>

#include "FragTrap.hpp"
#include "ScavTrap.hpp"

/**
 * #MULTIPLE INHERITANCE
 * 
 * # The diamond Problem
 * 
 * # virtual inheritance (the "trick")
 * 
 * # name shadowing
 * The subject require `DiamonTrap` to have a very specific
 * setup for its names:
 * - DiamondTRap::_name: The raw name passed to the constructor
 * - ClapTrap::_name: The raw name plus a `_clap_name` suffix.
 * 
 * if we didn't inherit or refer back to the `claptrap` base, we 
 * wouldn't be able to reach that specific suffixed variable to  display it in our
 * `whoamI()` fnction
 * 
 * This subject is very strict: "The ClapTrap instance of DiamonTrap will be created once,
 * and only once". Listing `ClapTrap`in our virtual inheritance chain i sthe only way to ensure that
 * FragTrap and `ScavTrap` don't create two separate, conflicting robots inside our one DiamondTrap
 */
class DiamondTrap : public ScavTrap, public FragTrap {
 private:
  std::string _name;  // Shadows ClapTrap::_name

 public:
  // Orthodox Canonical Form
  DiamondTrap();
  DiamondTrap(const DiamondTrap& src);
  DiamondTrap& operator=(const DiamondTrap& rhs);
  ~DiamondTrap();

  // Parameterized Constructor
  explicit DiamondTrap(const std::string& name);

  std::string getName() const;  // Overriden to return DiamondTrap::_name
  // Expose ScavTrap's attack to resolve multiple inheritance ambiguity
  /**
   * using 'using' here is perfectly valid
   * and considered as a standard C++98 practice for resolving
   * name hiding and ambiguity in multiple inheritance
   * "the using namespace <ns_name> and friend keywords are forbidden"
   * they forbid to use namespace. it does not forbid a member using-declaration
   * technically and legally ouur code does not violate the usbject rule
   * 
   * According to the C++ Standard (ISO/IEC 14882:1998)
   * https://en.cppreference.com/w/cpp/language/using_declaration.html
   * 
   */
  using ScavTrap::attack;

  // Special ability
  void whoAmI();
};

#endif  // CPP_MODULE03_EX03_DIAMONDTRAP_HPP_
