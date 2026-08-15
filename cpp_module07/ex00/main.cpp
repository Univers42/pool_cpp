/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 11:26:30 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/13 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Walkthrough of ex00. Each section is either something the subject asks for
// or something that caught me out while writing it.
//
//   make run                 this walkthrough
//   make test                the assertions behind it
//   ./tests/mutants.sh       breaks the header on purpose, tests must fail
//   ./tests/compile_fail.sh  the code that must NOT compile

#include <iostream>
#include <string>

#include "whatever.hpp"


// Printing helpers. Same shape as ScalarConverter in module 06: all static,
// constructors private so nobody can build one.
class Ui {
 public:
  static const char* const kBold;
  static const char* const kDim;
  static const char* const kGreen;
  static const char* const kCyan;
  static const char* const kYellow;
  static const char* const kRed;
  static const char* const kOff;

  static void title(const char* text) {
    const std::string t(text);
    std::cout << "\n"
              << kBold << kCyan << t << kOff << "\n"
              << kCyan << std::string(t.size(), '=') << kOff << std::endl;
  }

  static void section(const char* text) {
    const std::string t(text);
    std::cout << "\n  " << kBold << ++g_section << ". " << t << kOff << "\n"
              << kDim << "  " << std::string(t.size() + 3, '-') << kOff
              << std::endl;
  }

  static void note(const char* text) {
    std::cout << "     " << kYellow << "note" << kOff << "  " << kDim << text
              << kOff << std::endl;
  }

  static void code(const char* text) {
    std::cout << "     " << kDim << "| " << kOff << kCyan << text << kOff
              << std::endl;
  }

  template <typename T>
  static void show(const char* expr, const T& value) {
    std::cout << "     " << pad(expr) << kGreen << value << kOff << std::endl;
  }

  static void verdict(bool ok, const char* text) {
    std::cout << "     " << (ok ? kGreen : kRed) << (ok ? "OK  " : "BAD ")
              << kOff << text << std::endl;
  }

 private:
  // Declared, never defined, so an accidental Ui u; fails at link time.
  Ui();
  Ui(const Ui& other);
  Ui& operator=(const Ui& other);
  ~Ui();

  static std::string pad(const char* expr) {
    std::string e(expr);
    if (e.size() < 32) e += std::string(32 - e.size(), ' ');
    return e;
  }

  static int g_section;
};

const char* const Ui::kBold = "\033[1m";
const char* const Ui::kDim = "\033[2m";
const char* const Ui::kGreen = "\033[92m";
const char* const Ui::kCyan = "\033[96m";
const char* const Ui::kYellow = "\033[93m";
const char* const Ui::kRed = "\033[91m";
const char* const Ui::kOff = "\033[0m";
int Ui::g_section = 0;


// Only the rank is compared, so two different cards can come out equal. That
// is what makes the "on a tie you get the second one" rule visible on screen
// instead of something you have to take on faith.
struct Card {
  int rank;
  char suit;

  Card() : rank(0), suit('?') {}
  Card(int r, char s) : rank(r), suit(s) {}
  Card(const Card& o) : rank(o.rank), suit(o.suit) {}
  Card& operator=(const Card& o) {
    if (this != &o) {
      rank = o.rank;
      suit = o.suit;
    }
    return *this;
  }
  ~Card() {}

  bool operator<(const Card& o) const { return rank < o.rank; }
  bool operator>(const Card& o) const { return rank > o.rank; }
};

std::ostream& operator<<(std::ostream& os, const Card& c) {
  return os << c.rank << c.suit;
}


// Counts its own copies so the cost of swap/min/max is a number, not a claim.
struct Tracked {
  static int copyCtor;
  static int copyAssign;
  int v;

  // Only the copy operations count, so the numbers stay honest.
  Tracked() : v(0) {}
  explicit Tracked(int value) : v(value) {}
  Tracked(const Tracked& o) : v(o.v) { ++copyCtor; }
  Tracked& operator=(const Tracked& o) {
    if (this != &o) v = o.v;
    ++copyAssign;
    return *this;
  }
  ~Tracked() {}

  bool operator<(const Tracked& o) const { return v < o.v; }
  bool operator>(const Tracked& o) const { return v > o.v; }

  static void reset() {
    copyCtor = 0;
    copyAssign = 0;
  }
};
int Tracked::copyCtor = 0;
int Tracked::copyAssign = 0;

// Same thing by value, so section 5 can compare the two copy counts.
template <typename T>
static T minByValue(T a, T b) {
  return (a < b) ? a : b;
}

// Called with std::string and never with int. The body uses .size(), which no
// int has, and it still compiles: the body is only checked per type you ask
// for. That is section 8.
template <typename T>
static void describe(const char* label, const T& t) {
  Ui::show(label, t.size());
}


static void subjectScenario() {
  Ui::section("the subject's code, run exactly as printed");

  // Copied straight out of the PDF so the output can be compared line for
  // line. Everything after this section is mine.
  int a = 2;
  int b = 3;

  ::swap(a, b);
  std::cout << "a = " << a << ", b = " << b << std::endl;
  std::cout << "min( a, b ) = " << ::min(a, b) << std::endl;
  std::cout << "max( a, b ) = " << ::max(a, b) << std::endl;
  std::string c = "chaine1";
  std::string d = "chaine2";
  ::swap(c, d);
  std::cout << "c = " << c << ", d = " << d << std::endl;
  std::cout << "min( c, d ) = " << ::min(c, d) << std::endl;
  std::cout << "max( c, d ) = " << ::max(c, d) << std::endl;

  Ui::note("one swap, one min, one max, written once and used on int and");
  Ui::note("string. The subject prints the parens without spaces in its");
  Ui::note("expected output but with spaces in its code; this is the code.");
}

static void oneTemplateManyTypes() {
  Ui::section("one template, five types");

  double e = 1.5, f = -2.5;
  ::swap(e, f);
  Ui::show("double  e, was 1.5", e);
  Ui::show("double  f, was -2.5", f);

  char g = 'z', h = 'a';
  ::swap(g, h);
  Ui::show("char    g, was 'z'", g);
  Ui::show("char    h, was 'a'", h);

  Card jack(11, 'S'), five(5, 'H');
  ::swap(jack, five);
  Ui::show("Card    jack, was 11S", jack);
  Ui::show("Card    five, was 5H", five);
  Ui::show("Card    ::min of the two", ::min(jack, five));

  Ui::note("nothing here is shared at run time: the compiler stamps out a");
  Ui::note("separate swap<int>, swap<double>, swap<char>, swap<std::string>");
  Ui::note("and swap<Card>. Templates are a compile-time code generator, not");
  Ui::note("a runtime abstraction - there is no virtual call, no indirection.");
  Ui::note("Card only had to supply operator< and operator>. No base class, no");
  Ui::note("interface to inherit: 'has the operations' IS the requirement.");
}

static void whyGlobalScope() {
  Ui::section("why every call here says ::");

  // No error, no warning, just a different function. std::string is enough to
  // show it; the version using a namespace of my own lives in tests/test.cpp,
  // which is where it belongs.
  std::string longA(40, 'a'), longB(40, 'b');
  const char* bufferOfA = longA.data();
  swap(longA, longB);
  const bool stolenBuffer = (longB.data() == bufferOfA);
  Ui::show("swap(str, str) moved the buffer", stolenBuffer ? "yes" : "no");
  Ui::show("longA now starts with", longA[0]);

  std::string c1(40, 'c'), c2(40, 'd');
  const char* bufferOfC1 = c1.data();
  ::swap(c1, c2);
  Ui::show("::swap(str, str) moved it", (c2.data() == bufferOfC1) ? "yes" : "no");
  Ui::show("c1 now starts with", c1[0]);

  Ui::note("both calls swap the strings, but only the second one is mine.");
  Ui::note("<string> carries a swap written for basic_string, and that one is");
  Ui::note("more specialised than my generic swap(T&, T&), so it wins. It");
  Ui::note("trades the buffers in O(1) rather than making my three copies,");
  Ui::note("which is why the address moves in the first case and not in the");
  Ui::note("second. Nothing warns you that your template was never called.");

  Ui::code("swap(widget1, widget2);   // can also be a hard error");
  Ui::note("if the other candidate is exactly as generic as mine, neither is");
  Ui::note("preferred and the call is ambiguous. So leaving off the :: gets");
  Ui::note("you the wrong function or no function at all.");
  Ui::note("tests/compile_fail.sh builds that case; tests/test.cpp checks");
  Ui::note("which function actually ran.");
}

static void referenceIdentity() {
  Ui::section("min/max return a reference - on a tie you get the SECOND");

  int p = 5, q = 5;
  Ui::show("p == q, both are", p);
  Ui::show("&p", static_cast<const void*>(&p));
  Ui::show("&q", static_cast<const void*>(&q));
  Ui::show("&::min(p, q)", static_cast<const void*>(&::min(p, q)));
  Ui::verdict(&::min(p, q) == &q, "::min returned q, the second argument");
  Ui::verdict(&::max(p, q) == &q, "::max returned q, the second argument");

  Ui::note("comparing VALUES here proves nothing: min is 5 and max is 5 either");
  Ui::note("way. The rule is only observable because the return type is a");
  Ui::note("reference - so the test asserts identity, not equality.");

  Card aceSpades(1, 'S'), aceHearts(1, 'H');
  Ui::show("::min(1S, 1H)", ::min(aceSpades, aceHearts));
  Ui::show("::max(1S, 1H)", ::max(aceSpades, aceHearts));
  Ui::note("Card compares rank only, so these two ARE equal - and the tie rule");
  Ui::note("becomes readable: both answers are the second argument, 1H.");
  Ui::note("It falls out of the ternary for free: (a < b) ? a : b yields b");
  Ui::note("when neither is smaller. No special case was ever written.");
}

static void referenceCosts() {
  Ui::section("const T& in, const T& out: zero copies");

  Tracked big1(1), big2(2);

  Tracked::reset();
  const Tracked& cheap = ::min(big1, big2);
  Ui::show("::min copy-constructions", Tracked::copyCtor);
  Ui::show("::min copy-assignments", Tracked::copyAssign);
  Ui::show("value it selected", cheap.v);

  Tracked::reset();
  const Tracked byValue = minByValue(big1, big2);
  Ui::show("by-value copy-constructions", Tracked::copyCtor);
  Ui::show("value it selected", byValue.v);

  Ui::note("same answer, and the by-value version paid for it: two parameters");
  Ui::note("copied in, one copy back out. On std::string or a big struct that");
  Ui::note("is the whole cost of the call. Taking and returning const T& is");
  Ui::note("why ::min(hugeString, otherHugeString) allocates nothing.");
}

static void swapCost() {
  Ui::section("what swap really costs in C++98");

  Tracked u(10), v(20);
  Tracked::reset();
  ::swap(u, v);
  Ui::show("copy-constructions", Tracked::copyCtor);
  Ui::show("copy-assignments", Tracked::copyAssign);
  Ui::show("u, v after swap", u.v);
  Ui::show("v", v.v);

  Ui::code("T temp = a;   // 1 copy construction");
  Ui::code("a = b;        // 1 copy assignment");
  Ui::code("b = temp;     // 1 copy assignment");
  Ui::note("three full copies of T, and there is no way around it in C++98:");
  Ui::note("move semantics (T&&) arrived in C++11. std::swap costs exactly the");
  Ui::note("same here. This is an accepted trade-off, not an oversight.");
}

static void deductionNeedsAgreement() {
  Ui::section("deduction needs the two arguments to agree");

  Ui::show("::min(3, 7)", ::min(3, 7));
  Ui::show("::min<double>(1, 2.5)", ::min<double>(1, 2.5));

  Ui::code("::min(1, 2.5);   // does not compile");
  Ui::note("T is deduced independently from each parameter: argument 1 says");
  Ui::note("T = int, argument 2.5 says T = double. Deduction does not pick a");
  Ui::note("winner and it does not convert - conflicting deductions are a hard");
  Ui::note("error. Naming T explicitly, ::min<double>(...), stops deduction");
  Ui::note("and lets the ordinary int -> double conversion happen instead.");
}

static void instantiationIsLazy() {
  Ui::section("a template is only compiled when it is instantiated");

  const std::string word = "chaine1";
  describe("describe(std::string).size()", word);

  Ui::code("template <typename T>");
  Ui::code("void describe(const char* label, const T& t) { t.size(); }");
  Ui::note("int has no .size(), yet this file compiles. The body of a template");
  Ui::note("is only type-checked against a type once you instantiate it, and");
  Ui::note("describe<int> is never asked for. Call describe(\"x\", 42) and the");
  Ui::note("error appears at that line - which is also why a template body");
  Ui::note("hidden in a .cpp links only for the types its author happened to");
  Ui::note("instantiate there. Templates live in headers for this reason.");
}

static void danglingTrap() {
  Ui::section("the trap: a reference to something that already died");

  int keep = 4, other = 9;
  const int& safe = ::min(keep, other);
  Ui::show("safe: both args are variables", safe);

  Ui::code("const int& bad = ::min(1, 2);   // undefined behaviour");
  Ui::note("1 and 2 are literals, so the compiler materialises temporaries to");
  Ui::note("bind the const int& parameters. Those temporaries die at the end");
  Ui::note("of that statement - the reference outlives what it points at.");
  Ui::note("Binding a temporary to a reference extends its lifetime, but that");
  Ui::note("rule does NOT reach through a function return.");
  Ui::code("int bad = ::min(1, 2);          // fine: copies before the death");
  Ui::note("Returning const T& is what buys the identity guarantee in section");
  Ui::note("4 and the zero copies in section 5. This is the bill for it.");
  Ui::show("int copy = ::min(1, 2)", ::min(1, 2));
}

int main(void) {
  Ui::title("ex00 - whatever.hpp : function templates");
  std::cout << "  " << Ui::kDim
            << "swap / min / max, written once, generated per type" << Ui::kOff
            << std::endl;

  subjectScenario();
  oneTemplateManyTypes();
  whyGlobalScope();
  referenceIdentity();
  referenceCosts();
  swapCost();
  deductionNeedsAgreement();
  instantiationIsLazy();
  danglingTrap();

  std::cout << "\n"
            << Ui::kBold << "  takeaway" << Ui::kOff << "  "
            << "assert identity where the contract is a reference; a template\n"
            << "            is a code generator, so its errors and its costs "
               "both land\n"
            << "            at the call site.\n"
            << std::endl;
  return (0);
}
