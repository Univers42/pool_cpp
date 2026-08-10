/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClapTrap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:03:39 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/05 12:22:57 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE03_EX00_CLAPTRAP_HPP_
#define CPP_MODULE03_EX00_CLAPTRAP_HPP_

/**
 * https://en.cppreference.com/w/cpp/language/explicit.html
 * https://en.cppreference.com/w/cpp/language/destructor.html
 * https://en.cppreference.com/w/cpp/language/copy_elision.html
 * 
 * PREPROC: Guard (#ifndef, #define, #endif)
 * This is a Preprocessor Directive
 * it prevents "Double inclusion", if we #include this file twice in different
 * parts of our program, the compiler will  freak out because we're trying to 
 * define the same class twice
 * The preprocessor checks if CPP_MODULE03_EX00_CLAPTRAP_HPP_ is defined.
 * if not, it defines it and processes the file. If it is already defined, it skips
 * everything until `#endif`
 * https://www.learncpp.com/cpp-tutorial/header-guards/
 * https://en.cppreference.com/w/cpp/preprocessor.html
 * 
 * 
 * ACCESSORS: `private vs public`
 * This is the core encapsulation
 * - private: These member (attributes) can only be accessed by code inside the
 * `ClapTRap` class. This protects our data. We don't want a random function to set
 * _hitPoints to -9999; we want the class to control via methods
 * public is an interface. this is hw  the outside worldj interacts with our
 * object
 * https://en.cppreference.com/w/cpp/language/access.html
 * 
 * OCCF: Orthodoc canonical Class form or rules of three
 * to make a class "Well-behaved" in C++, we must implement these four specifi methods.
 * if we don't, C++ will try to write them for us, whichh often leds to memory bugs
 * 
 * https://en.cppreference.com/w/cpp/language/rule_of_three.html
 * https://www.learncpp.com/cpp-tutorial/introduction-to-the-copy-constructor/
 * https://www.learncpp.com/cpp-tutorial/overloading-the-assignment-operator/
 * 
 * EXPLICIT: we see this on parameterized constructor
 * The problem is that without `explicit`, C++ might try to "help" us by converting a string
 * into a `ClapTrap` automatically (implicit conversion)
 * Fix: explicit tells the compiler: "No magic tricks. if the user wants a clapTrap
 * they must call the constructor specifically"
 * https://en.cppreference.com/w/cpp/language/explicit.html
 * 
 * PASS-BY-REFERENCE-TO-CONST: (const std::string& name)
 * This is a major perfomrnace optimization.
 * PASS-BY-VALUE: if we used `(std::srtring name)`, the computer would copy the entire
 * string into a new memory location. This is slow.
 * PASS-BY-REFERENCE(&): we are passing a `pointer-alias` to the original string. 
 * No copy is made
 * const: since we are passing the original, we `add`const to compromise: "I will look at this string
 * but I won't change it"
 * https://en.cppreference.com/w/cpp/language/reference_initialization.html
 * https://www.learncpp.com/cpp-tutorial/pass-by-const-lvalue-reference/
 * https://www.learncpp.com/cpp-tutorial/const-class-objects-and-const-member-functions/
 * 
 * 
 * CONSTANT-MEMBER-FUNCTION: (getName() const)
 * @note: notice the `const` at the end of the getters. This means that this function will not modify
 * any member variables of the class. 
 * GOAL: it's needed because we we have a `const ClapTrap myRobot`, we can only call functions that
 * are marked `const`. If we forget the `const`on getHitPoints(), the compiler will refuse to let a
 * `constant` robot tell you how muchh health it has
 * https://www.learncpp.com/cpp-tutorial/const-class-objects-and-const-member-functions/
 * 
 * 
 * 
 * OPERATOR-OVERLOADING: std::ostream& operator<<
 * This allows us to treat our class like a built-in type (like an `int`)
 * - The Goal: instead of writing std::cout << myRobot.getName() << ..., we want to just
 * write `std::cout`, not ClapTRap
 * https://en.cppreference.com/w/cpp/language/operators.html
 * https://www.learncpp.com/cpp-tutorial/overloading-the-io-operators/
 * 
 * 
 * DATA-MEMBER-NAMING-CONVENTION: _name
 * The underscore prefix (or suffix) is common convention to distinguish between `member variables`
 * (which liives as long as the object lives) and `local variables/parameters` (which die when the 
 * funciton ends). It prevents "shadowing" bugs where we accidentally use a parameter instead of the
 * class attribute
 * https://en.cppreference.com/w/cpp/language/initializer_list.html
 * https://www.learncpp.com/cpp-tutorial/constructor-member-initializer-lists/
 * 
 * 
*/
#include <iostream>
#include <string>

// Color Macros
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"

class ClapTrap {
 private:
  std::string _name;
  unsigned int _hitPoints;
  unsigned int _energyPoints;
  unsigned int _attackDamage;

 public:
  // Orthodox Canonical Form
  ClapTrap();
  ClapTrap(const ClapTrap& src);
  ClapTrap& operator=(const ClapTrap& rhs);
  ~ClapTrap();

  // Parameterized Constructor
  explicit ClapTrap(const std::string& name);

  // Subject Mandatory function
  void attack(const std::string& target);
  void takeDamage(unsigned int amount);
  void beRepaired(unsigned int amount);

  // Getters
  /**
   * We could write without using `const` member funciton as llong as we never
   * use a constant OBJECT
   * BUT THIS IS A MAJOR DESIGN FLAW
   * The biggest flaw here is that our class becomes incompatible with "constant object"
   *  or "reference to constants"
   * 
   * if we don't use the `const` keyword on `getHitPoints()`, the compiler assumes that
   * the function might change the object. therefore, it forbids us from calling it on
   * a `const` object
   * 
   * EXAMPLE: 
   * 
   * ```
   * void printStatus(const ClapTrap& robot) {
   *  std::cout << robot.getHitPoints() << std::endl;   // this will FAIL TO COMPILE if getHitPoints() is not marked const
   * }
   * 
   * int  main() {
   * const   ClapTrap boss("Mega-bot"); // this will FAIL TO COMPILE if getName() is not marked const
   * }
   * 
   * WHY: beccause the commpiler says: "you promised me `boss` and `robot` would never change (const)
   * but you are trying to call a function that hasn't promised to stay away from my data"
   * 
   * IN: c++ "Getters" are logically supposed to be "read-only"
   * ```
   */
  std::string getName() const;
  unsigned int getHitPoints() const;
  unsigned int getEnergyPoints() const;
  unsigned int getAttackDamage() const;
};

// Stream Overload for easy printing
std::ostream& operator<<(std::ostream& o, const ClapTrap& i);

#endif  // CPP_MODULE03_EX00_CLAPTRAP_HPP_
