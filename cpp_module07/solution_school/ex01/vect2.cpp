/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vect2.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 16:23:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/14 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vect2.hpp"

#include <ostream>

vect2::vect2() : _x(0), _y(0) {}

vect2::vect2(int x, int y) : _x(x), _y(y) {}

vect2::vect2(const vect2& o) : _x(o._x), _y(o._y) {}

vect2& vect2::operator=(const vect2& o) {
  _x = o._x;
  _y = o._y;
  return (*this);
}

vect2::~vect2() {}

int& vect2::operator[](int idx) { return (idx == 0 ? _x : _y); }

int vect2::operator[](int idx) const { return (idx == 0 ? _x : _y); }

vect2& vect2::operator++(void) {
  ++_x;
  ++_y;
  return (*this);
}

vect2& vect2::operator--(void) {
  --_x;
  --_y;
  return (*this);
}

vect2 vect2::operator++(int) {
  vect2 tmp(*this);
  ++(*this);
  return (tmp);
}

vect2 vect2::operator--(int) {
  vect2 tmp(*this);
  --(*this);
  return (tmp);
}

vect2& vect2::operator+=(const vect2& o) {
  _x += o._x;
  _y += o._y;
  return (*this);
}

vect2& vect2::operator-=(const vect2& o) {
  _x -= o._x;
  _y -= o._y;
  return (*this);
}

vect2& vect2::operator*=(int n) {
  _x *= n;
  _y *= n;
  return (*this);
}

vect2 vect2::operator+(const vect2& o) const {
  return (vect2(_x + o._x, _y + o._y));
}

vect2 vect2::operator-(const vect2& o) const {
  return (vect2(_x - o._x, _y - o._y));
}

vect2 vect2::operator-(void) const { return (vect2(-_x, -_y)); }

vect2 vect2::operator*(int n) const { return (vect2(_x * n, _y * n)); }

bool vect2::operator==(const vect2& o) const {
  return ((_x == o._x) && (_y == o._y));
}

bool vect2::operator!=(const vect2& o) const { return (!(*this == o)); }

vect2 operator*(int n, const vect2& v) { return (v * n); }

std::ostream& operator<<(std::ostream& os, const vect2& v) {
  return (os << "{" << v[0] << ", " << v[1] << "}");
}
