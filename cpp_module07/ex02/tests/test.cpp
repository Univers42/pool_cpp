// Assert-based checks for Array<T>: size(), default init, deep copy,
// bounds-checked operator[] throwing std::exception, self-assignment,
// zero-size arrays, assignment from an empty array, nested arrays, and
// exception-safety when T's copy or default ctor throws
// (run under valgrind: a leak or double-free here is a test failure).
// Exits non-zero on failure via assert().
#include <cassert>
#include <exception>
#include <iostream>
#include <string>

#include "../Array.hpp"

// Bomb: copy-assignment throws when countdown hits 0 — detonates mid
// element-copy inside Array's copy ctor / operator=.
struct Bomb {
  static int countdown;
  int v;
  Bomb() : v(0) {}
  Bomb& operator=(const Bomb& o) {
    if (countdown > 0 && --countdown == 0) throw 42;
    v = o.v;
    return *this;
  }
};
int Bomb::countdown = -1;

// Mine: DEFAULT ctor throws — detonates inside new T[n]().
struct Mine {
  static int fuse;
  Mine() {
    if (fuse > 0 && --fuse == 0) throw 7;
  }
};
int Mine::fuse = -1;

// exercises the const operator[] overload
static bool throwsOutOfBounds(const Array<int>& a, unsigned int i) {
  try {
    (void)a[i];
  } catch (const std::exception&) {
    return true;
  }
  return false;
}

int main() {
  // empty array
  Array<int> empty;
  assert(empty.size() == 0);
  assert(throwsOutOfBounds(empty, 0));

  // n elements, default-initialized (new T[n]() zeroes ints)
  Array<int> a(5);
  assert(a.size() == 5);
  for (unsigned int i = 0; i < a.size(); ++i) assert(a[i] == 0);

  // deep copy: copy constructor
  a[0] = 42;
  Array<int> b(a);
  b[0] = -1;
  assert(a[0] == 42);
  assert(b[0] == -1);

  // deep copy: assignment operator
  Array<int> c;
  c = a;
  assert(c.size() == a.size());
  a[0] = 7;
  assert(c[0] == 42);

  // out of bounds throws, const and non-const paths
  assert(throwsOutOfBounds(a, a.size()));
  assert(throwsOutOfBounds(a, static_cast<unsigned int>(-2)));
  bool threw = false;
  try {
    a[a.size()] = 0;
  } catch (const std::exception&) {
    threw = true;
  }
  assert(threw);

  // works with a non-trivial type
  Array<std::string> s(2);
  s[0] = "forty";
  s[1] = "two";
  Array<std::string> sc(s);
  s[1] = "three";
  assert(sc[1] == "two");

  // self-assignment keeps contents
  a = a;
  assert(a.size() == 5 && a[0] == 7);

  // const in-bounds read goes through the const operator[] overload
  const Array<int>& cref = a;
  assert(cref[0] == 7);

  // Array(0): explicit zero size still allocates via new T[0](); every
  // access throws, and copying it takes cloneBuffer's n == 0 path
  Array<int> zero(0);
  assert(zero.size() == 0);
  assert(throwsOutOfBounds(zero, 0));
  Array<int> zeroCopy(zero);
  assert(zeroCopy.size() == 0);

  // assigning an empty array over a non-empty one must truly empty it
  // (regression: an operator= that early-returns on an empty rhs and keeps
  // the target's old size/contents passed the previous suite)
  b = empty;
  assert(b.size() == 0);
  assert(throwsOutOfBounds(b, 0));

  // nested arrays: deep copy through both layers
  Array<Array<int> > nest(2);
  nest[0] = a;
  Array<Array<int> > nest2(nest);
  nest2[0][0] = 999;
  assert(nest[0][0] == 7);

  // exception mid element-copy (operator= and copy ctor): no leak, no
  // dangling buffer, target of operator= keeps its old contents
  {
    Array<Bomb> x(4);
    Array<Bomb> y(4);
    Bomb::countdown = 3;
    threw = false;
    try {
      x = y;
    } catch (int) {
      threw = true;
    }
    Bomb::countdown = -1;
    assert(threw);
    Bomb::countdown = 2;
    threw = false;
    try {
      Array<Bomb> z(y);
      (void)z;
    } catch (int) {
      threw = true;
    }
    Bomb::countdown = -1;
    assert(threw);
  }

  // throwing DEFAULT ctor during operator='s allocation: strong guarantee
  {
    Array<Mine> x(3);
    Array<Mine> y(5);
    Mine::fuse = 2;
    threw = false;
    try {
      x = y;
    } catch (int) {
      threw = true;
    }
    Mine::fuse = -1;
    assert(threw);
    assert(x.size() == 3);
  }

  std::cout << "ex02 tests OK" << std::endl;
  return 0;
}
