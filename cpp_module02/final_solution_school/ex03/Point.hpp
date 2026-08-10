/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Point.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 21:41:23 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/04 20:39:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE02_EX03_POINT_HPP_
#define CPP_MODULE02_EX03_POINT_HPP_

#include "Fixed.hpp"

/**
 * @file Point.hpp
 * @brief Immutable 2D point class used by the BSP (point-in-triangle) test.
 *
 * @details
 * This exercise builds geometry on top of the `Fixed` type.
 *
 * ### Fixed-point representation (what “bits” are used here)
 * `Fixed` stores a signed integer `_fixedPointValue` with `_fractionalBits = 8`.
 * That is a Q24.8 style fixed-point number:
 *
 * - Real value = `_fixedPointValue / 2^8`
 * - 1.0 is stored as 256
 * - 0.5 is stored as 128
 *
 * So when `Point` stores coordinates as `Fixed`, every geometric computation
 * performed in `bsp.cpp` (subtractions, multiplications, comparisons) is done
 * through `Fixed` operators.
 *
 * ### Immutability
 * `Point` is intentionally *immutable*:
 *
 * - `_x` and `_y` are `const Fixed`
 * - once constructed, the coordinates don’t change
 *
 * This is a great fit for geometry: points represent positions.
 *
 * ### Assignment operator caveat
 * Because `_x` and `_y` are `const`, a normal assignment operator can’t
 * legally reassign them.
 *
 * This repository currently implements `operator=` by casting away constness
 * in `Point.cpp`. That works at runtime but is **undefined behaviour in C++**.
 * In a strictly-correct design, you would either:
 *
 * - delete/omit `operator=` (preferred for immutable types), or
 * - drop the `const` qualifiers.
 */

/**
 * @class Point
 * @brief Immutable 2D point (`x`, `y`) stored in fixed-point.
 *
 * @details
 * The BSP algorithm only needs read access to coordinates.
 *
 * Contract:
 * - Inputs: constructed from floats (converted to `Fixed` internally)
 * - Output: `getX()` / `getY()` expose `const Fixed&` for arithmetic
 * - Error modes: none (no dynamic allocation)
 */
class Point {
 private:
  Fixed const _x; ///< X coordinate in Q24.8 fixed-point (`Fixed`)
  Fixed const _y; ///< Y coordinate in Q24.8 fixed-point (`Fixed`)

 public:
  // Orthodox Canonical Form
  /**
   * @brief Default constructor.
   *
   * @details
   * Builds the origin (0, 0).
   */
  Point();

  /**
   * @brief Construct a point from two floats.
   *
   * @param x X coordinate as float.
   * @param y Y coordinate as float.
   *
   * @details
   * The floats are converted to fixed-point in `Fixed(float)`.
   */
  Point(const float x, const float y);

  /**
   * @brief Copy constructor.
   *
   * @details
   * Copies `_x` and `_y` (each is a `Fixed`). This is safe: `Fixed` owns no
   * heap memory here.
   */
  Point(const Point& other);

  /**
   * @brief Copy assignment.
   *
   * @warning
   * With `const` data members, this operation cannot be implemented legally.
   * The current implementation casts away constness in `Point.cpp`, which is
   * undefined behaviour by the C++ standard.
   */
  Point& operator=(const Point& other);

  /**
   * @brief Destructor.
   *
   * @details
   * Trivial: no dynamic memory.
   */
  ~Point();

  // Getters
  /**
   * @brief Get X coordinate.
   * @return Reference to the fixed-point X value.
   */
  Fixed const& getX(void) const;

  /**
   * @brief Get Y coordinate.
   * @return Reference to the fixed-point Y value.
   */
  Fixed const& getY(void) const;
};

// BSP function prototype
/**
 * @brief Test whether a point lies strictly inside the triangle (a, b, c).
 *
 * @param a Triangle vertex A.
 * @param b Triangle vertex B.
 * @param c Triangle vertex C.
 * @param point The point to test.
 *
 * @return `true` if `point` is strictly inside the triangle, `false` otherwise.
 *
 * @details
 * Implemented in `bsp.cpp` using cross-product sign consistency.
 * If the point lies exactly on an edge or vertex, the function returns false
 * (subject requirement).
 */
bool bsp(Point const a, Point const b, Point const c, Point const point);

#endif  // CPP_MODULE02_EX03_POINT_HPP_
