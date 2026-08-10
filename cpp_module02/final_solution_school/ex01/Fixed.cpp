/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 18:08:29 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/03 15:31:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Fixed.hpp"

Fixed::Fixed() : _fixedPointValue(0) {
  std::cout << "Default constructor called" << std::endl;
}

/**
 * We convert an integer into fixed-point representation
 * 
 * n = 8; // 0000 1000
 * _fractionalBits = 8
 * 
 * shifting left by 8 bits means:
 * "move the integer part 8 bits to the left, and fill the right with zeros"
 * 0000 1000 << 8
 * ----------------
 * 1000 0000 0000
 * 
 * which is (8 * 2^8) = 2048
 * 
 * WHY: FIXED-point rule
 * real_value = raw / 256
 * 
 * so if we store 
 * raw = 2048 / 256 = 8.0
 * We get that which is perfect no precision loss.
 * 
 * so by using << 8
 * 
 * we basically `n * 256`
 * 
 * WHY: shifting left = multiplying
 * n << k = n * 2^k
 * so
 * 8 << 8 = 8 * 256
 * 
 * 
 * n << _fractionalBits scales an integer into fixed point format by moving it into the integer part
 * and reserving the lower bits for fractions
 * 
 */
Fixed::Fixed(const int n) : _fixedPointValue(n << _fractionalBits) {
  std::cout << "Int constructor called" << std::endl;
}

/**
 * This is the `float-to-fixed-point` 
 * f * (1 << _fractionalBits)
 * 
 * _fractionalBits = 8 -> 1 << 8 = 256
 * multiply `f` by 256
 * why because our fixed-point stores numbers as:
 * raw / 256 = real_number
 * 
 * so we need to convert `f`into the integer `raw vallue` that represent it
 * 
 * exaample f = 42.42
 * _fractionalBits = 8
 * 42.42 * 256 = 10855.52
 * This is the `scaled integer` vreesion of the float
 */
Fixed::Fixed(const float f)
    : _fixedPointValue(roundf(f * (1 << _fractionalBits))) {
  std::cout << "Float constructor called" << std::endl;
}

Fixed::Fixed(const Fixed& other) {
  std::cout << "Copy constructor called" << std::endl;
  *this = other;
}

/**
 * WHY: return *this
 * it allows `chaining assignment` this is a pattern
 * Fixed a, b, c;
 * a = b = c;
 * 1. Evaluate `b = c` first
 *  - `operator=` is called on `b` with `other = c`
 *  - copies the value
 *  - return `*this` -> reference to `b`
 * 2. Now the outer assignment `a = (b = c)` is evaluated
 *  - Essentially becomes `a = b`
 * `operator=` is called on a with `other = b`
 * copies value from b into a
 * 
 * without returning `*this`, chaining would break;
 * 
 * @return &Fixed we return a refernce, not a copy for efficiency
 * @note
 * if we returned by `value` (Fixed), a temporary object would be made every time
 * --> unnecesary copy --> slower, could trigger destructor, etc..
 */
Fixed& Fixed::operator=(const Fixed& other) {
  std::cout << "Copy assignment operator called" << std::endl;
  if (this != &other) this->_fixedPointValue = other.getRawBits();
  return (*this);
}

// nothing to clean up, just print message debug
Fixed::~Fixed() { std::cout << "Destructor called" << std::endl; }

/**
 * GETTER AND SETTER PAIR together encapsulate the member while giving
 * control access
 * 
 * GETTER != SINGLETON PATTERN
 */

// GETTER = standard C++ / OPP term for functions that expose private members values
int Fixed::getRawBits(void) const { return this->_fixedPointValue; }

// Setter = Set a value to the private member
void Fixed::setRawBits(int const raw) { this->_fixedPointValue = raw; }

/**
 * let's imagine we want to print the number or do calculation in normal floating point
 * _fixedPointValue = 2048
 * but the real number is 2048 / 256 = 8.0
 * so we need to `shrink` it back by dividing by the scaling factor `1 << fractionalBits = 256`);
 * 
 */
float Fixed::toFloat(void) const {
  return static_cast<float>(this->_fixedPointValue) / (1 << _fractionalBits);   // 8 * 1 << 8 = 256
}

// Standard right shift is usually okay, but casting to float
// first ensures the truncation behaves as expected for negatives
int Fixed::toInt(void) const { return static_cast<int>(this->toFloat()); }

/**
 * std::ostream already defines `operators` for primitive types (int, float, string,...)
 * inside our overload.
 * o << i.toFloat();
 * without this function, std::cout << a would not compile, because
 * `cout` doesn't know how to print `fixed`objects
 */
std::ostream& operator<<(std::ostream& o, Fixed const& i) {
  o << i.toFloat();
  return o;
}
