// Plain C++98 assert-style tests for swap/min/max templates.
// Exits non-zero on first failure.

#include <iostream>
#include <string>

#include "whatever.hpp"

static int g_fail = 0;

#define CHECK(expr)                                              \
  do {                                                           \
    if (!(expr)) {                                               \
      std::cout << "FAIL: " << #expr << " (line " << __LINE__    \
                << ")" << std::endl;                             \
      ++g_fail;                                                  \
    }                                                            \
  } while (0)

int main(void) {
  // int
  int a = 2, b = 3;
  ::swap(a, b);
  CHECK(a == 3 && b == 2);
  CHECK(::min(a, b) == 2);
  CHECK(::max(a, b) == 3);

  // equal values: min/max must return the SECOND argument (by address)
  int x = 5, y = 5;
  CHECK(&::min(x, y) == &y);
  CHECK(&::max(x, y) == &y);

  // double
  double f = 1.5, g = -2.5;
  ::swap(f, g);
  CHECK(f == -2.5 && g == 1.5);
  CHECK(::min(f, g) == -2.5);
  CHECK(::max(f, g) == 1.5);

  // std::string
  std::string c = "chaine1", d = "chaine2";
  ::swap(c, d);
  CHECK(c == "chaine2" && d == "chaine1");
  CHECK(::min(c, d) == "chaine1");
  CHECK(::max(c, d) == "chaine2");

  if (g_fail) {
    std::cout << g_fail << " check(s) failed" << std::endl;
    return 1;
  }
  std::cout << "all tests passed" << std::endl;
  return 0;
}
