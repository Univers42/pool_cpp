/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClapTrap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:03:39 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/05 15:08:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE03_EX03_CLAPTRAP_HPP_
#define CPP_MODULE03_EX03_CLAPTRAP_HPP_

#include <iostream>
#include <string>

/**
 * VITAL BEST PRACTICE IN C++, whenever we are dealing with inheritance
 * we have to put the virtual ~ClapTrap()
 * 
 * Even if we leave attack, takeDamage, and beRepaired as non-virtual, adding
 * virtual to the destructor is the right move. Here is the "lower level"
 * explanation of why this saves our program from catastrophic leaks
 * 
 * The danger is deleting a child through a parent:
 * 
 * in c++, because of **upcasting**, we are allowed to create a diamondTrap
 * but store its memory address nside a Claptrap pointer.
 * 
 * ```cpp
 * ClapTrap* myRobt = new DiamondTrap("bob");
 * ```
 * 
 * The problem: when we are done with this robot and ccall `delete myRobot`
 * the compiler  looks at the pointer type (ClapTrap*)
 * - if the destructor is NOT virtual:: the compiler only calls ~(ClapTrap()).
 * The DiamondTrap, scavtrap and fragtrap layers are never destroyed. All the spec
 * memory of attributes they hold are leaked
 * if the destructor is virtual: The compiler looks at the actual object in memory.
 * It sees it is a `DiamondTrap`, so it calls `~DiamondTrap()` first, which then
 * safely chains down through the parents
 * 
 * Quoted C++
 *          "if a class has any virtual function, or if it is meant to be inherited from,
 *           its destructor MUST be virtual"
 * By adding `virtual ~ClapTrap()` we ensure taht the orthodo cannonical form is safe.
 * 
 * Virtual Inheritance: The subject explicitly notes that there is a "trick" to ensure 
 * the ClapTrap instance is created "once, and only once". By using virtual public ClapTrap,
 * we force the program to resolve the memory layout dynamically at runtime so 
 * FragTrap and ScavTrap share the exact same foundation.
 * 
 * Virtual Getter (getName): The refactor we did (virtual std::string getName() const;)
 * is pure dynamic polymorphism. When ScavTrap::attack() runs, it asks the v-table at runtime:
 * "Am I currently inside a ScavTrap or a DiamondTrap?" and fetches the correct name dynamically.
 * 
 * 
 * Virtual Destructor: virtual ~ClapTrap() ensures that if you delete a robot via a base pointer,
 * the program figures out at runtime exactly which child
 * destructors need to be fired to prevent memory leaks.
 * 
 * SUMMARIZE: the module 03 is primarily about inheritance and compile-time shadowing. However, Exercice
 * 03 forces us to use Dynamic Polymorphism (virtual inheritance) to solve the Diamond problem.. tye subtype
 * polymorphism (where all method are virtual) isn't the main focus until Module 04
 */
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
  virtual ~ClapTrap();

  explicit ClapTrap(const std::string& name);

  void attack(const std::string& target);
  void takeDamage(unsigned int amount);
  void beRepaired(unsigned int amount);

  virtual std::string getName() const;
  unsigned int getHitPoints() const;
  unsigned int getEnergyPoints() const;
  unsigned int getAttackDamage() const;
};

std::ostream& operator<<(std::ostream& o, const ClapTrap& i);

#endif  // CPP_MODULE03_EX03_CLAPTRAP_HPP_
