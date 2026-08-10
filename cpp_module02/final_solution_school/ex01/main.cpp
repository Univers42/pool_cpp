/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 18:23:47 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/04 21:01:59 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Fixed.hpp"

#define BOLD    "\033[1m"
#define RESET   "\033[0m"
#define DIM     "\033[2m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"
#define BLUE    "\033[34m"
#define RED     "\033[31m"

// ─────────────────────────────────────────────────────────────────────────────
// Helper — prints a clear section banner so output is easy to read
// ─────────────────────────────────────────────────────────────────────────────
static void banner(const char* title) {
  std::cout << BOLD << CYAN
            << "\n══════════════════════════════════════════\n"
            << "  " << title << "\n"
            << "══════════════════════════════════════════\n"
            << RESET;
}

int main(void) {

  // ── 1. DEFAULT CONSTRUCTOR ────────────────────────────────────────────────
  // Fixed() sets _fixedPointValue = 0
  // raw = 0  →  toFloat() = 0/256 = 0.0  →  toInt() = 0
  banner("1. Default constructor  →  Fixed a");
  {
    Fixed a;
    std::cout << GREEN  << "  a (operator<<) : " << RESET << a            << "\n";
    std::cout << GREEN  << "  a.toFloat()    : " << RESET << a.toFloat()  << "\n";
    std::cout << GREEN  << "  a.toInt()      : " << RESET << a.toInt()    << "\n";
    std::cout << GREEN  << "  a.getRawBits() : " << RESET << a.getRawBits() << "\n";
  }

  // ── 2. INT CONSTRUCTOR ────────────────────────────────────────────────────
  // Fixed(int n) stores n << 8, i.e. n * 256
  //   Fixed(10)  → raw = 10  * 256 = 2560  → toFloat = 10.0
  //   Fixed(-3)  → raw = -3  * 256 = -768  → toFloat = -3.0
  //   Fixed(0)   → raw = 0                 → toFloat =  0.0
  banner("2. Int constructor  →  Fixed(n)");
  {
    Fixed b(10);
    Fixed neg(-3);
    Fixed zero(0);

    std::cout << YELLOW << "  Fixed(10)  : " << RESET << b    << DIM << "  raw=" << b.getRawBits()    << RESET << "\n";
    std::cout << YELLOW << "  Fixed(-3)  : " << RESET << neg  << DIM << "  raw=" << neg.getRawBits()  << RESET << "\n";
    std::cout << YELLOW << "  Fixed(0)   : " << RESET << zero << DIM << "  raw=" << zero.getRawBits() << RESET << "\n";
  }

  // ── 3. FLOAT CONSTRUCTOR ──────────────────────────────────────────────────
  // Fixed(float f) stores roundf(f * 256)
  //   42.42f  → roundf(42.42 * 256) = roundf(10859.52) = 10860  → /256 = 42.4219
  //   -1.5f   → roundf(-1.5 * 256)  = -384               → /256 = -1.5
  //   0.004f  → roundf(0.004 * 256) = roundf(1.024) = 1   → /256 ≈ 0.00390625
  //             (minimum non-zero value representable in Q24.8)
  banner("3. Float constructor  →  Fixed(f)");
  {
    Fixed c(42.42f);
    Fixed neg(-1.5f);
    Fixed tiny(0.004f);
    Fixed exact(1234.4321f);

    std::cout << MAGENTA << "  Fixed(42.42f)     : " << RESET << c     << DIM << "  raw=" << c.getRawBits()     << RESET << "\n";
    std::cout << MAGENTA << "  Fixed(-1.5f)      : " << RESET << neg   << DIM << "  raw=" << neg.getRawBits()   << RESET << "\n";
    std::cout << MAGENTA << "  Fixed(0.004f)     : " << RESET << tiny  << DIM << "  raw=" << tiny.getRawBits()  << RESET << "\n";
    std::cout << MAGENTA << "  Fixed(1234.4321f) : " << RESET << exact << DIM << "  raw=" << exact.getRawBits() << RESET << "\n";
  }

  // ── 4. COPY CONSTRUCTOR ───────────────────────────────────────────────────
  // Fixed d(b) — creates a brand-new object by copying b's raw bits.
  // After copy: d and b have the SAME value but are INDEPENDENT objects.
  // Changing one must NOT affect the other (deep copy — no heap here,
  // but the principle is identical: you copy the value, not a pointer).
  banner("4. Copy constructor  →  Fixed d(b)");
  {
    Fixed const b(10);
    Fixed const d(b);

    std::cout << BLUE << "  b : " << RESET << b << DIM << "  raw=" << b.getRawBits() << RESET << "\n";
    std::cout << BLUE << "  d : " << RESET << d << DIM << "  raw=" << d.getRawBits() << RESET << "\n";
    std::cout << GREEN << "  Same value? " << RESET
              << (b.getRawBits() == d.getRawBits() ? "YES ✓" : "NO ✗") << "\n";
  }

  // ── 5. COPY ASSIGNMENT OPERATOR ───────────────────────────────────────────
  // a = b  — both objects already exist.
  // operator= overwrites a's raw bits with b's.
  // Key contract: self-assignment (a = a) must be a safe no-op.
  banner("5. Copy assignment  →  a = b");
  {
    Fixed a;
    Fixed b(42.42f);

    std::cout << CYAN << "  before:" << RESET << " a=" << a << "  b=" << b << "\n";
    a = b;
    std::cout << CYAN << "  after :" << RESET << " a=" << a << "  b=" << b << "\n";
    std::cout << DIM  << "  a.getRawBits()=" << a.getRawBits()
              << "  b.getRawBits()=" << b.getRawBits() << RESET << "\n";
  }

  // ── 6. CHAIN ASSIGNMENT ───────────────────────────────────────────────────
  // a = b = c  is right-associative:  a = (b = c)
  // operator= returns *this (reference), enabling the chain without extra copies.
  banner("6. Chain assignment  →  a = b = c");
  {
    Fixed a;
    Fixed b;
    Fixed c(99.5f);

    std::cout << YELLOW << "  before:" << RESET << " a=" << a << "  b=" << b << "  c=" << c << "\n";
    a = b = c;
    std::cout << YELLOW << "  after :" << RESET << " a=" << a << "  b=" << b << "  c=" << c << "\n";
  }

  // ── 7. SETRAWBITS / GETRAWBITS ────────────────────────────────────────────
  // setRawBits lets you inject a raw fixed-point integer directly —
  // useful for testing edge cases without going through a constructor.
  // raw=1   → toFloat = 1/256  ≈ 0.00390625  (smallest positive step)
  // raw=256 → toFloat = 256/256 = 1.0
  // raw=-1  → toFloat = -1/256 ≈ -0.00390625
  banner("7. setRawBits / getRawBits");
  {
    Fixed a;

    a.setRawBits(1);
    std::cout << BLUE << "  raw=1   →" << RESET << " toFloat=" << GREEN << a.toFloat() << RESET
              << "  toInt=" << YELLOW << a.toInt() << RESET << "\n";

    a.setRawBits(256);
    std::cout << BLUE << "  raw=256 →" << RESET << " toFloat=" << GREEN << a.toFloat() << RESET
              << "  toInt=" << YELLOW << a.toInt() << RESET << "\n";

    a.setRawBits(-1);
    std::cout << BLUE << "  raw=-1  →" << RESET << " toFloat=" << RED << a.toFloat() << RESET
              << "  toInt=" << RED << a.toInt() << RESET << "\n";
  }

  // ── 8. TOINT TRUNCATION ───────────────────────────────────────────────────
  // toInt() truncates toward zero (via the float cast).
  // 42.9f → toInt = 42   (fractional part discarded)
  // -1.9f → toInt = -1   (truncation, not floor)
  banner("8. toInt() truncation behaviour");
  {
    Fixed a(42.9f);
    Fixed b(-1.9f);
    Fixed c(0.99f);

    std::cout << MAGENTA << "  Fixed(42.9f).toInt()  = " << RESET << a.toInt() << "\n";
    std::cout << MAGENTA << "  Fixed(-1.9f).toInt()  = " << RESET << b.toInt() << "\n";
    std::cout << MAGENTA << "  Fixed(0.99f).toInt()  = " << RESET << c.toInt() << "\n";
  }

  // ── 9. SUBJECT REFERENCE OUTPUT ──────────────────────────────────────────
  // The exact output required by the 42 subject for ex01.
  banner("9. 42 subject reference output");
  {
    Fixed a;
    Fixed const b(10);
    Fixed const c(42.42f);
    Fixed const d(b);

    a = Fixed(1234.4321f);

    std::cout << "a is " << a << std::endl;
    std::cout << "b is " << b << std::endl;
    std::cout << "c is " << c << std::endl;
    std::cout << "d is " << d << std::endl;

    std::cout << "a is " << a.toInt() << " as integer" << std::endl;
    std::cout << "b is " << b.toInt() << " as integer" << std::endl;
    std::cout << "c is " << c.toInt() << " as integer" << std::endl;
    std::cout << "d is " << d.toInt() << " as integer" << std::endl;
  }

  std::cout << "\n";
  return 0;
}