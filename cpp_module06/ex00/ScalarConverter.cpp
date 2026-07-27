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

namespace {

bool isCharLiteral(const std::string& s) {
  if (s.length() == 3 && s[0] == '\'' && s[2] == '\'') return true;  // 'a'
  return s.length() == 1 && std::isprint(static_cast<unsigned char>(s[0])) &&
         !std::isdigit(static_cast<unsigned char>(s[0]));
}

bool isPseudoLiteral(const std::string& s) {
  return s == "nan" || s == "nanf" || s == "+inf" || s == "+inff" ||
         s == "-inf" || s == "-inff" || s == "inf" || s == "inff";
}

bool isIntLiteral(const std::string& s) {
  size_t i = (s[0] == '+' || s[0] == '-') ? 1 : 0;
  if (i >= s.length()) return false;
  for (; i < s.length(); ++i)
    if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
  return true;
}

bool isDoubleLiteral(const std::string& s) {
  size_t i = (s[0] == '+' || s[0] == '-') ? 1 : 0;
  bool dot = false, digit = false;
  for (; i < s.length(); ++i) {
    if (s[i] == '.') {
      if (dot) return false;
      dot = true;
    } else if (!std::isdigit(static_cast<unsigned char>(s[i]))) {
      return false;
    } else {
      digit = true;
    }
  }
  return dot && digit;
}

bool isFloatLiteral(const std::string& s) {
  if (s.length() < 2 || s[s.length() - 1] != 'f') return false;
  std::string num = s.substr(0, s.length() - 1);
  return isDoubleLiteral(num) || isIntLiteral(num);  // 4.2f and 42f
}

// True when appending ".0" keeps the printed value honest: integral and
// small enough that cout's default 6-significant-digit format shows all
// digits (also rejects nan/inf).
// ponytail: >= 1e6 integral values print as-is (scientific), no ".0".
bool wantsPointZero(double v) {
  return v == std::floor(v) && std::fabs(v) < 1e6;
}

void printChar(double v) {
  if (std::isnan(v) || v < 0.0 || v > 127.0)
    std::cout << "char: impossible" << std::endl;
  else if (!std::isprint(static_cast<int>(v)))
    std::cout << "char: Non displayable" << std::endl;
  else
    std::cout << "char: '" << static_cast<char>(v) << "'" << std::endl;
}

void printInt(double v) {
  if (std::isnan(v) ||
      v < static_cast<double>(std::numeric_limits<int>::min()) ||
      v > static_cast<double>(std::numeric_limits<int>::max()))
    std::cout << "int: impossible" << std::endl;
  else
    std::cout << "int: " << static_cast<int>(v) << std::endl;
}

void printFloat(double v) {
  float f = static_cast<float>(v);
  std::cout << "float: " << f;
  if (wantsPointZero(f)) std::cout << ".0";
  std::cout << "f" << std::endl;
}

void printDouble(double v) {
  std::cout << "double: " << v;
  if (wantsPointZero(v)) std::cout << ".0";
  std::cout << std::endl;
}

}  // namespace

void ScalarConverter::convert(const std::string& literal) {
  double v;
  if (isCharLiteral(literal)) {
    v = static_cast<double>(literal.length() == 1 ? literal[0] : literal[1]);
  } else if (isPseudoLiteral(literal)) {
    if (literal.find("nan") != std::string::npos)
      v = std::numeric_limits<double>::quiet_NaN();
    else if (literal[0] == '-')
      v = -std::numeric_limits<double>::infinity();
    else
      v = std::numeric_limits<double>::infinity();
  } else if (!literal.empty() && (isIntLiteral(literal) ||
                                  isFloatLiteral(literal) ||
                                  isDoubleLiteral(literal))) {
    // ponytail: one strtod funnel — stops before a trailing 'f', huge int
    // literals saturate to a big double so printInt reports "impossible"
    // instead of atoi's overflow UB.
    v = std::strtod(literal.c_str(), NULL);
  } else {
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
