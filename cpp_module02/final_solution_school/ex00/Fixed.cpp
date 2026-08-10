/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:54:34 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/04 20:44:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Fixed.hpp"

/**
 * @file Fixed.cpp
 * @brief Implementation of the Fixed class — Orthodox Canonical Form walkthrough.
 *
 * @details
 * **Why do we obsess over Canonical Form?**
 *
 * Later we will manage heap memory, build containers and write real C++ libraries.
 * If we do not understand object lifetime our program will:
 *  - segfault
 *  - double-free
 *  - leak memory
 *
 * **Why not just call the default constructor instead of a copy constructor?**
 *
 * Because the compiler is not asking the same question:
 *  - Default constructor → "how do I create an object from *nothing*?"
 *  - Copy constructor    → "how do I create an object from an *existing* object?"
 *
 * The data may look identical; the *source* is fundamentally different.
 * @code
 *   Fixed a;              // stack — default constructor
 *   Fixed b(a);           // stack — copy constructor
 *   Fixed* p = new Fixed(a); // heap — copy constructor (same ctor, different allocator)
 * @endcode
 */

#include <iostream>
#include "Fixed.hpp"

// Terminal colour codes — educational output only, not logic
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define DIM     "\033[2m"

/**
 * @brief Default constructor.
 *
 * @details
 * Initialises @c _fixedPointValue to 0 via the member-initialiser list.
 * Without `: _fixedPointValue(0)` the int would hold garbage — whatever
 * bytes happened to be at that stack address at that moment.
 * Always initialise explicitly: objects must start in a known, valid state.
 */
Fixed::Fixed() : _fixedPointValue(0) {
  std::cout << GREEN << "Default constructor called" << RESET << std::endl;
}

/**
 * @brief Copy constructor — produces an independent clone of @p other.
 *
 * @param other  Source object to copy.
 *
 * @details
 * Delegates actual copying to @c operator= so the copy logic lives in
 * exactly one place.  If you later change how values are copied you only
 * touch @c operator=.
 *
 * Memory layout after `Fixed b(a)` — both live on the stack, no sharing:
 * @code
 *   Stack
 *   ┌─────────────────────────────────┐
 *   │  a._fixedPointValue = 10752     │  ← original  (42 << 8)
 *   │  b._fixedPointValue = 10752     │  ← independent copy
 *   └─────────────────────────────────┘
 *   Changing b does NOT affect a.
 * @endcode
 *
 * If the member were a pointer this is where you would deep-copy the data:
 * @code
 *   _raw = new int(*other._raw);  // heap deep-copy, NOT: _raw = other._raw
 * @endcode
 */
Fixed::Fixed(const Fixed& other) : _fixedPointValue(0) {
  std::cout << YELLOW << "Copy constructor called" << RESET << std::endl;
  *this = other;  // delegate — one source of truth for copy logic
}

/**
 * @brief Copy assignment operator — overwrites an already-existing object.
 *
 * @param other  Source object to copy from.
 * @return       @c *this, enabling chained assignment (@c a = b = c).
 *
 * @details
 * Unlike the copy constructor the destination object *already exists* and
 * may hold resources that need to be released before overwriting.
 *
 * **Self-assignment guard** — `if (this != &other)` is not optional:
 * @code
 *   Fixed a;
 *   a = a;  // without the guard: might free a's memory then read it → UB
 * @endcode
 *
 * We read @c other._fixedPointValue directly instead of going through
 * @c getRawBits() to avoid the verbose debug print on every assignment.
 */
Fixed& Fixed::operator=(const Fixed& other) {
  std::cout << CYAN << "Copy assignment operator called" << RESET << std::endl;
  if (this != &other)
    this->_fixedPointValue = other._fixedPointValue;
  return *this;
}

/**
 * @brief Destructor.
 *
 * @details
 * Called automatically when the object leaves scope (stack) or is @c delete'd
 * (heap).  @c Fixed only holds a plain @c int so there is nothing to free.
 * If @c _fixedPointValue were an @c int* you would write:
 * @code
 *   delete _fixedPointValue;
 * @endcode
 */
Fixed::~Fixed() {
  std::cout << RED << "Destructor called" << RESET << std::endl;
}

/**
 * @brief Returns the raw internal integer (value × 256, not divided back).
 * @return Raw fixed-point bits, e.g. logical 42.0 → 10752.
 */
int Fixed::getRawBits(void) const {
  std::cout << DIM << "  getRawBits member function called" << RESET << std::endl;
  return this->_fixedPointValue;
}

/**
 * @brief Directly sets the raw internal integer (no scaling applied).
 * @param raw  Raw value to store.
 */
void Fixed::setRawBits(int const raw) {
  this->_fixedPointValue = raw;
}
