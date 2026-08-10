/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 18:50:34 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 18:06:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Fixed.hpp"

/* ── Pure Bitwise Logic Gates (Internal Helpers) ─────────────────────────── */
static int bit_add(int a, int b) {
  while (b != 0) {
    int carry = a & b;
    a = a ^ b;
    b = carry << 1;
  }
  return a;
}

static int bit_sub(int a, int b) { return bit_add(a, bit_add(~b, 1)); }

static int64_t ll_add(int64_t a, int64_t b) {
  while (b != 0) {
    int64_t carry = a & b;
    a = a ^ b;
    b = carry << 1;
  }
  return a;
}

static int64_t ll_sub(int64_t a, int64_t b) { return ll_add(a, ll_add(~b, 1)); }

/* ── Orthodox Canonical Form ─────────────────────────────────────────────── */
Fixed::Fixed() : _fixedPointValue(0) {}
Fixed::Fixed(const int n) : _fixedPointValue(n << _fractionalBits) {}
Fixed::Fixed(const float f)
    : _fixedPointValue(roundf(f * (1 << _fractionalBits))) {}
Fixed::Fixed(const Fixed& other) { *this = other; }
Fixed& Fixed::operator=(const Fixed& other) {
  if (this != &other) this->_fixedPointValue = other.getRawBits();
  return *this;
}
Fixed::~Fixed() {}

/* ── Getters / Setters / Converters ──────────────────────────────────────── */
int Fixed::getRawBits(void) const { return this->_fixedPointValue; }
void Fixed::setRawBits(int const raw) { this->_fixedPointValue = raw; }
float Fixed::toFloat(void) const {
  return static_cast<float>(this->_fixedPointValue) / (1 << _fractionalBits);
}
int Fixed::toInt(void) const {
  return this->_fixedPointValue >> _fractionalBits;
}

/* ── Pure Bitwise Comparison Operators ───────────────────────────────────── */
bool Fixed::operator==(const Fixed& other) const {
  return !(this->_fixedPointValue ^ other._fixedPointValue);
}
bool Fixed::operator!=(const Fixed& other) const {
  return (this->_fixedPointValue ^ other._fixedPointValue);
}
bool Fixed::operator<(const Fixed& other) const {
  int a = this->_fixedPointValue;
  int b = other._fixedPointValue;
  int sign_a = (a >> 31) & 1;
  int sign_b = (b >> 31) & 1;
  int sign_diff = (bit_sub(a, b) >> 31) & 1;
  return ((sign_a ^ sign_b) & sign_a) | (~(sign_a ^ sign_b) & sign_diff);
}
bool Fixed::operator>(const Fixed& other) const {
  int a = this->_fixedPointValue;
  int b = other._fixedPointValue;
  int sign_a = (a >> 31) & 1;
  int sign_b = (b >> 31) & 1;
  int sign_diff_ba = (bit_sub(b, a) >> 31) & 1;
  return ((sign_a ^ sign_b) & sign_b) | (~(sign_a ^ sign_b) & sign_diff_ba);
}
bool Fixed::operator<=(const Fixed& other) const {
  return (*this < other) || (*this == other);
}
bool Fixed::operator>=(const Fixed& other) const {
  return (*this > other) || (*this == other);
}

/* ── Bitwise Arithmetic Operators ────────────────────────────────────────── */
Fixed Fixed::operator+(const Fixed& other) const {
  Fixed res;
  res.setRawBits(bit_add(this->_fixedPointValue, other._fixedPointValue));
  return res;
}
Fixed Fixed::operator-(const Fixed& other) const {
  Fixed res;
  res.setRawBits(bit_sub(this->_fixedPointValue, other._fixedPointValue));
  return res;
}
Fixed Fixed::operator*(const Fixed& other) const {
  Fixed res;
  int64_t a = this->_fixedPointValue;
  int64_t b = other._fixedPointValue;
  int64_t sign_a = (a >> 63) & 1;
  int64_t sign_b = (b >> 63) & 1;
  int64_t final_sign = sign_a ^ sign_b;
  if (sign_a) a = ll_sub(0, a);
  if (sign_b) b = ll_sub(0, b);
  int64_t product = 0;
  while (b > 0) {
    if (b & 1) product = ll_add(product, a);
    a <<= 1;
    b >>= 1;
  }
  int64_t rounding_bit = (product >> bit_sub(_fractionalBits, 1)) & 1;
  product >>= _fractionalBits;
  product = ll_add(product, rounding_bit);
  if (final_sign) product = ll_sub(0, product);
  res.setRawBits(static_cast<int>(product));
  return res;
}

Fixed Fixed::operator/(const Fixed& other) const {
  Fixed res;
  if (other._fixedPointValue == 0) {
    std::cerr << "Error: Division by zero" << std::endl;
    return res;
  }
  int64_t a = this->_fixedPointValue;
  int64_t b = other._fixedPointValue;
  int64_t sign_a = (a >> 63) & 1;
  int64_t sign_b = (b >> 63) & 1;
  int64_t final_sign = sign_a ^ sign_b;
  if (sign_a) a = ll_sub(0, a);
  if (sign_b) b = ll_sub(0, b);
  int64_t dividend = a << _fractionalBits;
  int64_t divisor = b;
  int64_t quotient = 0;
  for (int i = 39; i >= 0; i = bit_sub(i, 1)) {
    int64_t shifted_divisor = divisor << i;
    int64_t diff = ll_sub(dividend, shifted_divisor);
    if (((diff >> 63) & 1) == 0) {
      dividend = diff;
      quotient = ll_add(quotient, static_cast<int64_t>(1) << i);
    }
  }
  if (final_sign) quotient = ll_sub(0, quotient);
  res.setRawBits(static_cast<int>(quotient));
  return res;
}

/* ── Increment / Decrement Operators ─────────────────────────────────────── */
Fixed& Fixed::operator++(void) {
  this->_fixedPointValue = bit_add(this->_fixedPointValue, 1);
  return *this;
}
Fixed Fixed::operator++(int) {
  Fixed tmp(*this);
  operator++();
  return tmp;
}
Fixed& Fixed::operator--(void) {
  this->_fixedPointValue = bit_sub(this->_fixedPointValue, 1);
  return *this;
}
Fixed Fixed::operator--(int) {
  Fixed tmp(*this);
  operator--();
  return tmp;
}

/* ── Static Min/Max Functions ────────────────────────────────────────────── */
Fixed& Fixed::min(Fixed& a, Fixed& b) { return (a < b) ? a : b; }
const Fixed& Fixed::min(const Fixed& a, const Fixed& b) {
  return (a < b) ? a : b;
}
Fixed& Fixed::max(Fixed& a, Fixed& b) { return (a > b) ? a : b; }
const Fixed& Fixed::max(const Fixed& a, const Fixed& b) {
  return (a > b) ? a : b;
}

/* ── Stream Overload ─────────────────────────────────────────────────────── */
std::ostream& operator<<(std::ostream& o, Fixed const& i) {
  o << i.toFloat();
  return o;
}
