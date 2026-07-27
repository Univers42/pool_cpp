// Standalone C++98 test for RPN: evaluates postfix expressions and
// verifies both correct results and error cases. Exits non-zero on failure.

#include <iostream>
#include <string>

#include "RPN.hpp"

static int g_failures = 0;

static void expectValue(const std::string& expr, int expected) {
  try {
    RPN rpn;
    int got = rpn.evaluate(expr);
    if (got != expected) {
      std::cerr << "FAIL: \"" << expr << "\" -> " << got << ", expected "
                << expected << std::endl;
      ++g_failures;
    }
  } catch (const std::exception&) {
    std::cerr << "FAIL: \"" << expr << "\" threw, expected " << expected
              << std::endl;
    ++g_failures;
  }
}

static void expectError(const std::string& expr) {
  try {
    RPN rpn;
    rpn.evaluate(expr);
    std::cerr << "FAIL: \"" << expr << "\" evaluated, expected Error"
              << std::endl;
    ++g_failures;
  } catch (const std::exception&) {
    // expected
  }
}

int main() {
  // Subject examples (verbatim).
  expectValue("8 9 * 9 - 9 - 9 - 4 - 1 +", 42);
  expectValue("7 7 * 7 -", 42);
  expectValue("1 2 * 2 / 2 * 2 4 - +", 0);

  // Operand order and arithmetic semantics.
  expectValue("3 4 -", -1);   // left - right, not right - left
  expectValue("8 3 /", 2);    // integer truncation
  expectValue("9 9 /", 1);    // left / right, not right / left
  expectValue("9 9 *", 81);   // operands < 10, results may exceed 9
  expectValue("0 5 -", -5);   // intermediate results may go negative
  expectValue("5", 5);        // single operand, no operator

  // Error cases: all must throw, never print a result.
  expectError("(1 + 1)");   // brackets (subject example)
  expectError("");          // empty expression
  expectError("1 +");       // missing operand
  expectError("+");         // operator on empty stack
  expectError("1 2");       // leftover operands
  expectError("5 0 /");     // division by zero
  expectError("0 0 /");     // 0/0 is division by zero too
  expectError("a b +");     // junk tokens
  expectError("1.5 2 +");   // decimals are not handled
  expectError("10 2 +");    // operands must be single digits
  expectError("-1 2 +");    // negative literals are not single digits
  expectError("\xe9 1 +");  // high-bit char: isdigit needs unsigned char

  // Object is reusable: state must not leak between evaluations.
  {
    RPN rpn;
    rpn.evaluate("1 1 +");
    if (rpn.evaluate("2 2 +") != 4) {
      std::cerr << "FAIL: stale stack state after previous evaluate"
                << std::endl;
      ++g_failures;
    }
  }

  if (g_failures) {
    std::cerr << g_failures << " test(s) failed" << std::endl;
    return 1;
  }
  std::cout << "All RPN tests passed" << std::endl;
  return 0;
}
