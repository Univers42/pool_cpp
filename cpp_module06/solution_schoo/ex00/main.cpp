/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:45:03 by marvin            #+#    #+#             */
/*   Updated: 2026/08/11 00:00:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Two modes, on purpose:
//
//   ./ex00 <literal>   converts that literal and prints EXACTLY the four lines
//                      the subject specifies, with no decoration whatsoever.
//                      This is the mode an evaluator runs and diffs.
//
//   ./ex00             no argument: a guided tour over a curated set of
//                      literals, laid out as a table. Nothing here changes
//                      what convert() does — the tour just calls it repeatedly
//                      and tabulates the answers.
//
// Assertions live in tests/test.cpp (make test); this file is the readable
// proof.

// No STL containers and no algorithm header: the subject allows those only in
// Modules 08 and 09. std::string is fine — the subject's own convert()
// signature takes one. A plain array of four strings does the job here.
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "ScalarConverter.hpp"

static const char* RESET = "\033[0m";
static const char* BOLD = "\033[1m";
static const char* DIM = "\033[2m";
static const char* CYAN = "\033[96m";
static const char* GREEN = "\033[92m";
static const char* YELLOW = "\033[93m";

// ── Tiny presentation helpers ───────────────────────────────────────────────
static void rule(const char* color) {
  std::cout << color;
  for (int i = 0; i < 76; ++i) std::cout << "─";
  std::cout << RESET << std::endl;
}
static void section(int n, const std::string& title) {
  std::cout << std::endl
            << BOLD << CYAN << " " << n << " · " << title << RESET << std::endl;
  rule(DIM);
}
static void note(const std::string& text) {
  std::cout << DIM << "   " << text << RESET << std::endl;
}

// convert() always prints exactly four lines, so four strings is all we ever
// need — no container required.
struct Fields {
  std::string f[4];
};

// convert() writes to cout. Capturing it is what lets the tour lay the answers
// out as a table instead of 4 loose lines per literal.
static Fields convertToFields(const std::string& literal) {
  std::ostringstream out;
  std::streambuf* old = std::cout.rdbuf(out.rdbuf());
  ScalarConverter::convert(literal);
  std::cout.rdbuf(old);

  Fields fields;
  for (int i = 0; i < 4; ++i) fields.f[i] = "?";

  std::string line;
  std::istringstream in(out.str());
  for (int i = 0; i < 4 && std::getline(in, line); ++i) {
    std::string::size_type colon = line.find(": ");
    fields.f[i] = (colon == std::string::npos) ? line : line.substr(colon + 2);
  }
  return fields;
}

static void header() {
  std::cout << "   " << DIM << std::left << std::setw(15) << "literal"
            << std::setw(18) << "char" << std::setw(14) << "int"
            << std::setw(15) << "float" << "double" << RESET << std::endl;
}

static void tableRow(const std::string& literal) {
  Fields f = convertToFields(literal);
  std::string shown = literal.empty() ? "(empty)" : literal;
  std::cout << "   " << YELLOW << std::left << std::setw(15) << shown << RESET
            << std::setw(18) << f.f[0] << std::setw(14) << f.f[1]
            << std::setw(15) << f.f[2] << f.f[3] << std::endl;
}

// ── the guided tour ─────────────────────────────────────────────────────────
static void tour() {
  std::cout << std::endl
            << BOLD << " ex00 · CONVERSION OF SCALAR TYPES — static_cast"
            << RESET << std::endl;
  rule(CYAN);
  note("Run with an argument to convert a single literal:  ./ex00 42");

  section(1, "THE SUBJECT'S OWN EXAMPLES");
  header();
  tableRow("0");
  tableRow("nan");
  tableRow("42.0f");

  section(2, "CHAR LITERALS");
  note("A quoted char, or a lone printable non-digit. Note that '+' and '.'");
  note("are characters too, not malformed numbers.");
  header();
  tableRow("'a'");
  tableRow("c");
  tableRow("'0'");
  tableRow("+");
  tableRow(".fsdahdfkjahkjsd");

  section(3, "INT, FLOAT AND DOUBLE LITERALS");
  header();
  tableRow("42");
  tableRow("-42");
  tableRow("4.2");
  tableRow("4.2f");
  tableRow(".5");
  tableRow("4.");

  section(4, "THE PSEUDO-LITERALS");
  note("All eight forms. strtod parses them natively, so convert() needs no");
  note("special case — only a whitelist to keep \"infinity\" out.");
  header();
  tableRow("nan");
  tableRow("nanf");
  tableRow("inf");
  tableRow("inff");
  tableRow("+inf");
  tableRow("+inff");
  tableRow("-inf");
  tableRow("-inff");

  section(5, "THE BOUNDARIES WHERE A TARGET TYPE GIVES UP");
  note("31/32 is the printable edge; 127/128 is where char runs out;");
  note("2147483647/2147483648 is where int does.");
  header();
  tableRow("31");
  tableRow("32");
  tableRow("127");
  tableRow("128");
  tableRow("2147483647");
  tableRow("2147483648");
  tableRow("9999999999");

  section(6, "THE .0 SUFFIX CEILING");
  note("Below 1e6 an integral value prints with .0; at 1e6 cout switches to");
  note("scientific notation and the suffix would be a lie, so it is dropped.");
  header();
  tableRow("999999");
  tableRow("1000000");

  section(7, "WHAT IS REJECTED, AND WHY");
  note("strtod alone would happily accept every one of these. The validators");
  note("exist precisely to refuse them before strtod ever sees them.");
  header();
  tableRow("0x5");
  tableRow("1e10");
  tableRow("infinity");
  tableRow("++42");
  tableRow("4.2.3");
  tableRow("abc");
  tableRow("");

  std::cout << std::endl;
  rule(CYAN);
  std::cout << " " << GREEN << "✔" << RESET
            << " every conversion above is an explicit static_cast; the only"
            << std::endl
            << "   implicit one is float -> double, which is a promotion."
            << std::endl
            << std::endl;
}

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
  section(0, "PROMOTIONS — exact, so implicit is fine");

  char c = 'A';
  int promoted = c;  // 4.5 integral promotion, implicit
  std::cout << "   char 'A'      -> int    : " << promoted
            << "   (round trip: '" << static_cast<char>(promoted)
            << "') no loss" << std::endl;

  float f = 4.2f;
  double widened = f;  // 4.6 floating point promotion, implicit
  std::cout << "   float 4.2f    -> double : " << widened
            << "  (back to float: " << static_cast<float>(widened)
            << ") no loss" << std::endl;

  int i = std::numeric_limits<int>::max();
  double asDouble = i;  // widening, exact: 32 bits into a 53-bit mantissa
  std::cout << "   int INT_MAX   -> double : " << asDouble
            << " (back to int: " << static_cast<int>(asDouble) << ") no loss"
            << std::endl;

  section(0, "CONVERSIONS — lossy, so static_cast is required");

  double d = 3.99;
  std::cout << "   double 3.99   -> int    : " << static_cast<int>(d)
            << "        truncates toward zero, .99 is gone" << std::endl;

  // 2^24 + 1 is the first integer a float cannot represent. Printed as whole
  // numbers, because cout's 6-significant-digit default would hide the loss.
  double big = 16777217.0;
  std::cout << "   double 2^24+1 -> float  : "
            << static_cast<long>(static_cast<float>(big)) << " but it was "
            << static_cast<long>(big) << "  <- one whole unit lost"
            << std::endl;

  int over = 300;
  std::cout << "   int 300       -> char   : "
            << static_cast<int>(static_cast<char>(over))
            << "       wrapped, 300 does not fit in a char" << std::endl;

  std::cout << "   double 1e20   -> int    : UNDEFINED BEHAVIOUR, so convert()"
            << "\n                             range-checks BEFORE casting"
            << std::endl;

  note("");
  note("If the value survives a round trip it is a promotion and may stay");
  note("implicit. If it cannot, it is a conversion and static_cast makes");
  note("that visible to whoever reads the line next.");
}

#endif  // PROMOTION

int main(int argc, char** argv) {
#ifdef PROMOTION
  promotionDemo();
#endif
  if (argc != 2) {
    tour();
    return 0;
  }

  // Exactly the four lines the subject specifies. No colour, no framing.
  ScalarConverter::convert(argv[1]);

  return 0;
}
