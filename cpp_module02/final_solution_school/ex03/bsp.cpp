/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsp.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 21:48:43 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/04 20:39:13 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Point.hpp"

struct Vec2 {
  Fixed x;
  Fixed y;
};

/**
 * @file bsp.cpp
 * @brief BSP (Binary Space Partitioning) style point-in-triangle test.
 *
 * @details
 * Despite the filename, this is not a tree-based BSP. The function `bsp()`
 * implements a classic *point-in-triangle* test using oriented areas.
 *
 * ## Key idea: consistent orientation using cross products
 * For an edge (A -> B) and a test point P, the sign of the 2D cross product
 * tells on which side of the directed edge the point lies.
 *
 * In 2D, we compute the Z component of the 3D cross product:
 *
 *   cross((B - A), (P - A)).z
 *
 * - positive: P is to the “left” of the directed edge
 * - negative: P is to the “right”
 * - zero:     P is collinear with the edge (on the line)
 *
 * `bsp()` computes this value for the three edges of the triangle.
 * If all three cross products have the same sign, P is inside.
 * If signs are mixed, P is outside.
 *
 * ## Strictness (subject requirement)
 * The subject wants: “a point on an edge or a vertex returns false”.
 * That’s why we explicitly reject when any cross product is exactly 0.
 *
 * ## Where fixed-point “bits” come into play
 * All arithmetic here is done with `Fixed`:
 * - subtractions: `Fixed::operator-`
 * - multiplications: `Fixed::operator*`
 * - comparisons: `Fixed::operator<`, `operator>` and `operator==`
 *
 * Internally, `Fixed` stores values scaled by `2^8` and implements some
 * operations using bitwise logic. So even though `bsp.cpp` reads like normal
 * arithmetic, the underlying representation is fixed-point.
 * 
 * DETERMINANT: 2x2
 * for a 2x2 matrix:
 * [a b]
 * [c d]
 * 
 * https://www.hec.ca/cams/rubriques-aide/notion-mathematiques/Les_determinants_des_matrices.pdf
 * 
 * GOAL: The determinant is an essential tool to identify the maximum points and the minimum
 * of the point from a function of some variables
 * 
 * A matrice is map of shape
 *      [A11    A12   ...   A1n]
 * A =  [A21    A22   ...   A2n]
 *      [Am1    Am2   ...   Amn]
 * 
 * A matrice is saif that the dimension `m x n` when this one possess m rows and n columns
 * GLOSSARY:
 * 
 * RULE: the order is chosen so that the edge direction is fixed, and the point is tested against that edge
 * in BSP / point-in-triangle tests, the rule is 
 *    Vector1 = edge direction
 *    Vector2 = direction from edge to the point
 * 
 * Then:
 *  det(edge, point)
 *
 * CASE: PAB 3 differnt points
 * A is the reference origin
 * edge is AB
 * Test point is P
 * 
 * build vectors
 * ```
 * edge vector v = AB = B - A
 * point vector u = AP = P - A
 * 
 * Correct order (standard orientation test):
 *  ✅det(v, u) = det(AB, AP)
 * ❌det(u, v) = det(ap,ab) = -det(ab,ap)
 * same geometry, flipped again
 * ```
 * 
 */

/**
 * @brief Compute the signed “area” / orientation for three points.
 * A cross product matrix, specifically the skew-symmetric matrix [a](x) formed from a
 * vector a = [a(1), a(2), a(3)]^T, represents the linear mapping A(b) = a x b as a
 * matrix-vector product [a](x)b, it is a 3 x 3 matrix defined as:
 * [a](x) = [ 0     -a3   -a2  ]
 *          [ a3    0     -a1  ]
 *          [ -a2   a1    0    ]
 * This matrix is used extensively in 3D rotation, robotics...
 * @param p1 Typically the test point (P).
 * @param p2 First vertex of the edge.
 * @param p3 Second vertex of the edge.
 *
 * @return A `Fixed` value proportional to the oriented area.
 *
 * @details
 * This returns the Z component of a 2D cross product (signed scalar):
 *
 *   (p1 - p3) x (p2 - p3)
 *
 * Expanded to:
 *   (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3)
 *
 * Only the sign matters for inside/outside.
 * 
 *
 */

/**
 * @struct Vec2
 * @brief A plain 2D vector (dx, dy) in fixed-point.
 *
 * @details
 * This is intentionally a C-style struct (all public, no methods).
 * That is perfectly idiomatic in C++98 for a pure data aggregate.
 *
 * Using a struct here vs a full class:
 * - No invariant to protect → no reason for private members.
 * - No heap allocation → no destructor needed.
 * - Lightweight: the compiler lays it out as two consecutive `Fixed` values.
 *
 * MEMORY LAYOUT:
 *   ┌──────────────┬──────────────┐
 *   │  x  (Fixed)  │  y  (Fixed)  │
 *   │  4 bytes     │  4 bytes     │
 *   └──────────────┴──────────────┘
 */

/**
 * @brief Build a Vec2 from two Points: the vector `from → to`.
 *
 * @param from  Origin point of the vector.
 * @param to    Tip point of the vector.
 * @return      Vec2 { to.x - from.x, to.y - from.y }
 *
 * @details
 * This encodes the geometric concept of a *direction* between two positions.
 *
 * Example — edge AB where A = (0,0), B = (10,0):
 * @code
 *   Vec2 ab = vectorFrom(A, B);
 *   // ab.x = Fixed(10) - Fixed(0) = Fixed(10)
 *   // ab.y = Fixed(0)  - Fixed(0) = Fixed(0)
 * @endcode
 *
 * The result is passed to `crossProduct` as the two vectors whose
 * determinant gives the signed area / orientation.
 */
static Vec2 vectorFrom(Point const& from, Point const& to) {
  Vec2 v;
  v.x = to.getX() - from.getX();
  v.y = to.getY() - from.getY();
  return v;
}

/**
 * @brief Compute the 2D cross product (determinant) of two Vec2 vectors.
 *
 * @param u  First vector  (edge direction: vertex → vertex).
 * @param v  Second vector (point direction: vertex → test point).
 * @return   Fixed scalar = u.x * v.y - u.y * v.x
 *
 * @details
 * The 2D cross product is the Z component of the 3D cross product
 * when both vectors lie in the XY plane:
 *
 *   u × v = |u  v| = u.x * v.y - u.y * v.x
 *             determinant of the 2×2 matrix [u | v]
 *
 * MATRIX FORM:
 *   | u.x  v.x |
 *   | u.y  v.y |   →  det = u.x * v.y  −  u.y * v.x
 *
 * SIGN MEANING:
 *   > 0  →  v is to the LEFT  of u  (counter-clockwise turn)
 *   < 0  →  v is to the RIGHT of u  (clockwise turn)
 *   = 0  →  u and v are collinear   (point on the edge line)
 *
 * FIXED-POINT NOTE:
 * `u.x * v.y` calls `Fixed::operator*` which keeps the result scaled by 2^8.
 * The subtraction `Fixed::operator-` then operates on those scaled values.
 * The final sign is preserved exactly — no float conversion needed.
 */
static Fixed det(Vec2 const u, Vec2 const v) {
  return (u.x * v.y) - (u.y * v.x);
}

/**
 * @brief Compute the signed orientation of point p relative to edge (p2→p3).
 *
 * @param p   The test point.
 * @param p2  First vertex of the edge (edge origin).
 * @param p3  Second vertex of the edge (edge tip).
 * @return    Fixed: sign tells which side of the edge p is on.
 *
 * @details
 * Builds two vectors anchored at p3:
 *   edge  = vectorFrom(p3, p2)   ← the edge direction (p3 → p2)
 *   point = vectorFrom(p3, p)    ← direction toward the test point
 *
 * Then returns det(edge, point).
 *
 * WHY anchor at p3?
 *   Translating both vectors to the same origin (p3) isolates the
 *   relative geometry — the determinant only measures orientation,
 *   not absolute position.
 *
 * TRACE — p=(2,2), p2=(0,0), p3=(10,0):
 * @code
 *   edge  = vectorFrom(p3=(10,0), p2=(0,0)) → { -10, 0 }
 *   point = vectorFrom(p3=(10,0), p=(2,2))  → { -8,  2 }
 *   det   = (-10)*2 - (0)*(-8) = -20
 *   → negative: (2,2) is to the RIGHT of edge (10,0)→(0,0)
 * @endcode
 */
static Fixed crossProduct(Point const p, Point const p2, Point const p3) {
  Vec2 edge  = vectorFrom(p3, p2);
  Vec2 point = vectorFrom(p3, p);
  return det(edge, point);
}

/**
 * @brief Return true if `point` is strictly inside triangle (a, b, c).
 *
 * @details
 * Steps:
 * 1) Compute cross products for edges AB, BC, CA with respect to `point`.
 * 2) If any cross product is 0 => on an edge/vertex => return false.
 * 3) If the signs are mixed (some positive and some negative) => outside.
 * 4) Otherwise => strictly inside.
 */
bool bsp(Point const a, Point const b, Point const c, Point const point) {
  Fixed d1, d2, d3;
  bool has_neg, has_pos;

  // Calculate cross products for the 3 edges
  d1 = crossProduct(point, a, b);
  d2 = crossProduct(point, b, c);
  d3 = crossProduct(point, c, a);

  // Subject requirement: If point is a vertex or on an edge, return False.
  // An edge collision yields a cross product of exactly 0.
  if (d1 == Fixed(0) || d2 == Fixed(0) || d3 == Fixed(0)) return false;

  // Check if the signs are mixed
  has_neg = (d1 < Fixed(0)) || (d2 < Fixed(0)) || (d3 < Fixed(0));
  has_pos = (d1 > Fixed(0)) || (d2 > Fixed(0)) || (d3 > Fixed(0));

  // If it has BOTH negative and positive cross products, the point is OUTSIDE.
  // If it is entirely negative OR entirely positive, it is strictly INSIDE.
  return !(has_neg && has_pos);
}
