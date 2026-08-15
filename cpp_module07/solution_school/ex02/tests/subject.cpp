/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subject.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/13 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// One section per bullet of the subject, in the subject's order, so I can
// point at each requirement during the defence. test.cpp checks the class is
// correct; this checks it does what the paper actually asks for.
//
//   make test              runs alongside test.cpp
//   ./build/bin/subject    run directly
//
// Requirement 4 is the one that needed thinking about. "You MUST use the
// operator new[]" and "preventive allocation is forbidden" are about
// allocation, and no amount of checking values can see either. So Probe has
// its own operator new[] and delete[] that count calls.

#include <cstddef>
#include <exception>
#include <iostream>
#include <string>

#include "../Array.hpp"


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
      size_t len = std::string(s.name).length();
      std::string dots(len < 56 ? 56 - len : 1, '.');
      std::cout << "  " << s.name << " " << dots << " " << s.checks << " "
                << (s.failed ? "FAILED" : "ok") << std::endl;
      checks += s.checks;
      failed += s.failed;
    }
    std::cout << std::endl;
    if (!failed) {
      std::cout << "  " << g_sectionCount << " requirements, " << checks
                << " checks, all satisfied" << std::endl;
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

  static const int kMaxSections = 16;
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


// A class-level operator new[] is picked over the global one for `new Probe[n]`,
// so these counters see exactly the allocations Array makes for its buffer.
// This is what makes requirement 4 checkable rather than merely readable.
struct Probe {
  static int arrayNews;
  static int arrayDeletes;
  static int alive;
  int v;

  Probe() : v(0) { ++alive; }
  Probe(const Probe& o) : v(o.v) { ++alive; }
  ~Probe() { --alive; }
  Probe& operator=(const Probe& o) {
    v = o.v;
    return *this;
  }

  static void* operator new[](size_t bytes) {
    ++arrayNews;
    return ::operator new[](bytes);
  }
  static void operator delete[](void* p) {
    ++arrayDeletes;
    ::operator delete[](p);
  }

  static void reset() {
    arrayNews = 0;
    arrayDeletes = 0;
  }
};
int Probe::arrayNews = 0;
int Probe::arrayDeletes = 0;
int Probe::alive = 0;

// A plain aggregate, to confirm value-initialisation reaches types that have
// no user-written constructor at all.
struct Pod {
  int x;
  double y;
};


static void req_default_ctor() {
  Check::section("1. no-parameter construction creates an EMPTY array");

  Array<int> a;
  Check::ok(a.size() == 0, "size() is 0");

  // "Empty" has to mean unusable-by-index, not just size 0.
  bool threw = false;
  try {
    (void)a[0];
  } catch (const std::exception&) {
    threw = true;
  }
  Check::ok(threw, "index 0 has nothing to reach and throws");

  Array<std::string> s;
  Check::ok(s.size() == 0, "same for a class element type");
}


static void req_sized_ctor() {
  Check::section("2. Array(unsigned int n) gives n default-initialised elements");

  // The subject's own tip: new int() value-initialises to 0. That is exactly
  // the behaviour Array(n) has to reproduce for every element.
  int* tip = new int();
  Check::ok(*tip == 0, "the subject's tip: *(new int()) is 0");
  delete tip;

  Array<int> a(5);
  Check::ok(a.size() == 5, "size() reports the requested count");
  bool allZero = true;
  for (unsigned int i = 0; i < a.size(); ++i)
    if (a[i] != 0) allZero = false;
  Check::ok(allZero, "every int element is 0, not garbage - new T[n]() not new T[n]");

  Array<double> d(4);
  bool doublesZero = true;
  for (unsigned int i = 0; i < d.size(); ++i)
    if (d[i] != 0.0) doublesZero = false;
  Check::ok(doublesZero, "doubles are value-initialised too");

  // A type with no user-written constructor: value-initialisation still
  // zeroes each member.
  Array<Pod> p(3);
  bool podZero = true;
  for (unsigned int i = 0; i < p.size(); ++i)
    if (p[i].x != 0 || p[i].y != 0.0) podZero = false;
  Check::ok(podZero, "an aggregate with no ctor is zeroed member by member");

  // A class type gets its default constructor run.
  Array<std::string> s(3);
  Check::ok(s[0].empty() && s[2].empty(), "class elements are default-constructed");

  Check::ok(Array<int>(1).size() == 1, "n = 1 works");
  Check::ok(Array<int>(0).size() == 0, "n = 0 is accepted and reports 0");
}


static void req_copy_and_assign() {
  Check::section("3. copy ctor and operator=, with neither side affecting the other");

  Array<int> original(3);
  original[0] = 10;
  original[1] = 20;
  original[2] = 30;

  // -- copy construction
  Array<int> copy(original);
  Check::ok(copy.size() == 3 && copy[1] == 20, "the copy starts out equal");

  copy[0] = -1;
  Check::ok(original[0] == 10, "modifying the COPY does not affect the ORIGINAL");
  original[2] = -3;
  Check::ok(copy[2] == 30, "modifying the ORIGINAL does not affect the COPY");

  // -- assignment
  Array<int> assigned;
  assigned = original;
  Check::ok(assigned.size() == 3 && assigned[0] == 10, "assignment copies both");

  assigned[0] = -99;
  Check::ok(original[0] == 10, "modifying the ASSIGNED copy leaves the source alone");
  original[1] = -88;
  Check::ok(assigned[1] == 20, "modifying the source leaves the assigned copy alone");

  // The subject says "in both cases", so a class element type has to hold up
  // too - a shallow copy of std::string would double-free its buffer.
  Array<std::string> s(2);
  s[0] = "forty";
  s[1] = "two";
  Array<std::string> sCopy(s);
  s[1] = "three";
  Check::ok(sCopy[1] == "two", "std::string elements are deep-copied");

  Array<std::string> sAssigned;
  sAssigned = s;
  s[0] = "changed";
  Check::ok(sAssigned[0] == "forty", "same through operator=");

  // Independence has to survive the source being destroyed entirely.
  Array<int>* heap = new Array<int>(2);
  (*heap)[0] = 7;
  Array<int> survivor(*heap);
  delete heap;
  Check::ok(survivor.size() == 2 && survivor[0] == 7,
        "the copy outlives the original it was made from");
}


static void req_allocation_rules() {
  Check::section("4. new[] is used, nothing is pre-allocated, nothing overruns");

  // -- "Preventive allocation is forbidden": the default ctor must not
  //    allocate a buffer at all.
  Probe::reset();
  {
    Array<Probe> empty;
    Check::ok(Probe::arrayNews == 0,
          "the default ctor performed ZERO array allocations");
    Check::ok(Probe::alive == 0, "and constructed zero elements");
    Check::ok(empty.size() == 0, "while still reporting size 0");
  }

  // -- "You MUST use the operator new[]": a sized construction goes through
  //    Probe's own array-new, which only `new Probe[n]` can reach.
  Probe::reset();
  {
    Array<Probe> a(4);
    Check::ok(Probe::arrayNews == 1, "Array(4) allocated via operator new[], once");
    Check::ok(Probe::alive == 4, "and constructed exactly four elements");
  }
  Check::ok(Probe::arrayDeletes == 1, "the destructor released it via delete[]");
  Check::ok(Probe::alive == 0, "and destroyed all four elements");

  // -- every allocation is matched, across the copy paths too
  Probe::reset();
  {
    Array<Probe> a(3);
    Array<Probe> b(a);
    Array<Probe> c;
    c = b;
  }
  Check::ok(Probe::arrayNews == Probe::arrayDeletes,
        "every new[] was matched by exactly one delete[]");
  Check::ok(Probe::alive == 0, "no element outlived its array");

  // -- "must never access non-allocated memory": every index outside the
  //    range refuses to touch the buffer. Run under valgrind for the rest.
  Array<int> guarded(4);
  bool allRefused = true;
  const unsigned int outside[] = {4, 5, 1000, static_cast<unsigned int>(-1),
                                  static_cast<unsigned int>(-2)};
  for (int i = 0; i < 5; ++i) {
    try {
      guarded[outside[i]] = 0;
      allRefused = false;
    } catch (const std::exception&) {
    }
  }
  Check::ok(allRefused, "every out-of-range index refused to touch memory");

  // An empty array owns no buffer, so index 0 must not be dereferenced.
  Array<int> nothing;
  bool emptyRefused = false;
  try {
    (void)nothing[0];
  } catch (const std::exception&) {
    emptyRefused = true;
  }
  Check::ok(emptyRefused, "an empty array never dereferences its null buffer");
}


static void req_subscript() {
  Check::section("5. elements are reachable through the subscript operator");

  Array<int> a(3);
  a[0] = 1;
  a[1] = 2;
  a[2] = 3;
  Check::ok(a[0] == 1 && a[1] == 2 && a[2] == 3, "[] both reads and writes");

  // It returns a reference, so it is a genuine lvalue.
  a[1] += 40;
  Check::ok(a[1] == 42, "[] returns a reference you can modify through");

  // A const Array must still be readable - that is the second overload.
  const Array<int>& frozen = a;
  Check::ok(frozen[1] == 42, "[] works on a const Array (const overload)");

  Array<std::string> s(2);
  s[0] = "hello";
  Check::ok(s[0] == "hello", "[] works for a class element type");
}


static void req_out_of_bounds_throws() {
  Check::section("6. an out-of-bounds index throws an std::exception");

  Array<int> a(3);

  // The subject asks specifically for an std::exception, so that is the
  // handler used here - not the concrete type.
  bool caughtAsStdException = false;
  try {
    a[3] = 0;
  } catch (const std::exception& e) {
    caughtAsStdException = true;
    Check::ok(e.what() != NULL, "what() returns a message");
  }
  Check::ok(caughtAsStdException, "index == size throws, caught as std::exception");

  bool caughtHigh = false;
  try {
    (void)a[999];
  } catch (const std::exception&) {
    caughtHigh = true;
  }
  Check::ok(caughtHigh, "a far out-of-range index throws");

  // The index parameter is unsigned, so a negative literal wraps to a huge
  // value and is caught by the same single check.
  bool caughtNegative = false;
  try {
    a[-2] = 0;
  } catch (const std::exception&) {
    caughtNegative = true;
  }
  Check::ok(caughtNegative, "a[-2] throws (it wraps to a huge unsigned index)");

  // The const overload has to throw as well.
  const Array<int>& frozen = a;
  bool caughtConst = false;
  try {
    (void)frozen[3];
  } catch (const std::exception&) {
    caughtConst = true;
  }
  Check::ok(caughtConst, "the const overload throws too");

  // A throw must leave the array usable.
  Check::ok(a.size() == 3, "the array is unchanged after a failed access");
  a[0] = 5;
  Check::ok(a[0] == 5, "and still writable");
}


// size() must be callable on a const instance and take no parameters. Both are
// compile-time facts: this function would not compile otherwise, so its
// existence is the proof.
static unsigned int sizeOfConst(const Array<int>& a) { return a.size(); }

static void req_size() {
  Check::section("7. size() returns the count, takes nothing, modifies nothing");

  Array<int> a(7);
  Check::ok(a.size() == 7, "size() reports the element count");
  Check::ok(sizeOfConst(a) == 7, "size() is const - callable on a const Array");

  // "must not modify the current instance": calling it repeatedly changes
  // neither the reported size nor the contents.
  a[0] = 11;
  a[6] = 66;
  for (int i = 0; i < 5; ++i) (void)a.size();
  Check::ok(a.size() == 7, "repeated calls do not change the size");
  Check::ok(a[0] == 11 && a[6] == 66, "nor the contents");

  Array<int> empty;
  Check::ok(empty.size() == 0, "an empty array reports 0");

  Array<int> copy(a);
  Check::ok(copy.size() == a.size(), "a copy reports the same size");

  // size() tracks assignment in both directions.
  Array<int> grown;
  grown = a;
  Check::ok(grown.size() == 7, "size() follows an assignment that grows");
  grown = empty;
  Check::ok(grown.size() == 0, "and one that empties");
}

int main(void) {
  req_default_ctor();
  req_sized_ctor();
  req_copy_and_assign();
  req_allocation_rules();
  req_subscript();
  req_out_of_bounds_throws();
  req_size();

  return Check::report("ex02 subject conformance  (each section = one requirement)");
}
