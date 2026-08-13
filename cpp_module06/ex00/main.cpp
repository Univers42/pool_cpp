/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:45:03 by marvin            #+#    #+#             */
/*   Updated: 2025/12/25 21:45:03 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "ScalarConverter.hpp"

#ifdef PROMOTION

#include <limits>

// Built only with -DPROMOTION. The evaluation sheet says "accept the use of
// implicit casts for promotion casts only", so this shows where the line is.
//
// PROMOTION is a specific term in the standard, not a synonym for "cast":
//   - integral promotion   (4.5)  anything below int  -> int      always exact
//   - floating promotion   (4.6)  float               -> double   always exact
// Everything else that changes type is a CONVERSION (4.7 - 4.9) and may lose
// data. Implicit is defensible for the first group and never for the second.
static void promotionDemo() {
  std::cout << "=== PROMOTIONS — exact, so implicit is fine ===" << std::endl;

  char c = 'A';
  int promoted = c;  // 4.5 integral promotion, implicit
  std::cout << "  char 'A'      -> int    : " << promoted
            << "   (round trip: '" << static_cast<char>(promoted)
            << "') no loss" << std::endl;

  float f = 4.2f;
  double widened = f;  // 4.6 floating point promotion, implicit
  std::cout << "  float 4.2f    -> double : " << widened
            << "  (back to float: " << static_cast<float>(widened)
            << ") no loss" << std::endl;

  int i = std::numeric_limits<int>::max();
  double asDouble = i;  // widening, exact: 32 bits into a 53-bit mantissa
  std::cout << "  int INT_MAX   -> double : " << asDouble
            << " (back to int: " << static_cast<int>(asDouble) << ") no loss"
            << std::endl;

  std::cout << "\n=== CONVERSIONS — lossy, so static_cast is required ==="
            << std::endl;

  double d = 3.99;
  std::cout << "  double 3.99   -> int    : " << static_cast<int>(d)
            << "        truncates toward zero, .99 is gone" << std::endl;

  // 2^24 + 1 is the first integer a float cannot represent. Printed as whole
  // numbers, because cout's 6-significant-digit default would hide the loss.
  double big = 16777217.0;
  std::cout << "  double 2^24+1 -> float  : "
            << static_cast<long>(static_cast<float>(big)) << " but it was "
            << static_cast<long>(big) << "  <- one whole unit lost"
            << std::endl;

  int over = 300;
  std::cout << "  int 300       -> char   : "
            << static_cast<int>(static_cast<char>(over))
            << "       wrapped, 300 does not fit in a char" << std::endl;

  std::cout << "  double 1e20   -> int    : UNDEFINED BEHAVIOUR, so convert()"
            << "\n                            range-checks BEFORE casting"
            << std::endl;

  std::cout << "\nRule of thumb: if the value survives a round trip, it is a\n"
               "promotion and may stay implicit. If it cannot, it is a\n"
               "conversion and static_cast makes that visible.\n"
            << std::endl;
}

#endif  // PROMOTION

int main(int argc, char** argv) {
#ifdef PROMOTION
  promotionDemo();
#endif
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <literal>" << std::endl;
    return 1;
  }
  ScalarConverter::convert(argv[1]);
  return 0;
}
