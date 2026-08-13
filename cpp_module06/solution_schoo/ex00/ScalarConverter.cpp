/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScalarConverter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:43:40 by marvin            #+#    #+#             */
/*   Updated: 2025/12/25 21:43:40 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ScalarConverter.hpp"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

// Every helper below is `static`: internal linkage, so none of them leak out
// as symbols. Parsing is implementation detail, not API.

static bool isCharLiteral(const std::string& s) {
  if (s.length() == 3 && s[0] == '\'' && s[2] == '\'') return true;
  return s.length() == 1 && std::isprint(static_cast<unsigned char>(s[0])) &&
         !std::isdigit(static_cast<unsigned char>(s[0]));
}

static bool isPseudoLiteral(const std::string& s) {
  return s == "nan" || s == "nanf" || s == "+inf" || s == "+inff" ||
         s == "-inf" || s == "-inff" || s == "inf" || s == "inff";
}

// Grammar: [+-]? ( digit | '.' )* 'f'?  with at least one digit and at most
// one dot. One pass recognises int (42), double (4.2) and float (4.2f, 42f)
// literals, which previously took three near-identical scanners.
//
// Deliberately stricter than strtod, and that is the whole job: strtod would
// also swallow hex (0x5 -> 5), exponents (1e10), leading blanks and
// "infinity". This function is the gate that keeps them out.
static bool isNumericLiteral(const std::string& s) {
  size_t i = (!s.empty() && (s[0] == '+' || s[0] == '-')) ? 1 : 0;
  size_t end = s.length();
  if (end > i && s[end - 1] == 'f') --end;  // trailing float suffix
  bool dot = false, digit = false;
  for (; i < end; ++i) {
    if (s[i] == '.' && !dot)
      dot = true;
    else if (std::isdigit(static_cast<unsigned char>(s[i])))
      digit = true;
    else
      return (false);
  }
  return (digit);
}

// True when appending ".0" keeps the printed value honest: integral and
// small enough that cout's default 6-significant-digit format shows all
// digits (also rejects nan/inf).
// ponytail: >= 1e6 integral values print as-is (scientific), no ".0".
static inline bool wantsPointZero(double v) {
  return v == std::floor(v) && std::fabs(v) < 1e6;
}

// Finite and inside [lo, hi]. The !isnan test is belt-and-braces: NaN already
// compares false against both >= and <=, so it would be excluded without it
// (mutation-checked — removing it changes no output). Kept because "reject
// NaN" is the intent, and stating the intent beats relying on the reader
// knowing IEEE comparison rules.
static inline bool inRange(double v, double lo, double hi) {
  return (!std::isnan(v) && v >= lo && v <= hi);
}

static void printChar(double v) {
  if (!inRange(v, 0.0, 127.0))
    std::cout << "char: impossible" << std::endl;
  else if (!std::isprint(static_cast<int>(v)))
    std::cout << "char: Non displayable" << std::endl;
  else
    std::cout << "char: '" << static_cast<char>(v) << "'" << std::endl;
}

static void printInt(double v) {
#ifdef PROMOTION
  // WIDENING (int -> double): every int is representable in a double's 53-bit
  // mantissa, so the value cannot change and the conversion may be left
  // implicit. Compile with -DPROMOTION to build this variant.
  if (!inRange(v, std::numeric_limits<int>::min(),
               std::numeric_limits<int>::max()))
#else
  if (!inRange(v, static_cast<double>(std::numeric_limits<int>::min()),
               static_cast<double>(std::numeric_limits<int>::max())))
#endif
    std::cout << "int: impossible" << std::endl;
  else
    // NARROWING (floating-integral conversion, 4.9): double -> int truncates
    // and is undefined out of range. Never implicit, in any build.
    std::cout << "int: " << static_cast<int>(v) << std::endl;
}

static void printFloat(double v) {
  // NARROWING (float conversion, 4.8): double -> float loses precision, so the
  // cast is explicit and stays explicit under every build.
  float f = static_cast<float>(v);
  // PROMOTION (floating point promotion, 4.6): f is a float, wantsPointZero
  // takes a double. Widening float -> double is exact, so this is left
  // implicit even in the default build — this is the case the evaluation
  // sheet means by "accept implicit casts for promotion only".
  std::cout << "float: " << f << (wantsPointZero(f) ? ".0" : "") << "f"
            << std::endl;
}

static void printDouble(double v) {
  std::cout << "double: " << v << (wantsPointZero(v) ? ".0" : "") << std::endl;
}

// ponytail: one strtod funnel for every non-char literal. It parses the
// pseudo-literals natively (nan/nanf/+-inf/+-inff) and stops before a
// trailing 'f', so they need no special case; and a huge int literal
// saturates to a big double, letting printInt's range check report
// "impossible" instead of gambling on atoi's overflow UB.
void ScalarConverter::convert(const std::string& literal) {
  double v;
  if (isCharLiteral(literal))
#ifdef PROMOTION
    // PROMOTION then widening (char -> int -> double): both steps are exact,
    // so the value is preserved and the conversion may be left implicit.
    v = literal.length() == 1 ? literal[0] : literal[1];
#else
    v = static_cast<double>(literal.length() == 1 ? literal[0] : literal[1]);
#endif
  else if (isPseudoLiteral(literal) || isNumericLiteral(literal))
    v = std::strtod(literal.c_str(), NULL);
  else {
    std::cout << "char: impossible" << std::endl
              << "int: impossible" << std::endl
              << "float: impossible" << std::endl
              << "double: impossible" << std::endl;
    return;
  }
  printChar(v);
  printInt(v);
  printFloat(v);
  printDouble(v);
}
