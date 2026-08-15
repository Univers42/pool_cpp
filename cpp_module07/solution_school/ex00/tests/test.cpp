/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 11:20:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/13 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Tests for swap/min/max, fixed cases plus a fuzz run.
//
//   make test               build and run
//   ./build/bin/test 1234   different fuzz seed
//   ./tests/mutants.sh      breaks the header, these tests must fail
//   ./tests/compile_fail.sh the code that must NOT compile
//
// Most of these check addresses rather than values. Comparing values cannot
// see the "on a tie you get the second argument" rule at all: with x and y
// both 5, the answer is 5 either way.

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "../whatever.hpp"


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


// Counts copies, so the cost is a number instead of something I assert.
struct Tracked {
  static int copyCtor;
  static int copyAssign;
  int v;

  explicit Tracked(int value) : v(value) {}
  Tracked(const Tracked& o) : v(o.v) { ++copyCtor; }
  Tracked& operator=(const Tracked& o) {
    v = o.v;
    ++copyAssign;
    return *this;
  }
  bool operator<(const Tracked& o) const { return v < o.v; }
  bool operator>(const Tracked& o) const { return v > o.v; }

  static void reset() {
    copyCtor = 0;
    copyAssign = 0;
  }
};
int Tracked::copyCtor = 0;
int Tracked::copyAssign = 0;

// This has to stay a namespace. ADL makes lib::swap a candidate for an
// unqualified swap(w1, w2) only because Widget lives in lib, and it never
// looks at static members of a class. As a class the test below would pass
// while proving nothing.
namespace lib {

int swapCalls = 0;

struct Widget {
  int v;
  explicit Widget(int value) : v(value) {}
};

// Not a template: an exact match beats ::swap<Widget> outright.
void swap(Widget& a, Widget& b) {
  ++swapCalls;
  const int t = a.v;
  a.v = b.v;
  b.v = t;
}

}  // namespace lib

// Rank only, so two different cards compare equal and the tie rule shows up
// in the value as well as the address.
struct Card {
  int rank;
  char suit;
  Card(int r, char s) : rank(r), suit(s) {}
  bool operator<(const Card& o) const { return rank < o.rank; }
  bool operator>(const Card& o) const { return rank > o.rank; }
  bool operator==(const Card& o) const {
    return rank == o.rank && suit == o.suit;
  }
};


static void test_subject_contract() {
  Check::section("swap/min/max on the subject's own values");

  int a = 2, b = 3;
  ::swap(a, b);
  Check::ok(a == 3 && b == 2, "swap(int) exchanged 2 and 3");
  Check::ok(::min(a, b) == 2, "min(3, 2) is 2");
  Check::ok(::max(a, b) == 3, "max(3, 2) is 3");

  std::string c = "chaine1", d = "chaine2";
  ::swap(c, d);
  Check::ok(c == "chaine2" && d == "chaine1", "swap(std::string) exchanged");
  Check::ok(::min(c, d) == "chaine1", "min(string) is chaine1");
  Check::ok(::max(c, d) == "chaine2", "max(string) is chaine2");

  double e = 1.5, f = -2.5;
  ::swap(e, f);
  Check::ok(e == -2.5 && f == 1.5, "swap(double) exchanged");
  Check::ok(::min(e, f) == -2.5, "min(double)");
  Check::ok(::max(e, f) == 1.5, "max(double)");

  char g = 'z', h = 'a';
  ::swap(g, h);
  Check::ok(g == 'a' && h == 'z', "swap(char) exchanged");
  Check::ok(::min(g, h) == 'a', "min(char)");
  Check::ok(::max(g, h) == 'z', "max(char)");
}


static void test_returns_reference_to_argument() {
  Check::section("min/max return the argument itself, not a copy");

  int a = 1, b = 2;
  Check::ok(&::min(a, b) == &a, "min returns a reference to a, not a copy of it");
  Check::ok(&::max(a, b) == &b, "max returns a reference to b, not a copy of it");

  std::string s = "aaa", u = "bbb";
  Check::ok(&::min(s, u) == &s, "min(string) aliases the argument");
  Check::ok(&::max(s, u) == &u, "max(string) aliases the argument");

  // A copy would compare equal, so only the address can tell them apart.
  const int& picked = ::min(a, b);
  a = 99;
  Check::ok(picked == 99, "the returned reference tracks later writes to a");
}

static void test_tie_returns_second() {
  Check::section("on a tie, min and max both return the SECOND argument");

  int x = 5, y = 5;
  Check::ok(&::min(x, y) == &y, "min(5, 5) is y");
  Check::ok(&::max(x, y) == &y, "max(5, 5) is y");
  Check::ok(&::min(x, y) != &x, "min(5, 5) is not x");
  Check::ok(&::max(x, y) != &x, "max(5, 5) is not x");

  double p = 2.5, q = 2.5;
  Check::ok(&::min(p, q) == &q, "min(double tie) is the second");
  Check::ok(&::max(p, q) == &q, "max(double tie) is the second");

  std::string m = "same", n = "same";
  Check::ok(&::min(m, n) == &n, "min(string tie) is the second");
  Check::ok(&::max(m, n) == &n, "max(string tie) is the second");

  // Card compares on rank alone, so the tie rule is visible in the VALUE:
  // this catches a min/max that returns the first argument even though a
  // plain int test could not.
  Card first(1, 'S'), second(1, 'H');
  Check::ok(::min(first, second) == second, "min of two equal cards is the second");
  Check::ok(::max(first, second) == second, "max of two equal cards is the second");

  // Side effect of the rule: on a tie both hand back the same object.
  Check::ok(&::min(x, y) == &::max(x, y), "on a tie min and max agree on identity");
}

static void test_strictness() {
  Check::section("min/max pick the correct side when there is no tie");

  int lo = -7, hi = 7;
  Check::ok(&::min(lo, hi) == &lo, "min(lo, hi) is lo");
  Check::ok(&::min(hi, lo) == &lo, "min(hi, lo) is still lo");
  Check::ok(&::max(lo, hi) == &hi, "max(lo, hi) is hi");
  Check::ok(&::max(hi, lo) == &hi, "max(hi, lo) is still hi");

  // Together they account for both arguments - true only when they differ.
  const int* a = &::min(lo, hi);
  const int* b = &::max(lo, hi);
  Check::ok(a != b, "distinct values give distinct answers");
  Check::ok((a == &lo && b == &hi), "min and max between them cover both inputs");
}


static void test_qualified_call() {
  Check::section("dropping the :: silently calls a different function");

  lib::Widget w1(1), w2(2);

  lib::swapCalls = 0;
  swap(w1, w2);  // deliberately unqualified - this is the thing under test
  Check::ok(lib::swapCalls == 1, "unqualified swap went to lib::swap via ADL");

  lib::swapCalls = 0;
  ::swap(w1, w2);
  Check::ok(lib::swapCalls == 0, "::swap bypassed ADL and used our template");
  Check::ok(w1.v == 1 && w2.v == 2, "two swaps returned the values to the start");

  // std::string is the dangerous one: no error, just a different function.
  // <string> has a swap made for basic_string, which is more specialised than
  // my generic one, so it wins and swaps buffers in O(1) instead of copying.
  // The strings are long so the small-string optimisation does not hide it.
  std::string longA(40, 'a'), longB(40, 'b');
  const char* bufferOfA = longA.data();
  swap(longA, longB);  // deliberately unqualified
  Check::ok(longB.data() == bufferOfA,
        "unqualified swap(string) stole the buffer - that was std::swap");
  Check::ok(longA[0] == 'b' && longB[0] == 'a', "it did swap, just not with ours");

  // Ours copies, so each string keeps whatever buffer it already owned.
  std::string c1(40, 'c'), c2(40, 'd');
  const char* bufferOfC1 = c1.data();
  ::swap(c1, c2);
  Check::ok(c1.data() == bufferOfC1, "::swap copied into the existing buffer");
  Check::ok(c1[0] == 'd' && c2[0] == 'c', "and still produced the right answer");
}


static void test_no_copies() {
  Check::section("min/max copy nothing; swap costs exactly 1 ctor + 2 assigns");

  Tracked a(1), b(2);

  Tracked::reset();
  const Tracked& small = ::min(a, b);
  Check::ok(Tracked::copyCtor == 0, "min made no copy construction");
  Check::ok(Tracked::copyAssign == 0, "min made no copy assignment");
  Check::ok(&small == &a, "min returned the argument");

  Tracked::reset();
  const Tracked& big = ::max(a, b);
  Check::ok(Tracked::copyCtor == 0, "max made no copy construction");
  Check::ok(&big == &b, "max returned the argument");

  // swap goes through a temporary: T temp = a (ctor); a = b; b = temp.
  Tracked::reset();
  ::swap(a, b);
  Check::ok(Tracked::copyCtor == 1, "swap made exactly one copy construction");
  Check::ok(Tracked::copyAssign == 2, "swap made exactly two copy assignments");
  Check::ok(a.v == 2 && b.v == 1, "swap actually exchanged the values");
}


static void test_edges() {
  Check::section("edge cases: self-swap, aliasing, const arguments");

  // Self-swap must be a no-op, not corruption. The temporary is what saves it.
  int a = 42;
  ::swap(a, a);
  Check::ok(a == 42, "swap(a, a) leaves a alone");

  std::string s = "unchanged";
  ::swap(s, s);
  Check::ok(s == "unchanged", "swap(s, s) on a string leaves it alone");

  // Both parameters bound to one object: min/max must return that object.
  int one = 3;
  Check::ok(&::min(one, one) == &one, "min(a, a) is a");
  Check::ok(&::max(one, one) == &one, "max(a, a) is a");

  // const arguments are accepted, because the parameters are const T&.
  const int ca = 4, cb = 9;
  Check::ok(::min(ca, cb) == 4, "min accepts const lvalues");
  Check::ok(::max(ca, cb) == 9, "max accepts const lvalues");
  Check::ok(&::min(ca, cb) == &ca, "min of const lvalues still aliases");

  // min/max are read-only: neither argument may change.
  int keepA = 11, keepB = 22;
  (void)::min(keepA, keepB);
  (void)::max(keepA, keepB);
  Check::ok(keepA == 11 && keepB == 22, "min/max leave both arguments untouched");

  // Empty and equal-prefix strings, where operator< is doing real work.
  std::string empty = "", nonEmpty = "a";
  Check::ok(&::min(empty, nonEmpty) == &empty, "empty string is the smaller one");
  std::string shortStr = "abc", longStr = "abcd";
  Check::ok(&::min(shortStr, longStr) == &shortStr, "prefix sorts before extension");

  // Explicit template argument: deduction is bypassed and the int converts.
  Check::ok(::min<double>(1, 2.5) == 1.0, "min<double>(1, 2.5) converts and works");
  Check::ok(::max<double>(1, 2.5) == 2.5, "max<double>(1, 2.5) converts and works");
}


static void test_fuzz_ints(int iterations) {
  Check::section("fuzz: random int pairs satisfy every invariant");

  bool sawTie = false, sawLess = false, sawGreater = false;
  bool allGood = true;
  std::string firstFailure;

  for (int i = 0; i < iterations; ++i) {
    // Small range on purpose so ties actually come up.
    int a = (std::rand() % 21) - 10;
    int b = (std::rand() % 21) - 10;
    const int origA = a, origB = b;

    if (a == b) sawTie = true;
    if (a < b) sawLess = true;
    if (a > b) sawGreater = true;

    const int& lo = ::min(a, b);
    const int& hi = ::max(a, b);

    bool good = true;
    // value invariants
    if (lo > hi) good = false;
    if (lo != origA && lo != origB) good = false;
    if (hi != origA && hi != origB) good = false;
    if (lo != (origA < origB ? origA : origB)) good = false;
    if (hi != (origA > origB ? origA : origB)) good = false;
    // identity invariants: the answer is always one of the two arguments
    if (&lo != &a && &lo != &b) good = false;
    if (&hi != &a && &hi != &b) good = false;
    // the tie rule
    if (origA == origB && (&lo != &b || &hi != &b)) good = false;
    // strict cases must pick the correct side
    if (origA < origB && (&lo != &a || &hi != &b)) good = false;
    if (origA > origB && (&lo != &b || &hi != &a)) good = false;
    // read-only
    if (a != origA || b != origB) good = false;

    // swap is its own inverse
    ::swap(a, b);
    if (a != origB || b != origA) good = false;
    ::swap(a, b);
    if (a != origA || b != origB) good = false;

    if (!good && allGood) {
      std::ostringstream why;
      why << "a=" << origA << " b=" << origB;
      firstFailure = why.str();
      allGood = false;
    }
  }

  Check::ok(allGood, "all invariants held" +
                     (firstFailure.empty() ? "" : " - first bad: " +
                                                      firstFailure));
  // Coverage: a fuzz run that never produced a tie proves nothing about ties.
  Check::ok(sawTie, "the run actually produced ties");
  Check::ok(sawLess, "the run actually produced a < b");
  Check::ok(sawGreater, "the run actually produced a > b");
}

static void test_fuzz_strings(int iterations) {
  Check::section("fuzz: random strings, compared against std::string's own <");

  bool allGood = true;
  std::string firstFailure;

  for (int i = 0; i < iterations; ++i) {
    std::string a, b;
    const int lenA = std::rand() % 6;
    const int lenB = std::rand() % 6;
    for (int k = 0; k < lenA; ++k) a += static_cast<char>('a' + std::rand() % 3);
    for (int k = 0; k < lenB; ++k) b += static_cast<char>('a' + std::rand() % 3);

    const std::string origA = a, origB = b;
    const std::string& lo = ::min(a, b);
    const std::string& hi = ::max(a, b);

    bool good = true;
    if (lo != (origA < origB ? origA : origB)) good = false;
    if (hi != (origA > origB ? origA : origB)) good = false;
    if (&lo != &a && &lo != &b) good = false;
    if (origA == origB && (&lo != &b || &hi != &b)) good = false;

    ::swap(a, b);
    if (a != origB || b != origA) good = false;

    if (!good && allGood) {
      firstFailure = "\"" + origA + "\" vs \"" + origB + "\"";
      allGood = false;
    }
  }

  Check::ok(allGood, "string invariants held" +
                     (firstFailure.empty() ? "" : " - first bad: " +
                                                      firstFailure));
}

static void test_more_edges() {
  Check::section("edges: pointers, chaining, bool, extremes");

  // Pointers have all the comparison operators, so T = int* is legal.
  int storage[2] = {0, 0};
  int* lowAddr = &storage[0];
  int* highAddr = &storage[1];
  Check::ok(&::min(lowAddr, highAddr) == &lowAddr, "min works on pointers");
  ::swap(lowAddr, highAddr);
  Check::ok(lowAddr == &storage[1] && highAddr == &storage[0],
            "swap works on pointers");

  // Chaining: each call hands back a reference to one of the originals, so
  // the identity has to survive being fed straight back in.
  int x = 3, y = 1, z = 2;
  Check::ok(&::min(::min(x, y), z) == &y, "chained min still aliases y");
  Check::ok(&::max(::max(x, y), z) == &x, "chained max still aliases x");

  // Three-way tie: every step keeps returning the second argument.
  int t1 = 5, t2 = 5, t3 = 5;
  Check::ok(&::min(::min(t1, t2), t3) == &t3, "a three-way tie ends at the last");

  bool no = false, yes = true;
  ::swap(no, yes);
  Check::ok(no == true && yes == false, "swap works on bool");
  Check::ok(::min(no, yes) == false, "min works on bool");

  // Extremes, to be sure nothing overflows on the comparison.
  int big = 2147483647, small = -2147483647 - 1;
  Check::ok(&::min(big, small) == &small, "min at the int limits");
  Check::ok(&::max(big, small) == &big, "max at the int limits");
  ::swap(big, small);
  Check::ok(big == -2147483647 - 1 && small == 2147483647, "swap at the limits");

  double negZero = -0.0, posZero = 0.0;
  Check::ok(&::min(negZero, posZero) == &posZero,
            "-0.0 and 0.0 compare equal, so the second comes back");

  std::string longOne(1000, 'x'), longTwo(1000, 'y');
  ::swap(longOne, longTwo);
  Check::ok(longOne[0] == 'y' && longTwo[0] == 'x' && longOne.size() == 1000,
            "swap on 1000-character strings");
}

int main(int argc, char** argv) {
  unsigned int seed = 42;
  if (argc > 1) seed = static_cast<unsigned int>(std::atoi(argv[1]));
  std::srand(seed);
  const int iterations = 4000;

  test_subject_contract();
  test_returns_reference_to_argument();
  test_tie_returns_second();
  test_strictness();
  test_qualified_call();
  test_no_copies();
  test_edges();
  test_more_edges();
  test_fuzz_ints(iterations);
  test_fuzz_strings(iterations);

  std::ostringstream title;
  title << "ex00 swap / min / max  (seed " << seed << ", " << iterations
        << " fuzz iterations)";
  return Check::report(title.str().c_str());
}
