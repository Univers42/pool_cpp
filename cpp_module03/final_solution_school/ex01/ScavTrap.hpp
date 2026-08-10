/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScavTrap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:12:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/05 14:26:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE03_EX01_SCAVTRAP_HPP_
#define CPP_MODULE03_EX01_SCAVTRAP_HPP_

#include <iostream>
#include <string>

#include "ClapTrap.hpp"
/**
 * INHERITANCE: class ScavTrap : public ClapTrap
 * The "ls-A" Relationship: by inheiriting publicly, ScavTrap gets everything
 * ClapTrap has (attributes and methods)
 * Code Reuse: we don't have to redefine `_name` or `takeDamage()`
 * ScavTrap already knows how to do those things because its "parent" does
 * the public modifier: Using **public** inheritance means that the public
 * members of ClapTrap stay public in Scavtrap.
 * 
 * PROTECT VS PRIVATE: in ex00 we made our variables private
 * the cconclict if ClapTRap attributes `private`, Scavtrap cannot touch
 * them directly (even though it "owns" them)
 * - The solution: we need to learn about the protected access specifier.
 *  - `private`: only the class itself can see it
 *  - `protected`: The class and its children (like Scavtrap) can see it
 * -  `public`: Everyone can see it
 * 
 * CHAIN: constructor destructor
 * Thhe order of creation: When we create a ScavTrap, the computer first builds
 * a `ClapTrap`, then wraps the `ScavTrap` layers around it.
 * the order of destruction: When it dies, it's the reverse: the `scavTrap`
 * layer is destroyed first, then the `ClapTrap` layer.
 * The syntax: in our `ScavTrap.cpp`,  we will use the initializer list
 * to tell the parent how to build itself.
 * 
 * ```cpp
 * ScavTrap::ScavTrap(std::string name) : ClapTrap(name){}
 * ```
 * 
 * 
 * OVERRIDE: FUNCTION OVERRIDING which is a static polymorphism
 * in C++, having the samme function anme in both a parent (ClapTrap) and a chilld (ScavTrap)
 * does not cause a shadow "Error" or compilation failure because of a concept function overriding
 * here is how the compiler handles this "lower level" name overlap
 * 
 * When we define `attack()` in ScavTrap we are telling the compiler taht this new version should
 * take priority for any `ScavTrap` object
 * 
 * - The rule: in C++, name in a **derived class** (the child) hide names in the base class
 * if they have the same name.
 * 
 * The search order: When we call `robot.attack()`, the compiler first looks inside
 * the `scavTrap` scope. Siince it finds an `attack()`  fnction there. it stops searching and uses it.
 * it never even reaches the ClapTRap version
 * 
 * SHADOWING-SCOPE:
 * Since ScavTrap is a ClapTrap, it has access to its parent's functions.
 * if we wanted to call the original `ClapTrap` attack fromm inside a `ScavTrap`
 * function, we would use the `Scope resolution Operator`
 * SPECIFIC_MEMBER_VALUES:
 * 
 * 
 * UPCASTING: ScavTrap::ScavTrap(const ScavTrap& src) : ClapTrap(src)
 * ClapTRap accepts src when src is a ScavTrap.This works beccasue of this rule called upcasting:
 * a ScavTrap is a `clapTrap`
 * therefore, any function that asks for a `claptrap` (like the claptrap copy constructor)
 * will happily accept a `ScavTrap`. It simply ingores the "ScavTrap-only" parts (like guardGate)
 * and copies the part it knows about (_name, _hitPoints, etc.)
 * 
 * ORDER: c++ has very strict rule: we cannot build a house until we lay the foundations
 * the list stage: before the `{ }` fo Scavtrap ever runs, C++ looks at our initializer list.
 * PARENT_FIRST: it sees `: ClapTrap(src)`. It jumps to the `ClapTrap` Copy Constructor and copies
 * the name 
 * 
 * 
 * 
 */
class ScavTrap : public ClapTrap {
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

#endif  // CPP_MODULE03_EX01_SCAVTRAP_HPP_
