/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 18:42:31 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/04 16:31:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE02_EX03_FIXED_HPP_
#define CPP_MODULE02_EX03_FIXED_HPP_

#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <iostream>

class Fixed {
 private:
  int _fixedPointValue;
  static const int _fractionalBits = 8;

 public:
  Fixed();
  explicit Fixed(const int n);
  explicit Fixed(const float f);
  Fixed(const Fixed& other);
  Fixed& operator=(const Fixed& other);
  ~Fixed();

  int getRawBits(void) const;
  void setRawBits(int const raw);
  float toFloat(void) const;
  int toInt(void) const;
  bool operator>(const Fixed& other) const;
  bool operator<(const Fixed& other) const;
  bool operator>=(const Fixed& other) const;
  bool operator<=(const Fixed& other) const;
  bool operator==(const Fixed& other) const;
  bool operator!=(const Fixed& other) const;
  Fixed operator+(const Fixed& other) const;
  Fixed operator-(const Fixed& other) const;
  Fixed operator*(const Fixed& other) const;
  Fixed operator/(const Fixed& other) const;
  Fixed& operator++(void);
  Fixed operator++(int);
  Fixed& operator--(void);
  Fixed operator--(int);
  static Fixed& min(Fixed& a, Fixed& b);
  static const Fixed& min(const Fixed& a, const Fixed& b);
  static Fixed& max(Fixed& a, Fixed& b);
  static const Fixed& max(const Fixed& a, const Fixed& b);
};

std::ostream& operator<<(std::ostream& o, Fixed const& i);

#endif  // CPP_MODULE02_EX03_FIXED_HPP_
