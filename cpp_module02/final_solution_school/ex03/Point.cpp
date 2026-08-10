/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Point.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 21:44:31 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/04 16:29:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Point.hpp"

/**
 * @file Point.cpp
 * @brief Implementation of the `Point` class.
 *
 * @details
 * `Point` is designed to be immutable: it stores coordinates as `const Fixed`.
 * That makes value semantics very safe for geometry, but it has an important
 * consequence: a normal copy-assignment operator can’t legally change `_x` and
 * `_y`.
 *
 * The implementation below keeps `operator=` only to satisfy the “Orthodox
 * Canonical Form” requirement of the module. It does so by casting away
 * constness and writing into the object anyway.
 *
 * @warning
 * Casting away constness and modifying a `const` subobject is **undefined
 * behaviour** in standard C++. It may “work” in practice, but it’s not a
 * correct/portable design.
 */

/**
 * @brief Default constructor.
 * @details Builds the origin (0, 0).
 */
Point::Point() : _x(0), _y(0) {}

/**
 * @brief Construct a point from two floats.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @details The float to fixed-point conversion happens in `Fixed(float)`.
 */
Point::Point(const float x, const float y) : _x(x), _y(y) {}

/**
 * @brief Copy constructor.
 */
Point::Point(const Point& other) : _x(other._x), _y(other._y) {}

/**
 * TRAP BYPASS: _x and _y are const, so standard assignment fails
 * We cast away constness to satisfy the Orthodox Form requirements
 */
/**
 * @brief Copy assignment.
 * @warning This function relies on a const-cast “trap bypass” (UB).
 */
Point& Point::operator=(const Point& other) {
  if (this != &other) {
    (Fixed&)this->_x = other.getX();
    (Fixed&)this->_y = other.getY();
  }
  return (*this);
}

/**
 * @brief Destructor.
 */
Point::~Point() {}

/** @brief Get X coordinate. */
Fixed const& Point::getX(void) const { return (this->_x); }
/** @brief Get Y coordinate. */
Fixed const& Point::getY(void) const { return (this->_y); }
