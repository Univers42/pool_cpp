// Standalone C++98 test for ScalarConverter: captures convert()'s stdout and
// compares it against the exact expected block. Exits non-zero on failure.

#include <iostream>
#include <sstream>
#include <string>

#include "ScalarConverter.hpp"

static int g_failures = 0;

static void check(const std::string& literal, const std::string& expected) {
  std::ostringstream captured;
  std::streambuf* old = std::cout.rdbuf(captured.rdbuf());
  ScalarConverter::convert(literal);
  std::cout.rdbuf(old);
  if (captured.str() != expected) {
    ++g_failures;
    std::cout << "FAIL convert(\"" << literal << "\")\n--- got ---\n"
              << captured.str() << "--- want ---\n"
              << expected;
  }
}

int main() {
  // Subject's verbatim examples.
  check("0", "char: Non displayable\nint: 0\nfloat: 0.0f\ndouble: 0.0\n");
  check("nan", "char: impossible\nint: impossible\nfloat: nanf\ndouble: nan\n");
  check("42.0f", "char: '*'\nint: 42\nfloat: 42.0f\ndouble: 42.0\n");

  // Char literals, both accepted spellings.
  check("c", "char: 'c'\nint: 99\nfloat: 99.0f\ndouble: 99.0\n");
  check("'a'", "char: 'a'\nint: 97\nfloat: 97.0f\ndouble: 97.0\n");

  // Int and double literals, negatives truncate toward zero.
  check("-42", "char: impossible\nint: -42\nfloat: -42.0f\ndouble: -42.0\n");
  check("-4.2", "char: impossible\nint: -4\nfloat: -4.2f\ndouble: -4.2\n");

  // Pseudo-literals, float and double families.
  check("nanf",
        "char: impossible\nint: impossible\nfloat: nanf\ndouble: nan\n");
  check("+inff",
        "char: impossible\nint: impossible\nfloat: inff\ndouble: inf\n");
  check("-inf",
        "char: impossible\nint: impossible\nfloat: -inff\ndouble: -inf\n");

  // Overflow: beyond int range must be "impossible", never UB garbage.
  check("9999999999",
        "char: impossible\nint: impossible\nfloat: 1e+10f\ndouble: 1e+10\n");

  // Garbage input.
  check("abc",
        "char: impossible\nint: impossible\nfloat: impossible\n"
        "double: impossible\n");
  check("", "char: impossible\nint: impossible\nfloat: impossible\n"
            "double: impossible\n");

  if (g_failures != 0) {
    std::cout << g_failures << " check(s) failed" << std::endl;
    return 1;
  }
  std::cout << "test.cpp: all checks passed" << std::endl;
  return 0;
}
