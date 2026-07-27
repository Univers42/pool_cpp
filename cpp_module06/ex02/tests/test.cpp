// Standalone C++98 test for cpp_module06/ex02.
// Checks that identify() finds the real dynamic type via dynamic_cast,
// through both the pointer and the reference overloads, and that
// generate() only ever produces A, B, or C. Exits non-zero on failure.

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include "A.hpp"
#include "B.hpp"
#include "C.hpp"
#include "Functions.hpp"

// Capture what identify(Base*) prints.
static std::string idPtr(Base* p) {
  std::ostringstream out;
  std::streambuf* old = std::cout.rdbuf(out.rdbuf());
  identify(p);
  std::cout.rdbuf(old);
  return out.str();
}

// Capture what identify(Base&) prints.
static std::string idRef(Base& p) {
  std::ostringstream out;
  std::streambuf* old = std::cout.rdbuf(out.rdbuf());
  identify(p);
  std::cout.rdbuf(old);
  return out.str();
}

int main() {
  A a;
  B b;
  C c;

  // Pointer overload identifies the real type.
  assert(idPtr(&a) == "A\n");
  assert(idPtr(&b) == "B\n");
  assert(idPtr(&c) == "C\n");

  // Reference overload agrees.
  assert(idRef(a) == "A\n");
  assert(idRef(b) == "B\n");
  assert(idRef(c) == "C\n");

  // Upcast through Base loses nothing: dynamic type still found.
  Base* base = &b;
  assert(idPtr(base) == "B\n");
  assert(idRef(*base) == "B\n");

  // generate() only produces A, B, or C; both overloads always agree.
  std::srand(static_cast<unsigned int>(std::time(NULL)));
  for (int i = 0; i < 20; ++i) {
    std::ostringstream chatter;  // swallow generate()'s announce line
    std::streambuf* old = std::cout.rdbuf(chatter.rdbuf());
    Base* p = generate();
    std::cout.rdbuf(old);
    std::string got = idPtr(p);
    assert(got == "A\n" || got == "B\n" || got == "C\n");
    assert(got == idRef(*p));
    delete p;
  }

  std::cout << "ex02 tests passed" << std::endl;
  return 0;
}
