/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 18:08:24 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/03 15:20:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE02_EX01_FIXED_HPP_
#define CPP_MODULE02_EX01_FIXED_HPP_
#include <cmath>
#include <iostream>

class Fixed {
 private:
  int _fixedPointValue;                 // 32-bit int
  static const int _fractionalBits = 8;

 public:
 // Default constructor ; called when we write Fixed a;
 // compute object with default value which is 0
  Fixed();
  /**
   * DEEPCOPY: called when we create a new object from an exisitiing one
   * 
   * ```cpp
   * Fixed b(a);  // copy constructor
   * Fixed c = a; // also copy constructor
   * ```
   * 
   * clone `this` existing object into a brand-new one
   * Very important for `deep copy situations` dynamic memory
   * 
   */
  Fixed(const Fixed& other);
  /**
   * COPYASS: copy assignment operator
   * Fixed& operator=(const Fixed& ohter);
   * - Called when both objects already exist, and we want to overwrite one with another
   * ```cpp
   * Fixed a;
   * Fixed b;
   * b = a;
   * ```
   * b already exists -> copy constructor  is NOT called here
   */
  Fixed& operator=(const Fixed& other);
  ~Fixed();
  /**
   * INT-CONSTRUCTOR:
   * called when we create a fixed-point object from an integer:
   * 
   * Fixed a(42);
   * converts `42` into our `_fixedPointValue` format using:
   * ```cpp
   * _fixedPointValue = n << fractionalBits;
   * ```
   * 
   */
  explicit  Fixed(const int n);
  /**
   * FLOAT-CONSTRUCTOR:
   * called when we want to create a fixed-point object from a flat
   * Fixed b(3.14f);
   * converts 3.14 into fixedPointValue using scaling and rounding:
   * 
   * ```
   * _fixedPointValue = roundf(f * (1 << _fractionalBits));
   * ```
   */
  explicit  Fixed(const float f);

  // METHODS
  int   getRawBits(void) const;
  void  setRawBits(int const raw);
  float toFloat(void) const;
  int   toInt(void) const;
};

std::ostream& operator<<(std::ostream& o, Fixed const& i);

#endif  // CPP_MODULE02_EX01_FIXED_HPP_
