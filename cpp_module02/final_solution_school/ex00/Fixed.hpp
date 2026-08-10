/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:54:24 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/03 13:46:13 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE02_EX00_FIXED_HPP_
#define CPP_MODULE02_EX00_FIXED_HPP_

#include <iostream>

/**
 * @class Fixed
 * @brief A fixed-point number with 8 fractional bits, in Orthodox Canonical Form.
 *
 * @details
 * A fixed-point number is a plain @c int that we *interpret* as having a
 * decimal point at a fixed position.  With 8 fractional bits the scale
 * factor is 2^8 = 256, so the raw integer stored is always (value × 256).
 *
 * **Memory model** (32-bit int, 8 fractional bits)
 * @code
 *  bit 31                    bit 8  bit 7          bit 0
 *  [ sign | 23 integer bits       ][ 8 fractional bits ]
 *                                   ^^^^^^^^^^^^^^^^^^^
 *                                   _fractionalBits = 8
 *
 *  Logical 42.0  →  raw = 42 × 256 = 10752
 *  Binary:  00000000 00000000 00101010 00000000
 * @endcode
 *
 * **Precision** — smallest representable step = 1/256 ≈ 0.00390625
 * @code
 *   42.00000000   raw 10752   ← representable
 *   42.00390625   raw 10753   ← representable
 *   42.00100000              ← NOT representable (rounds to nearest)
 * @endcode
 *
 * **Why fixed-point instead of float?**
 * | Type        | Storage | Precision style         |
 * |-------------|---------|-------------------------|
 * | float       | 32 bits | variable (IEEE 754)     |
 * | double      | 64 bits | variable (IEEE 754)     |
 * | Fixed (ours)| 32 bits | fixed step of 1/256     |
 *
 * @c float results can differ across CPUs, compilers and optimisation levels.
 * Fixed-point is 100 % deterministic: same input → same output, always.
 * Used in game engines, audio DSP and embedded systems for that reason.
 *
 * **Orthodox Canonical Form (OCF)**
 *
 * C++ requires you to control four moments in an object's lifetime.
 * If you skip any of them the compiler auto-generates a *shallow* version
 * — it copies raw bytes including pointer values, without touching the data
 * those pointers point to.  That is fine for a plain @c int.  It is
 * catastrophic the moment @c _fixedPointValue becomes an @c int* :
 * @code
 *   // Hypothetical broken version with a heap pointer:
 *   NaiveFixed a;          // a._raw → new int(0)  on heap
 *   NaiveFixed b(a);       // compiler copies the POINTER → b._raw == a._raw
 *   b.setRawBits(99);      // writes through b._raw → also changes *a._raw !
 *   // scope ends → ~b() deletes a._raw
 *   //            → ~a() deletes the SAME address → DOUBLE FREE → crash / UB
 * @endcode
 *
 * Writing OCF explicitly even for a simple class trains the habit you *need*
 * once you touch heap memory, STL containers or RAII resource handles.
 */
class Fixed {
 private:
  int              _fixedPointValue;    ///< Raw scaled integer (logical value × 256)
  static const int _fractionalBits = 8; ///< Number of bits dedicated to the fraction

 public:
  /**
   * @brief Default constructor — initialises to zero, never leaves garbage.
   *
   * Uses a member-initialiser list so @c _fixedPointValue is set *before*
   * the constructor body runs.  Without @c : _fixedPointValue(0) the int
   * would hold whatever bytes happened to be at that stack address.
   */
  Fixed();

  /**
   * @brief Copy constructor — creates an independent clone of @p other.
   *
   * @param other  The existing object to copy from.
   *
   * Delegates to @c operator= so copy logic lives in exactly one place.
   * The new object gets its own @c _fixedPointValue; changing it later
   * does not affect @p other and vice-versa.
   *
   * @note This is called by:
   * @code
   *   Fixed b(a);             // explicit copy construction
   *   Fixed b = a;            // copy-initialisation (same thing)
   *   void foo(Fixed x);      // pass by value → copy construction
   *   Fixed bar() { return a; } // return by value → copy construction
   * @endcode
   */
  Fixed(const Fixed& other);

  /**
   * @brief Copy assignment operator — overwrites an *existing* object.
   *
   * @param other  The object to copy from.
   * @return       Reference to @c *this, enabling chained assignment (@c a=b=c).
   *
   * Unlike the copy constructor, the destination already has state.
   * The self-assignment guard (@c if (this != &other)) is critical:
   * without it @c a = a could corrupt the object if it owned heap memory
   * (free then read the freed region).
   */
  Fixed& operator=(const Fixed& other);

  /**
   * @brief Destructor — called automatically when the object goes out of scope.
   *
   * For @c Fixed there is nothing to free: @c _fixedPointValue is a plain
   * @c int that lives *inside* the object on the stack.  If it were an
   * @c int* you would write @c delete @c _fixedPointValue here.
   */
  ~Fixed();

  /**
   * @brief Returns the raw internal integer (not divided by 256).
   * @return Raw bits, e.g. logical 42.0 → returns 10752.
   */
  int  getRawBits(void) const;

  /**
   * @brief Directly overwrites the raw internal integer (no scaling applied).
   * @param raw  The raw value to store.
   */
  void setRawBits(int const raw);
};

#endif  // CPP_MODULE02_EX00_FIXED_HPP_
