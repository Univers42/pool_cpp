/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 18:50:34 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/04 16:32:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Fixed.hpp"


/**
 * @file    Fixed.cpp
 * @brief   Deep-dive explanation of Fixed-point using binary approach as CPU.
 *
 * @details
 * This file documents the algorithms of operotars overriden
 * by binary operation as CPU does
 * No hardware divide instruction is used. Instead the function re-implements
   the same way CPUs do it in silicon.
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * BACKGROUND: What is a Q24.8 fixed-point number?
 * ─────────────────────────────────────────────────────────────────────────────
 * A `Fixed` stores one `int` called `_fixedPointValue`.
 * The integer is split conceptually into two parts:
 *
 *  Bit 31 … Bit 8   →  integer part  (24 bits, signed)
 *  Bit  7 … Bit 0   →  fractional part (8 bits)
 *
 * To convert a real number  R  to its raw integer:
 * @code
 *   raw = (int) round(R × 2^8)   = (int) round(R × 256)
 * @endcode
 *
 * To convert back:
 * @code
 *   R = raw / 256.0
 * @endcode
 *
 * Examples:
 *   +6.62 →  6.62 × 256 =  1694.72 → raw =  1695
 *   -1.42 → -1.42 × 256 = -363.52  → raw =  -364
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * WHY LONG DIVISION? (and not just raw_a / raw_b?)
 * ─────────────────────────────────────────────────────────────────────────────
 * Naively:  result_raw = raw_a / raw_b
 *   = 1695 / (-364) = -4   ← WRONG!  Loses all fractional precision.
 *
 * Correct approach — shift numerator left by _fractionalBits first:
 *   result_raw = (raw_a × 2^8) / raw_b
 *             = (1695 × 256) / 364
 *             = 433920 / 364
 *             ≈ 1191.5...  → raw = 1192  (absolute value, sign handled separately)
 *   then negate:  result_raw = -1192
 *   back to float: -1192 / 256 = -4.65625  ✓
 *
 * The long-division loop implements  (raw_a × 2^8) / raw_b  WITHOUT using the
 * `/` operator, using only subtraction and left-shifts.
 */




























 
/* ── Pure Bitwise Logic Gates (Internal Helpers) ─────────────────────────── */

/**
 * TRACE: 15 + 23
 * 15 = 0000 1111
 * 23 = 0001 0111
 * 
 * MAPMIND:
 * Think of it like elementary school addition euclidian:
 * XOR = "add but ignore carry"
 * AND = "where carry happens"
 * shift = "carry moves left"
 * 
 * ITERATION:A:
 * carry = a & b;
 *    0000 1111
 * &  0001 0111
 * -------------
 *    0000 0111 (7)
 * 
 * a = a ^ b;
 *    0000 1111
 * ^  0001 0111
 * -------------
 *    0001 1000 (24)
 * 
 * b = carry << 1;
 * 
 * carry = 0000 0111
 * <<       1
 * ------------------
 * b =     0000 1110 (14)
 * 
 * a = 24
 * b = 14
 * 
 * ITERATION:B:
 * a = 0001 1000 (24)
 * b = 0000 1110 (14)
 * 
 *    0001 1000
 * &  0000 1110
 * --------------
 *    0000 1000      (8)
 * PARTIALSUM:
 *    0001 1000
 * ^  0000 1110
 * --------------
 *    0001 0110     (22)
 * 
 * SHIFT:
 * carry =  0000 1000
 * <<       1
 * -----------------
 * b =      0001 0000 (16)
 * 
 * State after iteration 2
 * a = 22
 * b = 16
 * 
 * ITERATION:C:
 * a = 0001 0110  (22)
 * b = 0001 0000  (16)
 * 
 *    0001 0110
 * &  0001 0000
 * ---------------
 *    0001 0000     (16)
 * 
 * PARTIALSUM:
 *    0001 0110
 * ^  0001 0000
 * --------------
 *    0000 0110   (6)
 * SHIFT:
 * carry = 0001 0000
 * <<         1
 * ------------------
 * 0010 0000 (32)
 * 
 * a = 6 (0000 0110)
 * b = 32 (0010 0000)
 * 
 *    0000 0110
 * &  0010 0000
 * ---------------
 *    0000 0000   (0)
 * 
 *    0000 0110
 * ^  0010 0000
 * ------------------
 *    0010 0110 (38)
 *
 * b = 0 << 1 = 0
 * if (b == 0) break;
 * return (a = 38);
 * 15 + 23 = 38
 */
static int bit_add(int a, int b) {
  while (b != 0) {
    int carry = a & b; // bits that will be overflow (1 + 1)
    a = a ^ b;         // sum WITHOUT carry
    b = carry << 1;    // move carry to the next higher bit
  }
  return a;
}

static int bit_is_positive(int n) {
  return ((n >> 31) & 1) ;
}

static long ll_bit_is_positive(int64_t n) {
  return (( n >> 63) & 1);
}

/**
 * TWOSCOMPLEMENT:
 * this is not a hack.
 * This is how CPU subtract
 * 
 * a -b = a + (~b + 1);
 * 
 * so In pleain data we have 
 * 15 = 0000 1111
 * 23 = 0001 0111
 * 
 * ~ flips every byte so it gives us
 * when b is equal to:
 * b =  0001 0111
 * ~b = 1110 1000     (-24)
 * ~b + 1 = 1110 1001 (-23)
 * 
 * |  bit |  value  |
 * |bit 7 |     -128|
 * |bit 6 |      +64|
 * |bit 5 |      +32|
 * |bit 4 |      +16|
 * |bit 3 |       +8|
 * |bit 2 |       +4|
 * |bit 1 |       +2|
 * |bit 0 |       +1|
 * 
 * ~b = -128 + 64 +32 + 8 = -24
 * now we undersand more easily why 
 * ~b + 1 = -23
 * BOOM!
 * 
 * so in general in bit every (~x + 1) = -x
 */
static int bit_sub(int a, int b) { return bit_add(a, bit_add(~b, 1)); }

//same as bit_add
static int64_t ll_add(int64_t a, int64_t b) {
  while (b != 0) {
    int64_t carry = a & b;
    a = a ^ b;
    b = carry << 1;
  }
  return a;
}

// same as bit_sub
static int64_t ll_sub(int64_t a, int64_t b) { return ll_add(a, ll_add(~b, 1)); }












/* ── Orthodox Canonical Form ─────────────────────────────────────────────── */


Fixed::Fixed() : _fixedPointValue(0) {}

/**
 * n * 256 because we know that _fractionalBits = 8
 */
Fixed::Fixed(const int n) : _fixedPointValue(n << _fractionalBits) {}

/**
 * This constructor here to scale the float numbers if needed
 */
Fixed::Fixed(const float f)
    : _fixedPointValue(roundf(f * (1 << _fractionalBits))) {}

// Deep copy of constructor
Fixed::Fixed(const Fixed& other) { *this = other; }

/**
 * overide per default to get this kind of result with compatibility for chain assignment
 * int a = b = c = 64;
 */
Fixed& Fixed::operator=(const Fixed& other) {
  if (this != &other) this->_fixedPointValue = other.getRawBits();
  return *this;
}

// Destructor empty as we don't allocate any memory
Fixed::~Fixed() {}












/* ── Getters / Setters / Converters ──────────────────────────────────────── */

int Fixed::getRawBits(void) const { return this->_fixedPointValue; }
void Fixed::setRawBits(int const raw) { this->_fixedPointValue = raw; }

/**
 * _fixedPointValue = 2048
 * the real number is 2048 / 256 = 8.0
 * s we need to `shrink` bakc by dividing by factor `1 << fractionalBits = 256`
 * 
 * 2048 / 256 = 8.0
 */
float Fixed::toFloat(void) const {
  return static_cast<float>(this->_fixedPointValue) / (1 << _fractionalBits);
}

// if  we take back the example above we can convert with static_cast
int Fixed::toInt(void) const { return static_cast<int>(this->toFloat()); }













/* ── Pure Bitwise Comparison Operators ───────────────────────────────────── */
/**
 * 
 */
/* ────────────────────────────────────────────────────────────────────────── */


/**
 * a ^ b == 0 --> all bits equal
 * !(0) = true
 * !(non-zero) -> false
 * so logically !(non-zero) -> false
 */
bool Fixed::operator==(const Fixed& other) const {
  return !(this->_fixedPointValue ^ other._fixedPointValue);
}

// same as before but without the ! as (0) == false
bool Fixed::operator!=(const Fixed& other) const {
  return (this->_fixedPointValue ^ other._fixedPointValue);
}

/**
 * this answer one question only : `<`
 * if `a < b` ?
 * but it does it without using `<`
 * 
 * WE ASSUME THAT IT'S A SIGNED 32-BIT INT (TWO'SCOMPLEMENT)
 * sign_a = (a >> 31) & 1;
 * sign_b = (b >> 31) & 1;
 * 
 * so positive = 0; negative = 1;
 * 
 * a = 10 -> sign_a = 0;
 * a = -1 -> sign_a = 1;
 * 
 * STEP2: subtract without -
 * bit_sub(a, b) -> a - b 
 * 
 * STEP3: detect "different signs"
 * 
 * 
 * CASE: 100 < 42 (should be false)
 * values in binary
 * ```bash
 * a = 100 =  00000000 00000000 00000000 01100100
 * b = 42 =   00000000 00000000 00000000 00101010
 * 
 * sign_a = (a >> 31) & 1;
 * sign_b = (b >> 31) & 1;
 * a >> 31 = 000...000 -> sign_a = 0
 * b >> 31 = 000...000 -> sign_b = 0 
 * 
 * both are positive
 * 100 - 42 = 58
 * 
 * 58 =      00000000 00000000 00000000 00111010
 * 
 * sign_diff = (58 >> 31) & 1;
 * 58 >> 31 & 1 --> sign_diff = 0
 * 
 * STEP3:
 * detect sign_difference:
 * sign_a ^ sign_b = 0 ^ 0 = 0;
 * 
 * final logic: ((sign_a ^ sign_b) & sign_a) | (~(sign_a ^ sign_b) & sign_diff);
 * (0 & 0) | (~0 & 0) = 0 | (1 & 0)
 * = 0, so (100 < 42) => false
 * 
 * CASE: -42 > 42
 * a = -42
 * b = 42
 * 32-bit signed integers, two's complement
 * 
 * a = -42 (00000000 00000000 00000000 00101010) ===TWOSCOMPLEMENT ===> 11111111 11111111 11111111 11010101 === INVERT + 1 ====> 11111111 11111111 11111111 11010110
 * so a = = -42 = 11111111 11111111 11111111 11010110
 * 
 * b = 42 = 00000000 00000000 00000000 00101010
 * 
 * sign_a = (a >> 31) & 1;
 * sign_b = (b >> 31) & 1;
 * 
 * a >> 31 = 1 -> sign_a = 1; so a is negative
 * b >> 31 = 0 -> sign_b = 0; so b is negative
 * 
 * -42 - 42 = -84
 * 
 * -84 = (11111111 11111111 11111111 10101100)
 * sign_diff = 1;
 * ```
 * 
 * sign_a ^ sign_b = 1 ^ 0 = 1;   // signs are diffrent
 * 
 * FINAL EXECUTION ((sign_a ^ sign_b) & sign_a) | (~(sign_a ^ sign_b) & sign_diff)
 * (1 & 1) | (~1 & 1)
 * = 1 | (0 & 1)
 * = 1, so -42 < 42  ==  true
 * 
 */
bool Fixed::operator<(const Fixed& other) const {
  int a = this->_fixedPointValue;
  int b = other._fixedPointValue;
  int sign_a = bit_is_positive(a);
  int sign_b = bit_is_positive(b);
  int sign_diff = bit_is_positive(bit_sub(a, b));
  return ((sign_a ^ sign_b) & sign_a) | (~(sign_a ^ sign_b) & sign_diff);
}

/**
 * This implementation reuses the `<` logic, but with roles swapped
 * CASE: 42 > 42
 * A = 42 = 00000000 00000000 00000000 00101010
 * B = 42 = 00000000 00000000 00000000 00101010
 * 
 * sign_a = 0;
 * sign_b = 0;
 * same sign
 * 
 * 0 = 00000000 0000 0000 0000 0000 0000 0000 0000 0000
 * sign_diff_ba = 0
 * ((sign_a ^ sign_b) & sign_b | (~(sign_a ^ sign_b) & sign_diff_ba))
 * (0 & 0) | (~0 & 0)
 * 0 | (1 &  0) = 0 so it' false
 */
bool Fixed::operator>(const Fixed& other) const {
  int a = this->_fixedPointValue;
  int b = other._fixedPointValue;
  int sign_a = bit_is_positive(a);
  int sign_b = bit_is_positive(b);
  int sign_diff_ba = bit_is_positive(bit_sub(b, a));
  return ((sign_a ^ sign_b) & sign_b) | (~(sign_a ^ sign_b) & sign_diff_ba);
}

// we use our custom operators especially created...
bool Fixed::operator<=(const Fixed& other) const {
  return (*this < other) || (*this == other);
}

// we use our custom operator especially created 
bool Fixed::operator>=(const Fixed& other) const {
  return (*this > other) || (*this == other);
}












/* ── Bitwise Arithmetic Operators ────────────────────────────────────────── */
/**
 * NOTICE: that the other and and this are her eto represent the number between
 * the operator 
 * basically this represent the number on the left and the other on the right
 * 
 * it works like this 
 * Fixed  a(54);
 * Fixed  b(64);
 * 
 * Fixed c = a + b;
 * 
 * this line:
 * a + b;     // is `syntactic sugar`
 * 
 * the ccompiler rewrites it into this exact call:
 * a.operator+(b);
 * 
 * Fixed  Fixed::operator+(const Fixed& other) const;
 * - `this` -> points to `a`
 * - `other` -> is `b`
 * 
 * we didn't pass `a` in argument because the compiler did it implicitly
 * 
 * it's teh same for any operatoor we creaet
 * 
 * |  Expresssion | Function call  |
 * |--------------|----------------|
 * |  a + b       | a.operators+(b)|
 * |  a < b       | a.operator<(b) |
 * |  a == b      | a.operator==(b)|
 * 
 * QUESTION: Do we  need to create one from scratch ?
 * yes, that's why `operator+` usually return a new Fixed
 * 
 * NOTE: Each object has its own memory
 * 
 */
/* ────────────────────────────────────────────────────────────────────────── */
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

/**
 * ALRIGHT BIG BOY operator
 * CASE: 42 x 2
 * _fractionalBits = 8
 * Fixed-point scale = 256
 * ll_add / ll_sub behave like + / - but bitwise
 * 64-bit signed integer (int64_t) => cross platform better than long or long long
 * Fixed a(42)
 * Fixed b(2)
 * GOAL: Fixed c = a * b; // expecct 84;
 * 
 * STEPS:
 * FIXED-POINT REPRESENTATION FIRST
 * before multiplication even starts, constructor already did this:
 * 42 -> 42 * 256 = 10752
 * 2 -> 2 * 256 = 512
 * 
 * so internally
 * a_fixedPointValue = 10572
 * b._fixedPointValue = 512
 * 
 * 
 * ENTER OPERATOR*
 * fixed res;
 * int64_t a = this->_fixedPointValue;  // 10752
 * int64_t b = other._fixedPointValue; //   512
 * 
 * EXTRACT SIGNS
 * int64_t sign_a = (a >> 63) & 1;
 * int64_t sign_b = (b >> 63) & 1;
 * 
 * sign_a = 0
 * sign_b = 0
 * final_sign = 0 ^ 0 = 0, from now on we already know that the result will be positive
 * 
 * NORMALIZE TO POSITIVE so far nothing to do..
 * if (sign_a) a = ll_sub(0, a);
 * if (sign_b) b = ll_sub(0, b);
 * > nothing happens because we knew that they were positive both
 * 
 * COMPUTE: This is binary multiplication, low how CPUs do it
 * product = 0;
 * a = 10572
 * b = 512 ==> (0000 0010 0000 0000)
 * 
 * |  Step  | b   |  b & 1   |   product   | a (shifted)  |
 * |       1| 512 |         0|       0     |    21504     |
 * |       2| 256 |         0|       0     |  43008       |
 * |       3| 128 |         0|       0     |  866016      |
 * |       4| 64  |         0|       0     |  172032      |
 * |       5| 32  |         0|       0     |  344064      |
 * |       6| 16  |         0|       0     |  688128      |
 * |       7| 8   |         0|       0     | 1376256      |
 * |       8| 4   |         0|       0     |  2752542     |
 * |       9| 2   |         0|       0     |  5505024     |
 * |      10| 1   |         1|   +55055024 |  11010048    |
 * |      11| 0   | -       |     5505024  |  --          |
 * 
 * this equals to 1024 * 512 = 5, 505, 024
 * (42 * 256) * (2 * 256) = (424 * 2) * 256^2
 * but fixed-point result must be:
 * (42 * 2) * 256
 * so we must divide by 256 once
 * 
 * ROUNDING LOGIC
 * int64_t rounding_bit = (product >> (_fractionalBits - 1) & 1);
 * that is :
 * (product >> 7) & 1
 * 
 * product = 5,505,24
 * product / 128 = 43,008
 * LSB is `0`, so :
 * `rounding_bit` = 0;
 * 
 * 
 * SCALE BACK DOWN
 * product >>= _fractionalBits; // >> 8
 * 5,505,024 / 256 =  21,504
 * product = 21,504
 * 
 * product = ll_add(product, rounding_bit);
 * no change here.
 * 
 * RESTORE sin (still positive)
 * if (final_sign)
 *  product = ll_sub(0, product);
 * 
 * res.swtRawBits(static_cast<int>(product));
 * res._fixedPointValue = 21,504
 * CONVERT:
 * res.toFloat() = 215054 / 256 = 84.0
 * 
 * Fixed multiply steps:
 * 1) multiply raw integers (gives scale^2)
 * 2) Shift right by fractional bits (/ 256)
 * 3) Round
 * 4(Restore sign)
 * 5) Store result
 * 
 * |Step          | Value     |
 * |Raw inputs    |10752 * 512|
 * |RAw product   |5,504,024  |
 * |after scaling |21,504     |
 * |final float   |84.0       |
 * 
 * CASE: -42,42 * 4.62 Fixed-point multiplication logic.
 * 
 * raw = round(f x 256)
 * for `-42.42`:
 * -42.42 x 256 = -10849.92 ~= -10850
 * for `4.62`:
 * 4.62 x 256 = 1183.68 ~= 1184
 * 
 * So internally:
 * ```
 * a._fixedPointValue = -10850
 * b._fixedPointValue = 1184
 * ```
 * 
 * extract signs
 * int64_t sign_a = (a >> 63) & 1;  // 1 (negative)
 * int64_t sign_b = (b >> 63) & 1;  // 0 (positive)
 * int64_t final_sign = sign_a ^ sign_b;  // 1 (result will be negative)
 * 
 * NORMALIZE:
 * if (sign_a) a = ll_sub(0, a); (-10850 -> 10850)
 * if (sign_b) = ll_sub(0, b); // b is positive. so it stays the same
 * 
 * this is binary long multiplication..
 * b = 1184 (0100 1000 0000)
 * only the bit set to 1 trigger additions
 * shifting `a` left multiplies by powers of 2
 * 
 * we can approximate the product:
 * 10850 x 1184 = 12,846,400
 * NOTE: This is still in `raw scale`, i.e. multiplied by 256^2
 * ROUNDING:
 * rounding_bit = (product >> (_fractionalBits - 1)) & 1
 * shift 12,846,400 >> 7 = 100,357
 * LSB = 1 -> rounding_bit = 1
 * SCALEBACK:
 * product >>= _fractionalBits
 * product = ll_add(product, rounding_bit)
 * 12,856,400 >> 8 = 50,210
 * add rounding bit:
 * 50,210 + 1 = 50,211
 * 
 * RESTORE:
 * if (final_sign) product = ll_sub(0, product);
 * final_sign = 1 -> product = -50,211
 * 
 * STORE:
 * res.setRawbits(static_cast<int>(product))
 * CONVERT: convert back to float for human readable
 * res.toFloat() = product / 256.0;
 * -50.211/256 = -196.14
 * 
 * OBSERVATION:
 * - actual float multiplication: `42.42 x 4.62 = -195.7404`
 * - Fixed-Point (8 fractional bits) result: `196.14`
 * Difference ~= 0.4 -> small fixed-point rounding error
 * NOTE:Fixed-point is deterministic but has limited precision (1/256 ~= 0.0039 per step)
 * 
 * SUMMARY:
 * |Step                | Value                     |
 * |input floats        | -42.42, 4.62              |
 * |Raw fixed-point     | -10850, 1184              |
 * |Sign extraction     | a:1 b:0, result:negative  |
 * |Normalize positive  | 10850, 1184               |
 * |Multiply            | 12,846,400                |
 * |Rounding            | +1                        |
 * |Scale back          | 50,211                    |
 * |restore sign        | -50,211                   |
 * |convert to float    | -196.14                   |
 * 
 * 
 */
Fixed Fixed::operator*(const Fixed& other) const {
  Fixed res;
  int64_t a = this->_fixedPointValue;
  int64_t b = other._fixedPointValue;
  int64_t sign_a = ll_bit_is_positive(a);
  int64_t sign_b = ll_bit_is_positive(b);
  int64_t final_sign = sign_a ^ sign_b;   // check identity if sign are different etc..
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

/**
 * @brief   Divide two Fixed-point numbers using binary long division.
 *
 * @param   other   The divisor (right-hand side of `/`).
 * @return  Fixed   A new Fixed containing the quotient, correctly rounded.
 *
 * @note    `_fractionalBits` = 8 throughout this explanation.
 *          Helper functions used (all avoid the `/` operator):
 *          - `ll_sub(a, b)`  →  a - b  (64-bit)
 *          - `ll_add(a, b)`  →  a + b  (64-bit)
 *          - `bit_sub(i, 1)` →  i - 1  (integer)
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * STEP 1 — Guard against division by zero
 * ─────────────────────────────────────────────────────────────────────────────
 * @code
 *   if (other._fixedPointValue == 0) { return Fixed(0); }
 * @endcode
 *
 * In our example  other._fixedPointValue = -364 ≠ 0,  so we continue.
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * STEP 2 — Copy raw values into 64-bit integers
 * ─────────────────────────────────────────────────────────────────────────────
 * @code
 *   int64_t a = this->_fixedPointValue;   //  a = +1695
 *   int64_t b = other._fixedPointValue;   //  b =  -364
 * @endcode
 *
 * We work in 64 bits to prevent overflow when we later shift `a` left by 8
 * (multiplying by 256 can exceed 32-bit range for large numbers).
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * STEP 3 — Extract and compute the sign of the result
 * ─────────────────────────────────────────────────────────────────────────────
 * @code
 *   int64_t sign_a    = (a >> 63) & 1;   //  0  (positive)
 *   int64_t sign_b    = (b >> 63) & 1;   //  1  (negative)
 *   int64_t final_sign = sign_a ^ sign_b; //  0 XOR 1 = 1  → result is negative
 * @endcode
 *
 * Truth table for XOR (same as the sign rule for multiplication/division):
 * @verbatim
 *   sign_a  sign_b  final_sign  meaning
 *     0       0        0        (+) / (+) = (+)
 *     0       1        1        (+) / (-) = (-)   ← our case
 *     1       0        1        (-) / (+) = (-)
 *     1       1        0        (-) / (-) = (+)
 * @endverbatim
 *
 * Arithmetic right-shift fills with sign bit, so `(a >> 63) & 1` is a
 * portable way to read the sign bit on any two's-complement platform.
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * STEP 4 — Work with absolute values
 * ─────────────────────────────────────────────────────────────────────────────
 * @code
 *   if (sign_a) a = ll_sub(0, a);   //  a was positive → no change  → a = +1695
 *   if (sign_b) b = ll_sub(0, b);   //  b was negative → b = 0 - (-364) = +364
 * @endcode
 *
 * Now both `a` and `b` are strictly positive. The long-division loop below
 * works only with positive numbers; the sign is restored in Step 7.
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * STEP 5 — Prepare the dividend (scale by 2^_fractionalBits)
 * ─────────────────────────────────────────────────────────────────────────────
 * @code
 *   int64_t dividend = a << _fractionalBits;   //  1695 << 8 = 1695 × 256 = 433920
 *   int64_t divisor  = b;                      //  364
 *   int64_t quotient = 0;
 * @endcode
 *
 * Why shift?  Recall:  result_raw = (a × 2^8) / b
 * We bake the × 2^8 into the dividend so the quotient we compute IS the raw
 * fixed-point result, without any extra scaling step.
 *
 * Sanity check:  433920 / 364 = 1191.538…  ≈ 1192  (we expect 1192 below)
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * STEP 6 — Binary long-division loop
 * ─────────────────────────────────────────────────────────────────────────────
 * @code
 *   for (int i = 39; i >= 0; i = bit_sub(i, 1)) {
 *       int64_t shifted_divisor = divisor << i;
 *       int64_t diff            = ll_sub(dividend, shifted_divisor);
 *       if (((diff >> 63) & 1) == 0) {   // diff ≥ 0  → bit i fits
 *           dividend = diff;
 *           quotient = ll_add(quotient, (int64_t)1 << i);
 *       }
 *   }
 * @endcode
 *
 * CONCEPT — this is "binary long division", exactly like decimal long division
 * but in base 2.  At each step we ask:
 *   "Can I subtract  (divisor × 2^i)  from what's left of the dividend?"
 * If yes → set bit i of the quotient, update the remaining dividend.
 * If no  → bit i is 0, move on.
 *
 * Why start at i=39?
 *   dividend can be at most  ≈ INT32_MAX × 256 ≈ 5.5 × 10^11
 *   2^39 = 5.5 × 10^11  → so 39 bits is the highest that could ever matter.
 *
 * @verbatim
 * TRACE (only iterations where the bit is SET are shown in detail):
 *
 *   Initial state:
 *     dividend = 433920
 *     divisor  = 364
 *     quotient = 0
 *
 *   i = 39 … 11  → shifted_divisor > 433920  → diff < 0 → skip all
 *
 *   ── i = 10 ──────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 10 = 364 × 1024 = 372736
 *   diff            = 433920 - 372736 = +61184   (≥ 0 ✓)
 *   → dividend = 61184
 *   → quotient = 0 + (1 << 10) = 1024
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   ── i = 9 ───────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 9 = 186368
 *   diff            = 61184 - 186368 = -125184   (< 0 ✗ → skip)
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   ── i = 8 ───────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 8 = 93184
 *   diff            = 61184 - 93184 = -32000    (< 0 ✗ → skip)
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   ── i = 7 ───────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 7 = 46592
 *   diff            = 61184 - 46592 = +14592    (≥ 0 ✓)
 *   → dividend = 14592
 *   → quotient = 1024 + (1 << 7) = 1024 + 128 = 1152
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   ── i = 6 ───────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 6 = 23296
 *   diff            = 14592 - 23296 = -8704     (< 0 ✗ → skip)
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   ── i = 5 ───────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 5 = 11648
 *   diff            = 14592 - 11648 = +2944     (≥ 0 ✓)
 *   → dividend = 2944
 *   → quotient = 1152 + (1 << 5) = 1152 + 32 = 1184
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   ── i = 4 ───────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 4 = 5824
 *   diff            = 2944 - 5824 = -2880       (< 0 ✗ → skip)
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   ── i = 3 ───────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 3 = 2912
 *   diff            = 2944 - 2912 = +32         (≥ 0 ✓)
 *   → dividend = 32
 *   → quotient = 1184 + (1 << 3) = 1184 + 8 = 1192
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   ── i = 2 ───────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 2 = 1456
 *   diff            = 32 - 1456 = -1424         (< 0 ✗ → skip)
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   ── i = 1 ───────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 1 = 728
 *   diff            = 32 - 728 = -696           (< 0 ✗ → skip)
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   ── i = 0 ───────────────────────────────────────────────────────────────
 *   shifted_divisor = 364 << 0 = 364
 *   diff            = 32 - 364 = -332           (< 0 ✗ → skip)
 *   ─────────────────────────────────────────────────────────────────────────
 *
 *   Loop ends.
 *   quotient (absolute) = 1192
 *
 * Binary representation of 1192:
 *   1192 = 1024 + 128 + 32 + 8
 *        = 2^10 + 2^7 + 2^5 + 2^3
 *        = 0b 0000 0100 1010 1000
 *               ^    ^  ^ ^
 *               10   7  5 3   ← the four bits we set
 * @endverbatim
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * STEP 7 — Apply the sign determined in Step 3
 * ─────────────────────────────────────────────────────────────────────────────
 * @code
 *   if (final_sign) quotient = ll_sub(0, quotient);
 * @endcode
 *
 * `final_sign` = 1 (result should be negative).
 * @code
 *   quotient = 0 - 1192 = -1192
 * @endcode
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * STEP 8 — Store the raw bits and return
 * ─────────────────────────────────────────────────────────────────────────────
 * @code
 *   res.setRawBits(static_cast<int>(quotient));   // stores -1192
 *   return res;
 * @endcode
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * FINAL VERIFICATION
 * ─────────────────────────────────────────────────────────────────────────────
 * @verbatim
 *   raw result  = -1192
 *   float value = -1192 / 256.0 = -4.65625
 *
 *   True value  = 6.62 / -1.42 = -4.66197...
 *   Error       = |-4.65625 - (-4.66197)| = 0.00572
 *
 *   The error is less than 1 ULP (unit in the last place):
 *   1 ULP = 1/256 = 0.00390625
 *   ... actually ~1.5 ULP — the extra half-ULP is because there is no
 *   rounding step (truncation only). Adding rounding (if dividend > 0
 *   after the loop, and the next bit would be 1) would reduce to ≤ 0.5 ULP.
 * @endverbatim
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * COMPLEXITY SUMMARY
 * ─────────────────────────────────────────────────────────────────────────────
 * @verbatim
 *   Iterations   : always exactly 40  (i = 39 down to 0)
 *   Operations   : 40 × (1 shift + 1 subtract + 1 compare + conditional add)
 *   No hardware `/` used anywhere — pure shifts and adds.
 * @endverbatim
 */
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


//pre-increment
//increment _fixedPointValue directly
//return a reference to the same object
//efficient, used in loop or when we want the new value immediately
//++x
Fixed& Fixed::operator++(void) {
  this->_fixedPointValue = bit_add(this->_fixedPointValue, 1);
  return *this;
}

//post-increment x++
Fixed Fixed::operator++(int) {
  Fixed tmp(*this);   // copy current state
  operator++();       // call pre-increment to actually increment
  return tmp;         // return  the old value
}

//pre-decrement --x
Fixed& Fixed::operator--(void) {
  this->_fixedPointValue = bit_sub(this->_fixedPointValue, 1);
  return *this;
}

//post-decrement x--
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
