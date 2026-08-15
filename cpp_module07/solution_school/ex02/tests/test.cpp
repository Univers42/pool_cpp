/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:10:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/13 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Tests for Array<T>, fixed cases plus a fuzz run.
//
//   make test               build and run
//   ./build/bin/test 1234   different fuzz seed
//   ./tests/mutants.sh      breaks Array.hpp, these tests must fail
//   ./tests/compile_fail.sh the code that must NOT compile
//   valgrind --leak-check=full ./build/bin/test
//
// The half worth reading is the exception paths. Bomb and Mine throw on
// purpose and both count how many of them are alive, so a buffer that leaks
// because its owner never finished constructing shows up as a count that does
// not come back to zero. Still run valgrind: a counter cannot see a double
// free.

#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include "../Array.hpp"
#include "../bigint.hpp"
#include "../iter.hpp"
#include "../vect2.hpp"


// Small harness so a failure says which section it came from. All static,
// constructors private so nobody can build one.
class Check {
 public:
  static void section(const char* name) {
    if (g_sectionCount >= kMaxSections) return;
    Section& s = g_sections[g_sectionCount++];
    s.name = name;
    s.checks = 0;
    s.failed = 0;
  }

  static void ok(bool cond, const std::string& detail) {
    Section& s = g_sections[g_sectionCount - 1];
    ++s.checks;
    if (cond) return;
    ++s.failed;
    if (g_errorCount < kMaxErrors)
      g_errors[g_errorCount++] = std::string(s.name) + " - " + detail;
  }

  static int report(const char* title) {
    int checks = 0, failed = 0;
    std::cout << "\n" << title << "\n" << std::endl;
    for (int i = 0; i < g_sectionCount; ++i) {
      const Section& s = g_sections[i];
      // 48 - len is unsigned, so a long name would wrap and throw.
      size_t len = std::string(s.name).length();
      std::string dots(len < 48 ? 48 - len : 1, '.');
      std::cout << "  " << s.name << " " << dots << " " << s.checks << " "
                << (s.failed ? "FAILED" : "ok") << std::endl;
      checks += s.checks;
      failed += s.failed;
    }
    std::cout << std::endl;
    if (!failed) {
      std::cout << "  " << g_sectionCount << " sections, " << checks
                << " checks, all passed" << std::endl;
      return 0;
    }
    std::cout << "  " << failed << " of " << checks << " checks FAILED"
              << std::endl
              << std::endl;
    for (int i = 0; i < g_errorCount; ++i)
      std::cout << "    " << g_errors[i] << std::endl;
    return 1;
  }

 private:
  // Declared, never defined: an accidental Check c; fails at link time.
  Check();
  Check(const Check& other);
  Check& operator=(const Check& other);
  ~Check();

  struct Section {
    const char* name;
    int checks;
    int failed;
  };

  static const int kMaxSections = 20;
  static const int kMaxErrors = 12;

  static Section g_sections[kMaxSections];
  static int g_sectionCount;
  static std::string g_errors[kMaxErrors];
  static int g_errorCount;
};

Check::Section Check::g_sections[Check::kMaxSections];
int Check::g_sectionCount = 0;
std::string Check::g_errors[Check::kMaxErrors];
int Check::g_errorCount = 0;


// Bomb's COPY-ASSIGNMENT throws, detonating partway through a buffer copy -
// inside the copy constructor or operator=, after some elements are done.
// `alive` is the leak detector: if a half-built Array's buffer is abandoned,
// those Bombs are never destroyed and the count stays high.
struct Bomb {
  static int countdown;
  static int alive;
  int v;

  Bomb() : v(0) { ++alive; }
  Bomb(const Bomb& o) : v(o.v) { ++alive; }
  ~Bomb() { --alive; }
  Bomb& operator=(const Bomb& o) {
    if (countdown > 0 && --countdown == 0) throw std::string("boom");
    v = o.v;
    return *this;
  }
  static void disarm() { countdown = -1; }
};
int Bomb::countdown = -1;
int Bomb::alive = 0;

// Mine's DEFAULT constructor throws, detonating inside new T[n]() - before
// there is any buffer to copy into.
struct Mine {
  static int fuse;
  static int alive;
  int v;

  Mine() : v(0) {
    if (fuse > 0 && --fuse == 0) throw std::string("click");
    ++alive;
  }
  Mine(const Mine& o) : v(o.v) { ++alive; }
  ~Mine() { --alive; }
  Mine& operator=(const Mine& o) {
    v = o.v;
    return *this;
  }
  static void disarm() { fuse = -1; }
};
int Mine::fuse = -1;
int Mine::alive = 0;

// A plain counted type: no throwing, just a balance sheet.
// `alive` goes up and down; `constructed` only ever goes up, which is what
// makes "this operation allocated nothing" an assertable claim.
struct Tracked {
  static int alive;
  static int constructed;
  int v;
  Tracked() : v(0) {
    ++alive;
    ++constructed;
  }
  Tracked(const Tracked& o) : v(o.v) {
    ++alive;
    ++constructed;
  }
  ~Tracked() { --alive; }
  Tracked& operator=(const Tracked& o) {
    v = o.v;
    return *this;
  }
};
int Tracked::alive = 0;
int Tracked::constructed = 0;


// Exercises the CONST operator[] overload. Taking the Array by const& is the
// only way to make the const version actually run rather than merely compile.
static bool throwsOutOfBounds(const Array<int>& a, unsigned int i) {
  try {
    (void)a[i];
  } catch (const std::exception&) {
    return true;
  }
  return false;
}

static bool throwsOutOfBoundsMutable(Array<int>& a, unsigned int i) {
  try {
    a[i] = 0;
  } catch (const std::exception&) {
    return true;
  }
  return false;
}

static std::string contentsOf(const Array<int>& a) {
  std::ostringstream os;
  for (unsigned int i = 0; i < a.size(); ++i)
    os << a[i] << (i + 1 < a.size() ? "," : "");
  return os.str();
}


static void test_construction() {
  Check::section("construction: default, sized, and explicitly zero");

  Array<int> empty;
  Check::ok(empty.size() == 0, "the default constructor gives size 0");

  Array<int> a(5);
  Check::ok(a.size() == 5, "Array<int>(5) reports size 5");

  // new T[n]() value-initialises: this is the difference the () makes.
  bool allZero = true;
  for (unsigned int i = 0; i < a.size(); ++i)
    if (a[i] != 0) allZero = false;
  Check::ok(allZero, "every element is value-initialised to 0, not garbage");

  Array<int> zero(0);
  Check::ok(zero.size() == 0, "Array<int>(0) reports size 0");

  // A class element type must be default-constructed too.
  Array<std::string> words(3);
  Check::ok(words.size() == 3, "Array<std::string>(3) reports size 3");
  Check::ok(words[0].empty() && words[2].empty(),
        "class elements are default-constructed");

  Array<double> reals(2);
  Check::ok(reals[0] == 0.0 && reals[1] == 0.0, "doubles are zeroed too");

  // A big one, to catch anything that only works for small n.
  Array<int> big(1000);
  bool bigZero = true;
  for (unsigned int i = 0; i < big.size(); ++i)
    if (big[i] != 0) bigZero = false;
  Check::ok(big.size() == 1000 && bigZero, "1000 elements, all zeroed");
}


static void test_subscript() {
  Check::section("operator[]: reads, writes, and both bounds");

  Array<int> a(4);
  for (unsigned int i = 0; i < 4; ++i) a[i] = static_cast<int>(i) * 10;
  Check::ok(contentsOf(a) == "0,10,20,30", "writes land at the right indices");

  Check::ok(a[0] == 0 && a[3] == 30, "the first and last elements are reachable");

  // Out of bounds, non-const path.
  Check::ok(throwsOutOfBoundsMutable(a, 4), "index == size throws");
  Check::ok(throwsOutOfBoundsMutable(a, 100), "a far index throws");

  // Out of bounds, const path - a different function, easy to leave untested.
  Check::ok(throwsOutOfBounds(a, 4), "the const overload throws at index == size");
  const Array<int>& frozen = a;
  Check::ok(frozen[0] == 0, "the const overload reads in-bounds correctly");
  Check::ok(frozen[3] == 30, "and reaches the last element");

  // The index is unsigned, so -2 wraps to a huge value and the same
  // `index >= _size` check catches it. There is no separate negative case.
  Check::ok(throwsOutOfBounds(a, static_cast<unsigned int>(-2)),
        "a negative literal wraps and is caught by the same check");
  Check::ok(throwsOutOfBounds(a, static_cast<unsigned int>(-1)),
        "so is -1, the largest unsigned value");

  // An empty array has no valid index at all.
  Array<int> empty;
  Check::ok(throwsOutOfBounds(empty, 0), "index 0 throws on an empty array");
  Array<int> zero(0);
  Check::ok(throwsOutOfBounds(zero, 0), "index 0 throws on Array(0) too");

  // Writing through operator[] must persist.
  a[2] = 999;
  Check::ok(a[2] == 999, "operator[] returns a reference you can assign through");
}

static void test_exception_type() {
  Check::section("the exception: type, hierarchy, and message");

  Array<int> a(1);

  // Catchable as std::exception& - which is what the subject's main does.
  bool asStd = false;
  try {
    (void)a[5];
  } catch (const std::exception&) {
    asStd = true;
  }
  Check::ok(asStd, "OutOfBoundsException derives from std::exception");

  // Catchable as its exact nested type.
  bool asExact = false;
  std::string message;
  try {
    (void)a[5];
  } catch (const Array<int>::OutOfBoundsException& e) {
    asExact = true;
    message = e.what();
  }
  Check::ok(asExact, "it is catchable as Array<int>::OutOfBoundsException");
  Check::ok(message == "Error: Array index is out of bounds!",
        "what() is exactly the documented message, got: \"" + message + "\"");

  // The nested class is a member of each instantiation, so Array<string>'s
  // exception is a DIFFERENT type - but still a std::exception.
  bool stringVersion = false;
  try {
    Array<std::string> s(1);
    (void)s[9];
  } catch (const Array<std::string>::OutOfBoundsException&) {
    stringVersion = true;
  }
  Check::ok(stringVersion, "each instantiation has its own nested exception type");

  // Throwing must not corrupt the array.
  Array<int> b(3);
  b[0] = 7;
  try {
    (void)b[99];
  } catch (const std::exception&) {
  }
  Check::ok(b.size() == 3 && b[0] == 7, "a failed access left the array untouched");
}


static void test_deep_copy() {
  Check::section("deep copy through both copy paths");

  Array<int> source(3);
  source[0] = 1;
  source[1] = 2;
  source[2] = 3;

  // copy constructor
  Array<int> viaCtor(source);
  Check::ok(viaCtor.size() == 3, "the copy constructor copies the size");
  Check::ok(contentsOf(viaCtor) == "1,2,3", "and the contents");
  viaCtor[0] = -1;
  Check::ok(source[0] == 1, "editing the copy does not reach the source");
  source[1] = 20;
  Check::ok(viaCtor[1] == 2, "and editing the source does not reach the copy");

  // copy-initialisation syntax goes through the copy constructor too
  Array<int> viaEquals = source;
  Check::ok(contentsOf(viaEquals) == "1,20,3", "Array<int> b = a; deep-copies");

  // assignment operator
  Array<int> viaAssign;
  viaAssign = source;
  Check::ok(viaAssign.size() == 3, "operator= copies the size");
  Check::ok(contentsOf(viaAssign) == "1,20,3", "and the contents");
  viaAssign[2] = -3;
  Check::ok(source[2] == 3, "editing the assigned copy does not reach the source");

  // assignment returns *this, so chaining works
  Array<int> x, y;
  x = y = source;
  Check::ok(contentsOf(x) == "1,20,3" && contentsOf(y) == "1,20,3",
        "operator= returns *this, so a = b = c works");

  // a non-trivial element type must be deep-copied element by element
  Array<std::string> s(2);
  s[0] = "forty";
  s[1] = "two";
  Array<std::string> sc(s);
  s[1] = "three";
  Check::ok(sc[1] == "two", "std::string elements are copied, not shared");
}

static void test_assignment_resizes() {
  Check::section("assignment replaces size and contents, in every direction");

  Array<int> small(2);
  small[0] = 1;
  small[1] = 2;
  Array<int> large(5);
  for (unsigned int i = 0; i < 5; ++i) large[i] = 100 + static_cast<int>(i);

  // growing
  Array<int> a(small);
  a = large;
  Check::ok(a.size() == 5, "assigning a larger array grows the target");
  Check::ok(contentsOf(a) == "100,101,102,103,104", "with the right contents");

  // shrinking
  Array<int> b(large);
  b = small;
  Check::ok(b.size() == 2, "assigning a smaller array shrinks the target");
  Check::ok(contentsOf(b) == "1,2", "with the right contents");
  Check::ok(throwsOutOfBounds(b, 2), "the old, larger indices now throw");

  // to empty - the regression a mutant found: an operator= that early-returns
  // on an empty right-hand side keeps the target's old size and passes any
  // test that only ever assigns non-empty arrays.
  Array<int> c(large);
  Array<int> empty;
  c = empty;
  Check::ok(c.size() == 0, "assigning an empty array truly empties the target");
  Check::ok(throwsOutOfBounds(c, 0), "and every index now throws");

  // from empty
  Array<int> d;
  d = large;
  Check::ok(d.size() == 5, "an empty target can be assigned a full array");
  Check::ok(contentsOf(d) == "100,101,102,103,104", "with the right contents");

  // Array(0) on both sides
  Array<int> zero(0);
  Array<int> e(3);
  e = zero;
  Check::ok(e.size() == 0, "assigning Array(0) empties the target");
  Array<int> zeroCopy(zero);
  Check::ok(zeroCopy.size() == 0, "copying Array(0) takes cloneBuffer's n==0 path");
}

static void test_self_assignment() {
  Check::section("self-assignment must not free the buffer it is reading");

  Array<int> a(3);
  a[0] = 1;
  a[1] = 2;
  a[2] = 3;

  // Routed through a reference on purpose: writing `a = a` directly is a
  // compiler diagnostic (-Wself-assign-overloaded), not a realistic call. The
  // real-world shape of this bug is two names for one object.
  Array<int>& alias = a;
  a = alias;
  Check::ok(a.size() == 3, "self-assignment preserves the size");
  Check::ok(contentsOf(a) == "1,2,3", "and the contents");

  // Through a pointer, which is how it actually happens in the wild.
  Array<int>* self = &a;
  a = *self;
  Check::ok(contentsOf(a) == "1,2,3", "self-assignment through a pointer is safe");

  // Same for an empty array.
  Array<int> empty;
  Array<int>& emptyAlias = empty;
  empty = emptyAlias;
  Check::ok(empty.size() == 0, "self-assigning an empty array is safe");

  // What `if (this != &rhs)` is actually FOR, in this implementation.
  //
  // Correctness does not depend on it: cloneBuffer builds the new buffer
  // before `delete[] _array` runs, so even without the guard a self-assignment
  // reads live memory and produces the right answer. (Deleting the guard is an
  // equivalent mutant for every value-based check - verified under valgrind.)
  // What the guard buys is the work it skips, and that IS observable: with it,
  // self-assignment constructs nothing at all.
  Tracked::constructed = 0;
  Array<Tracked> tr(4);
  const int afterBuild = Tracked::constructed;
  Array<Tracked>& trAlias = tr;
  tr = trAlias;
  Check::ok(Tracked::constructed == afterBuild,
        "self-assignment constructed no new elements - the guard short-circuits");
  Check::ok(tr.size() == 4, "and the array is intact afterwards");
}

static void test_nested() {
  Check::section("Array<Array<int> >: deep copy through both layers");

  Array<int> inner(2);
  inner[0] = 1;
  inner[1] = 2;

  Array<Array<int> > outer(2);
  outer[0] = inner;
  outer[1] = inner;
  Check::ok(outer.size() == 2, "the outer array holds two inner arrays");
  Check::ok(outer[0][0] == 1 && outer[1][1] == 2, "chained subscripting works");

  Array<Array<int> > copy(outer);
  copy[0][0] = 999;
  Check::ok(outer[0][0] == 1, "editing the nested copy did not reach the original");
  Check::ok(copy[0][0] == 999, "and the copy really was edited");

  // The inner arrays start empty, so the outer default-construction works.
  Array<Array<int> > blank(3);
  Check::ok(blank[0].size() == 0, "inner arrays are default-constructed empty");
  Check::ok(throwsOutOfBounds(blank[0], 0), "and behave like any empty array");
}


static void test_element_lifetime() {
  Check::section("every element constructed is eventually destroyed");

  Tracked::alive = 0;
  {
    Array<Tracked> a(4);
    Check::ok(Tracked::alive == 4, "Array<Tracked>(4) constructed four elements");
    Array<Tracked> b(a);
    Check::ok(Tracked::alive == 8, "the copy constructed four more");
    Array<Tracked> c;
    c = b;
    Check::ok(Tracked::alive == 12, "assignment constructed four more");
    c = Array<Tracked>();
    Check::ok(Tracked::alive == 8, "assigning an empty array destroyed those four");
  }
  Check::ok(Tracked::alive == 0, "leaving the scope destroyed everything");

  // Array(0) still allocates via new T[0](), which must still be delete[]d.
  Tracked::alive = 0;
  {
    Array<Tracked> zero(0);
    Check::ok(Tracked::alive == 0, "Array<Tracked>(0) constructs no elements");
  }
  Check::ok(Tracked::alive == 0, "and destroying it is clean");
}


static void test_throwing_copy_assign() {
  Check::section("T's copy-assignment throws mid-copy: no leak, no change");

  Bomb::disarm();
  Bomb::alive = 0;

  {
    Array<Bomb> source(4);
    const int baseline = Bomb::alive;
    Check::ok(baseline == 4, "four Bombs are live before the copy");

    // Copy constructor: detonate on the 3rd element copy.
    Bomb::countdown = 3;
    bool threw = false;
    try {
      Array<Bomb> victim(source);
      (void)victim;
    } catch (const std::string&) {
      threw = true;
    }
    Bomb::disarm();
    Check::ok(threw, "the copy constructor propagated the exception");
    // THE leak test: the half-filled buffer belongs to an object whose
    // constructor never completed, so no destructor will ever run for it.
    // cloneBuffer's catch(...) { delete[] fresh; throw; } is what balances it.
    Check::ok(Bomb::alive == baseline,
          "the abandoned buffer was freed - no orphaned elements");

    // operator=: same defect, different entry point.
    Array<Bomb> target(4);
    target[0].v = 42;
    const int baseline2 = Bomb::alive;
    Bomb::countdown = 2;
    threw = false;
    try {
      target = source;
    } catch (const std::string&) {
      threw = true;
    }
    Bomb::disarm();
    Check::ok(threw, "operator= propagated the exception");
    Check::ok(Bomb::alive == baseline2, "operator= leaked nothing either");
    // Strong guarantee: the target is exactly as it was.
    Check::ok(target.size() == 4, "the target kept its original size");
    Check::ok(target[0].v == 42, "and its original contents");
  }
  Check::ok(Bomb::alive == 0, "all Bombs destroyed at the end of the scope");
}

static void test_throwing_default_ctor() {
  Check::section("T's default ctor throws in new T[n](): strong guarantee holds");

  Mine::disarm();
  Mine::alive = 0;

  {
    // Detonating inside Array(n) itself: the object never exists.
    Mine::fuse = 3;
    bool threw = false;
    try {
      Array<Mine> stillborn(5);
      (void)stillborn;
    } catch (const std::string&) {
      threw = true;
    }
    Mine::disarm();
    Check::ok(threw, "Array(n) propagated the exception out of new T[n]()");
    Check::ok(Mine::alive == 0,
          "array-new destroyed the elements it had already built");

    // Detonating inside operator='s allocation. This is the mirror of the
    // Bomb case: the old code freed _array BEFORE allocating, so a throw here
    // left _array dangling and the destructor freed it a second time.
    Array<Mine> target(3);
    target[0].v = 7;
    target[2].v = 9;
    const int baseline = Mine::alive;

    Mine::fuse = 2;
    threw = false;
    try {
      Array<Mine> source(5);
      target = source;
    } catch (const std::string&) {
      threw = true;
    }
    Mine::disarm();
    Check::ok(threw, "operator= propagated the exception");
    Check::ok(target.size() == 3, "the target still has its ORIGINAL size");
    Check::ok(target[0].v == 7 && target[2].v == 9,
          "and its original contents, element for element");
    Check::ok(Mine::alive == baseline, "nothing was orphaned");

    // The target must still be usable - not left holding a freed pointer.
    target[1].v = 5;
    Check::ok(target[1].v == 5, "the target is still writable afterwards");
    Array<Mine> afterwards(target);
    Check::ok(afterwards.size() == 3, "and still copyable");

    // Copy constructor with a throwing default ctor.
    Mine::fuse = 2;
    threw = false;
    try {
      Array<Mine> victim(target);
      (void)victim;
    } catch (const std::string&) {
      threw = true;
    }
    Mine::disarm();
    Check::ok(threw, "the copy constructor propagated it too");
  }
  Check::ok(Mine::alive == 0, "all Mines destroyed at the end of the scope");
}


// Random operation sequences checked against a plain-array model. STL
// containers are off-limits until module 08, so the model is a fixed C array
// plus an explicit length - which is also exactly what Array is meant to be.
static void test_fuzz(int iterations) {
  Check::section("fuzz: random operation sequences against a plain-array model");

  const unsigned int kMax = 24;
  bool allGood = true;
  std::string firstFailure;
  int sawEmpty = 0, sawFull = 0, sawThrow = 0;

  for (int round = 0; round < iterations; ++round) {
    const unsigned int n = static_cast<unsigned int>(std::rand()) % (kMax + 1);
    if (n == 0) ++sawEmpty;
    if (n == kMax) ++sawFull;

    int model[kMax];
    Array<int> a(n);

    bool good = true;

    // Freshly built: value-initialised to zero.
    for (unsigned int i = 0; i < n; ++i) {
      if (a[i] != 0) good = false;
      model[i] = 0;
    }
    if (a.size() != n) good = false;

    // Random writes.
    const int writes = std::rand() % 40;
    for (int w = 0; w < writes && n > 0; ++w) {
      const unsigned int idx = static_cast<unsigned int>(std::rand()) % n;
      const int value = (std::rand() % 2001) - 1000;
      a[idx] = value;
      model[idx] = value;
    }
    for (unsigned int i = 0; i < n; ++i)
      if (a[i] != model[i]) good = false;

    // Copy construction is independent of the source.
    Array<int> copy(a);
    if (copy.size() != n) good = false;
    for (unsigned int i = 0; i < n; ++i)
      if (copy[i] != model[i]) good = false;
    if (n > 0) {
      copy[0] = model[0] + 12345;
      if (a[0] != model[0]) good = false;  // the source must be untouched
    }

    // Assignment onto an array of a different, random size.
    const unsigned int otherN =
        static_cast<unsigned int>(std::rand()) % (kMax + 1);
    Array<int> target(otherN);
    target = a;
    if (target.size() != n) good = false;
    for (unsigned int i = 0; i < n; ++i)
      if (target[i] != model[i]) good = false;
    if (n > 0) {
      target[n - 1] = model[n - 1] + 999;
      if (a[n - 1] != model[n - 1]) good = false;
    }

    // Self-assignment through an alias, at a random size.
    Array<int>& alias = a;
    a = alias;
    if (a.size() != n) good = false;
    for (unsigned int i = 0; i < n; ++i)
      if (a[i] != model[i]) good = false;

    // Every out-of-range index throws, on both overloads.
    const unsigned int bad[] = {n, n + 1, kMax + 100,
                                static_cast<unsigned int>(-1)};
    for (int k = 0; k < 4; ++k) {
      if (!throwsOutOfBounds(a, bad[k])) good = false;
      if (!throwsOutOfBoundsMutable(a, bad[k])) good = false;
      ++sawThrow;
    }

    if (!good && allGood) {
      std::ostringstream why;
      why << "round " << round << " n=" << n << " otherN=" << otherN;
      firstFailure = why.str();
      allGood = false;
    }
  }

  Check::ok(allGood, "every round matched the model" +
                     (firstFailure.empty() ? "" : " - first bad: " +
                                                      firstFailure));
  Check::ok(sawEmpty > 0, "the run actually produced empty arrays");
  Check::ok(sawFull > 0, "the run actually produced full-size arrays");
  Check::ok(sawThrow == iterations * 4, "every round exercised the throw paths");
}

// Fuzz the exception paths: detonate at every possible position and require
// the strong guarantee each time.
static void test_fuzz_exception_safety() {
  Check::section("fuzz: detonate at every position, strong guarantee each time");

  Bomb::disarm();
  Bomb::alive = 0;
  bool allGood = true;
  std::string firstFailure;

  for (int n = 1; n <= 8; ++n) {
    for (int detonateAt = 1; detonateAt <= n; ++detonateAt) {
      Array<Bomb> source(static_cast<unsigned int>(n));
      for (int i = 0; i < n; ++i) source[i].v = i + 1;

      Array<Bomb> target(3);
      for (int i = 0; i < 3; ++i) target[i].v = 100 + i;
      const int baseline = Bomb::alive;

      bool good = true;

      // copy constructor
      Bomb::countdown = detonateAt;
      bool threw = false;
      try {
        Array<Bomb> victim(source);
        (void)victim;
      } catch (const std::string&) {
        threw = true;
      }
      Bomb::disarm();
      if (!threw) good = false;
      if (Bomb::alive != baseline) good = false;  // leaked the partial buffer

      // operator=
      Bomb::countdown = detonateAt;
      threw = false;
      try {
        target = source;
      } catch (const std::string&) {
        threw = true;
      }
      Bomb::disarm();
      if (!threw) good = false;
      if (Bomb::alive != baseline) good = false;
      // strong guarantee: the target is byte-for-byte what it was
      if (target.size() != 3) good = false;
      for (int i = 0; i < 3; ++i)
        if (target[i].v != 100 + i) good = false;

      if (!good && allGood) {
        std::ostringstream why;
        why << "n=" << n << " detonateAt=" << detonateAt
            << " alive=" << Bomb::alive << " baseline=" << baseline;
        firstFailure = why.str();
        allGood = false;
      }
    }
  }

  Check::ok(allGood, "no leak and no change, at every detonation point" +
                     (firstFailure.empty() ? "" : " - first bad: " +
                                                      firstFailure));
  Check::ok(Bomb::alive == 0, "the balance sheet is back to zero");
}

static void test_more_edges() {
  Check::section("edges: pointer/bool elements, deep nesting, first-element throws");

  // new T[n]() on a pointer type zeroes them, so every slot is NULL rather
  // than a random address waiting to be dereferenced.
  Array<int*> pointers(4);
  bool allNull = true;
  for (unsigned int i = 0; i < pointers.size(); ++i)
    if (pointers[i] != NULL) allNull = false;
  Check::ok(allNull, "Array<int*>(4) holds four NULLs, not garbage addresses");
  int target = 7;
  pointers[2] = &target;
  Array<int*> pointerCopy(pointers);
  Check::ok(pointerCopy[2] == &target, "pointer elements copy by value");
  Check::ok(*pointerCopy[2] == 7, "and still point at the same object");

  Array<bool> flags(3);
  Check::ok(!flags[0] && !flags[1] && !flags[2], "Array<bool>(3) is all false");
  flags[1] = true;
  Array<bool> flagCopy(flags);
  flagCopy[1] = false;
  Check::ok(flags[1] == true, "bool elements deep-copy");

  Array<char> letters(3);
  Check::ok(letters[0] == 0, "Array<char>(3) is zeroed");

  // Three levels deep. Each layer only ever asks the next for new T[n]() and
  // assignment, so nesting keeps working without any special case.
  Array<int> leaf(2);
  leaf[0] = 42;
  Array<Array<int> > middle(2);
  middle[0] = leaf;
  Array<Array<Array<int> > > root(2);
  root[0] = middle;
  Check::ok(root[0][0][0] == 42, "three levels of subscripting reach the leaf");
  Array<Array<Array<int> > > rootCopy(root);
  rootCopy[0][0][0] = 99;
  Check::ok(root[0][0][0] == 42, "the copy is deep through all three layers");

  // Inner arrays of different sizes in one outer array.
  Array<Array<int> > ragged(3);
  ragged[0] = Array<int>(1);
  ragged[1] = Array<int>(5);
  Check::ok(ragged[0].size() == 1 && ragged[1].size() == 5 &&
                ragged[2].size() == 0,
            "inner arrays keep their own separate sizes");

  // Copy-constructing from a const reference has to work.
  Array<int> source(2);
  source[0] = 1;
  const Array<int>& frozen = source;
  Array<int> fromConst(frozen);
  Check::ok(fromConst.size() == 2 && fromConst[0] == 1,
            "copy construction from a const Array&");
  Array<int> assignedFromConst;
  assignedFromConst = frozen;
  Check::ok(assignedFromConst[0] == 1, "assignment from a const Array&");

  Array<int> single(1);
  single[0] = 5;
  Check::ok(single.size() == 1 && single[0] == 5, "n = 1");
  Check::ok(throwsOutOfBounds(single, 1), "and index 1 throws");

  // A bigger allocation, in case anything is quietly capped.
  const unsigned int kBig = 100000;
  Array<int> big(kBig);
  big[kBig - 1] = 123;
  Check::ok(big.size() == kBig && big[kBig - 1] == 123, "100000 elements");
  Array<int> bigCopy(big);
  Check::ok(bigCopy[kBig - 1] == 123, "and it deep-copies");

  // Detonating on the VERY FIRST element, where nothing has been built yet.
  Mine::disarm();
  Mine::alive = 0;
  {
    Mine::fuse = 1;
    bool threw = false;
    try {
      Array<Mine> never(5);
      (void)never;
    } catch (const std::string&) {
      threw = true;
    }
    Mine::disarm();
    Check::ok(threw, "a throw from the first element propagates");
    Check::ok(Mine::alive == 0, "and leaves nothing constructed behind");
  }

  Bomb::disarm();
  Bomb::alive = 0;
  {
    Array<Bomb> src(4);
    const int baseline = Bomb::alive;
    Bomb::countdown = 1;
    bool threw = false;
    try {
      Array<Bomb> victim(src);
      (void)victim;
    } catch (const std::string&) {
      threw = true;
    }
    Bomb::disarm();
    Check::ok(threw, "a throw on the first copy propagates");
    Check::ok(Bomb::alive == baseline, "and the fresh buffer was still freed");
  }
  Check::ok(Bomb::alive == 0, "balance back to zero");
}

// ---------------------------------------------------------------------------
// The eval sheet asks, by name, for proof that the template works with arrays
// of both simple and complex types. Everything above this line runs on int,
// Tracked, Bomb and Mine. These run on bigint - which owns a std::string, so a
// shared buffer shows up as changed data and then as a double free - and on
// vect2, which has a full operator set and no allocation at all.

template <typename T, typename F>
static void iterArray(Array<T>& a, F f) {
  if (a.size() == 0) return;
  ::iter(&a[0], a.size(), f);
}

template <typename T, typename F>
static void iterArray(const Array<T>& a, F f) {
  if (a.size() == 0) return;
  ::iter(&a[0], a.size(), f);
}

struct FibInto {
  bigint* prev;
  bigint* cur;
  FibInto(bigint* p, bigint* c) : prev(p), cur(c) {}
  void operator()(bigint& slot) const {
    slot = *prev;
    const bigint next = *prev + *cur;
    *prev = *cur;
    *cur = next;
  }
};

struct SumInto {
  bigint* total;
  explicit SumInto(bigint* t) : total(t) {}
  void operator()(const bigint& n) const { *total += n; }
};

struct CountVisits {
  int* n;
  explicit CountVisits(int* p) : n(p) {}
  template <typename T>
  void operator()(const T&) const {
    ++(*n);
  }
};

static void test_complex_element_types() {
  Check::section("Array<bigint> and Array<vect2>: complex element types");

  Array<bigint> numbers(4);
  Check::ok(numbers.size() == 4, "a sized Array of a class type");
  Check::ok(numbers[0] == bigint() && numbers[3] == bigint(),
            "new T[n]() default-constructed every bigint");

  Array<vect2> points(3);
  Check::ok(points[0] == vect2(0, 0) && points[2] == vect2(0, 0),
            "and every vect2");

  numbers[0] = bigint("170141183460469231731687303715884105727");
  Check::ok(numbers[0].digits() == 39, "a 39-digit value written through []");
  const Array<bigint>& sealed = numbers;
  Check::ok(sealed[0].digits() == 39, "and read back through the const []");

  points[1] = vect2(3, 4);
  Check::ok(points[1] == vect2(3, 4) && points[0] == vect2(0, 0),
            "writing one element leaves its neighbours alone");

  // The check a memcpy'd buffer fails: each bigint owns a string, so sharing
  // one would show up here first and as a double free second.
  Array<bigint> copied(numbers);
  copied[0] = bigint(1);
  Check::ok(numbers[0].digits() == 39 && copied[0] == bigint(1),
            "the copy constructor copies the bigints, not the pointer");

  Array<bigint> assigned;
  assigned = numbers;
  assigned[0] = bigint(2);
  Check::ok(numbers[0].digits() == 39 && assigned[0] == bigint(2),
            "and so does operator=");
  Check::ok(assigned.size() == 4, "which also took the size");

  bool threw = false;
  try {
    (void)numbers[4];
  } catch (const std::exception&) {
    threw = true;
  }
  Check::ok(threw, "out of range on Array<bigint> throws std::exception");

  threw = false;
  try {
    points[3] = vect2();
  } catch (const std::exception&) {
    threw = true;
  }
  Check::ok(threw, "and an out-of-range write on Array<vect2>");

  Array<bigint> none;
  Check::ok(none.size() == 0, "an empty Array of a complex type");
  threw = false;
  try {
    (void)none[0];
  } catch (const std::exception&) {
    threw = true;
  }
  Check::ok(threw, "which throws on index 0 like any other empty one");
}

static void test_iter_bridge() {
  Check::section("iter walking an Array's buffer through &a[0]");

  // The buffer is one contiguous new T[n] block, which is the only reason a
  // pointer-and-length algorithm can be pointed at it at all.
  Array<vect2> shape(3);
  Check::ok(&shape[1] == &shape[0] + 1 && &shape[2] == &shape[0] + 2,
            "the elements are contiguous");

  Array<bigint> fib(40);
  bigint prev(0), cur(1);
  iterArray(fib, FibInto(&prev, &cur));
  Check::ok(fib[0] == bigint(0) && fib[1] == bigint(1) && fib[10] == bigint(55),
            "iter filled the Array's buffer, in order");
  Check::ok(fib[39] == bigint("63245986"), "including the far end");

  bigint total;
  iterArray(static_cast<const Array<bigint>&>(fib), SumInto(&total));
  Check::ok(total + bigint(1) == cur, "the const overload read them all back");

  int visits = 0;
  iterArray(fib, CountVisits(&visits));
  Check::ok(visits == 40, "exactly one call per element");

  Array<bigint> empty;
  visits = 0;
  iterArray(empty, CountVisits(&visits));
  Check::ok(visits == 0, "an empty Array visits nothing");

  bool threw = false;
  try {
    (void)&empty[0];
  } catch (const std::exception&) {
    threw = true;
  }
  Check::ok(threw, "&a[0] on an empty Array throws, which is why the guard");
}

int main(int argc, char** argv) {
  unsigned int seed = 42;
  if (argc > 1) seed = static_cast<unsigned int>(std::atoi(argv[1]));
  std::srand(seed);
  const int iterations = 500;

  test_construction();
  test_subscript();
  test_exception_type();
  test_deep_copy();
  test_assignment_resizes();
  test_self_assignment();
  test_nested();
  test_element_lifetime();
  test_throwing_copy_assign();
  test_throwing_default_ctor();
  test_more_edges();
  test_complex_element_types();
  test_iter_bridge();
  test_fuzz(iterations);
  test_fuzz_exception_safety();

  std::ostringstream title;
  title << "ex02 Array<T>  (seed " << seed << ", " << iterations
        << " fuzz rounds)";
  return Check::report(title.str().c_str());
}
