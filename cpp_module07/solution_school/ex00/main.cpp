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
// or something that caught me out while writing it. The last three point the
// templates at a polymorphic hierarchy, which is where they stop agreeing with
// each other: min and max take an abstract base, swap refuses it, and the
// answer to "is this type usable" turns out to be per function, not per type.
//
// Every verdict line is checked, so a non-zero exit means a claim in here has
// stopped being true.
//
//   make run                 this walkthrough
//   make test                the assertions behind it
//   ./tests/mutants.sh       breaks the header on purpose, tests must fail
//   ./tests/compile_fail.sh  the code that must NOT compile

#include <complex>
#include <iostream>
#include <sstream>
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

  // The object prints its own line, so the label goes out without a newline
  // and whatever makeSound() resolves to finishes it.
  template <typename T>
  static void sound(const char* expr, const T& obj) {
    std::cout << "     " << pad(expr) << kGreen;
    obj.makeSound();
    std::cout << kOff;
  }

  // Every check in this file goes through here, and a failure is remembered
  // rather than only printed - main() returns non-zero on it, so `make run`
  // stops being a thing you have to read carefully to trust.
  static void verdict(bool ok, const char* text) {
    if (!ok) ++g_failures;
    std::cout << "     " << (ok ? kGreen : kRed) << (ok ? "OK  " : "BAD ")
              << kOff << text << std::endl;
  }

  static int failures() { return g_failures; }

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
  static int g_failures;
};

const char* const Ui::kBold = "\033[1m";
const char* const Ui::kDim = "\033[2m";
const char* const Ui::kGreen = "\033[92m";
const char* const Ui::kCyan = "\033[96m";
const char* const Ui::kYellow = "\033[93m";
const char* const Ui::kRed = "\033[91m";
const char* const Ui::kOff = "\033[0m";
int Ui::g_section = 0;
int Ui::g_failures = 0;


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


// A polymorphic hierarchy, because "does it work with complex types" has a
// second half that Card and std::complex cannot reach: a base that must not be
// instantiable, a virtual table, and a member the compiler cannot copy for you.
//
// Brain is that member. A Dog holds a Brain*, so the copy constructor the
// compiler would write duplicates the POINTER: two Dogs sharing one Brain,
// every idea written through one showing up in the other, and the second
// destructor freeing memory the first one already gave back. Writing the copy
// by hand is the only fix, and section 12 watches it happen.
class Brain {
 public:
  static const int kIdeas = 100;

  Brain() { ++s_live; }
  Brain(const Brain& o) {
    for (int i = 0; i < kIdeas; ++i) _ideas[i] = o._ideas[i];
    ++s_live;
  }
  Brain& operator=(const Brain& o) {
    if (this != &o) {
      for (int i = 0; i < kIdeas; ++i) _ideas[i] = o._ideas[i];
    }
    return *this;
  }
  ~Brain() { --s_live; }

  void setIdea(int i, const std::string& idea) { _ideas[i] = idea; }
  const std::string& getIdea(int i) const { return _ideas[i]; }

  // Not for valgrind's benefit - valgrind already knows. This is so the
  // program can fail its own run when a destructor does not fire.
  static int live() { return s_live; }

 private:
  std::string _ideas[kIdeas];
  static int s_live;
};
const int Brain::kIdeas;
int Brain::s_live = 0;


class Animal {
 public:
  virtual ~Animal() { --s_live; }

  const std::string& getType() const { return _type; }
  const std::string& getName() const { return _name; }

  // There is no such thing as a generic animal noise, and saying so with = 0
  // is what makes Animal abstract: no variable, no new, no array, no
  // temporary - not for me, and not for a template either.
  virtual void makeSound() const = 0;

  // Ordering is by SPECIES only, so two animals of the same species come out
  // equal - the same trick as Card above, and it is what keeps "on a tie you
  // get the second one" observable once these go through ::min.
  bool operator<(const Animal& o) const { return _type < o._type; }
  bool operator>(const Animal& o) const { return _type > o._type; }

  static int live() { return s_live; }

 protected:
  Animal(const std::string& type, const std::string& name)
      : _type(type), _name(name) {
    ++s_live;
  }
  Animal(const Animal& o) : _type(o._type), _name(o._name) { ++s_live; }

  // Protected on purpose. Public, it would let *a = *b through two Animal&
  // slice a Dog into its base half - type and name overwritten, Brain left
  // behind, no diagnostic. Derived classes still reach it, which is all
  // Dog::operator= needs.
  Animal& operator=(const Animal& o) {
    if (this != &o) {
      _type = o._type;
      _name = o._name;
    }
    return *this;
  }

  std::string _type;
  std::string _name;

 private:
  static int s_live;
};
int Animal::s_live = 0;

std::ostream& operator<<(std::ostream& os, const Animal& a) {
  return os << a.getName() << " the " << a.getType();
}


class Dog : public Animal {
 public:
  explicit Dog(const std::string& name)
      : Animal("Dog", name), _brain(new Brain()) {}
  Dog(const Dog& o) : Animal(o), _brain(new Brain(*o._brain)) {}

  // Copy and swap, built on the swap template this exercise is about. tmp is
  // a full deep copy made before anything here is touched, so a throw during
  // it leaves *this exactly as it was; the three swaps cannot fail; and tmp
  // walks out with the old Brain and frees it. Self-assignment is the guard.
  Dog& operator=(const Dog& o) {
    if (this != &o) {
      Dog tmp(o);
      ::swap(_type, tmp._type);
      ::swap(_name, tmp._name);
      ::swap(_brain, tmp._brain);
    }
    return *this;
  }
  ~Dog() { delete _brain; }

  void makeSound() const { std::cout << "Woof! Woof!" << std::endl; }

  Brain* brain() const { return _brain; }

 private:
  Brain* _brain;
};


class Cat : public Animal {
 public:
  explicit Cat(const std::string& name)
      : Animal("Cat", name), _brain(new Brain()) {}
  Cat(const Cat& o) : Animal(o), _brain(new Brain(*o._brain)) {}
  Cat& operator=(const Cat& o) {
    if (this != &o) {
      Cat tmp(o);
      ::swap(_type, tmp._type);
      ::swap(_name, tmp._name);
      ::swap(_brain, tmp._brain);
    }
    return *this;
  }
  ~Cat() { delete _brain; }

  // The name has to match Animal's exactly. My first draft of this file wrote
  // makeNoise here, and with a pure virtual base that is not a silent bug: Cat
  // never overrides anything, stays abstract, and new Cat(...) refuses to
  // compile. tests/compile_fail.sh keeps that case.
  void makeSound() const { std::cout << "Meow." << std::endl; }

  Brain* brain() const { return _brain; }

 private:
  Brain* _brain;
};


static void showZoo(const char* label, Animal* const zoo[], int n) {
  std::ostringstream line;
  for (int i = 0; i < n; ++i) {
    if (i) line << ", ";
    line << *zoo[i];
  }
  Ui::show(label, line.str());
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

// The eval sheet asks "do the functions also work with complex types?", and it
// means class types as opposed to int and char - Card and Tracked above are
// the answer it wants. std::complex is that question read literally, and it
// turns out to be the better test, because the answer splits down the middle.
static void complexTypes() {
  Ui::section("std::complex: swap yes, min/max no - and that is correct");

  std::complex<double> z1(3.0, 4.0);
  std::complex<double> z2(1.0, -2.0);
  Ui::show("z1", z1);
  Ui::show("z2", z2);
  Ui::show("z1.real()", z1.real());
  Ui::show("z1.imag()", z1.imag());
  Ui::show("z1 * z2", z1 * z2);
  Ui::show("std::abs(z1), the modulus", std::abs(z1));
  Ui::show("std::conj(z1)", std::conj(z1));

  const std::complex<double> was1 = z1;
  const std::complex<double> was2 = z2;
  ::swap(z1, z2);
  Ui::show("after ::swap, z1", z1);
  Ui::show("after ::swap, z2", z2);
  Ui::verdict(z1 == was2 && z2 == was1, "::swap<std::complex<double> > works");

  Ui::note("swap only ever needs a copy constructor and a copy assignment.");
  Ui::note("std::complex has both, so the compiler stamps out this one too.");

  Ui::code("::min(z1, z2);   // does not compile");
  Ui::note("std::complex supplies == and != and nothing else. There is");
  Ui::note("no ordering of C compatible with its arithmetic, so 'is 3+4i");
  Ui::note("below 1-2i' has no answer and the standard declines to invent");
  Ui::note("one. min and max ask for operator< and operator>, the type has");
  Ui::note("neither, and the error lands at the instantiation - section 8,");
  Ui::note("this time with a real type rather than a toy struct.");
  Ui::note("So the honest answer is per OPERATION, not per type: a template");
  Ui::note("requires exactly the operations its body uses. Card supplies <");
  Ui::note("and > so all three work on it; std::complex supplies neither,");
  Ui::note("so exactly one of the three does. There is no single yes here.");

  Ui::code("swap(z1, z2);    // no :: - ambiguous, a hard error");
  Ui::note("and this is the case section 3 could only describe:");
  Ui::note("std::complex lives in namespace std, so ADL finds std::swap,");
  Ui::note("which is exactly as generic as mine. Neither can be preferred,");
  Ui::note("so the call is rejected rather than silently resolved. Both are");
  Ui::note("in tests/compile_fail.sh.");
}

// Section 9 split the three functions by which OPERATORS a type has. An
// abstract base splits them again, on a different axis: what a template does
// with a T decides whether the type can be one.
static void abstractBase() {
  Ui::section("an abstract base: min and max take it, swap cannot");

  Dog rex("Rex");
  Cat mia("Mia");
  Animal& a = rex;
  Animal& b = mia;

  const Animal& first = ::min(a, b);
  Ui::show("::min(Rex, Mia)", first);
  Ui::show("::max(Rex, Mia)", ::max(a, b));
  Ui::verdict(&first == &mia, "Cat < Dog, and the reference points at mia");

  Ui::sound("and it is still a Cat", first);
  Ui::note("that line is the payoff for returning const T&. min handed back a");
  Ui::note("reference to the caller's own object, so the virtual call still");
  Ui::note("finds Cat::makeSound. A min that returned T by value would have");
  Ui::note("copied an Animal - sliced, if it could be built at all.");

  Ui::code("::min(rex, mia);   // does not compile");
  Ui::note("rex is a Dog& and mia is a Cat&, so deduction gets T = Dog from");
  Ui::note("one argument and T = Cat from the other and stops - section 7,");
  Ui::note("with a hierarchy instead of int and double. Naming the type,");
  Ui::note("::min<Animal>(rex, mia), or going through Animal& as above, is");
  Ui::note("what makes the two arguments agree.");
  Ui::show("::min<Animal>(rex, mia)", ::min<Animal>(rex, mia));

  Ui::code("Animal beast(\"Animal\", \"Nobody\");   // does not compile");
  Ui::code("::swap(a, b);                       // does not compile either");
  Ui::note("min and max never build a T. They take const T&, return const T&,");
  Ui::note("and touch nothing else - so an abstract T is fine by them. swap");
  Ui::note("opens with T temp = a; which needs a real Animal object, and");
  Ui::note("Animal has a pure virtual member, so there is no such thing.");
  Ui::note("That refusal is a feature: had Animal been concrete, that swap");
  Ui::note("would have compiled and sliced - trading the two base halves and");
  Ui::note("leaving each animal with the other's name and its own Brain.");
  Ui::note("What a template requires of T is decided by its body, one");
  Ui::note("operation at a time. Nothing declares it.");
}

static void arrayOfAnimals() {
  Ui::section("an array of animals, sorted with ::min and ::swap");

  Ui::note("an array of Animal is impossible for the same reason a variable");
  Ui::note("is: the compiler would have to build 4 of them. An array of");
  Ui::note("Animal* is not - a pointer is a concrete type whatever it aims");
  Ui::note("at, and that indirection is where the polymorphism lives.");

  const int kSize = 4;
  Animal* zoo[kSize];
  zoo[0] = new Dog("Rex");
  zoo[1] = new Cat("Mia");
  zoo[2] = new Dog("Bo");
  zoo[3] = new Cat("Ash");

  Ui::show("Animal::live()", Animal::live());
  Ui::show("Brain::live()", Brain::live());
  showZoo("the zoo", zoo, kSize);

  // Selection sort written with nothing but the two templates. min hands back
  // a reference to one of its arguments, so "which one won" is an address
  // comparison - the identity rule from section 4, finally doing some work.
  for (int i = 0; i < kSize - 1; ++i) {
    int best = i;
    for (int j = i + 1; j < kSize; ++j) {
      if (&::min(*zoo[j], *zoo[best]) == zoo[j]) best = j;
    }
    ::swap(zoo[i], zoo[best]);
  }
  showZoo("after the sort", zoo, kSize);

  Ui::note("the two Cats kept their order and so did the two Dogs, and that");
  Ui::note("was not arranged: same species compares equal, min returns the");
  Ui::note("SECOND on a tie, so best never moves on one. The tie rule is what");
  Ui::note("makes this sort stable - for free, again.");

  Ui::note("::swap(zoo[i], zoo[best]) instantiates swap<Animal*>. That");
  Ui::note("moves two pointers - one copy construction and two assignments");
  Ui::note("of a machine word - while the Dogs and Cats stay exactly where");
  Ui::note("they were allocated. No object is copied, so nothing is sliced.");

  Ui::code("::min(zoo[0], zoo[1]);   // compiles, and compares ADDRESSES");
  Ui::note("T deduces to Animal*, and pointers have operator<. It builds, it");
  Ui::note("runs, and it answers a question nobody asked: which allocation");
  Ui::note("sits lower in memory. Dereference first - ::min(*zoo[0], *zoo[1])");
  Ui::note("- so the comparison reaches the animals. Nothing can warn you:");
  Ui::note("Animal* has every operation min asks for. min/max want the");
  Ui::note("object, swap wants the pointer, and the * is the difference.");

  for (int i = 0; i < kSize; ++i) {
    Ui::sound("makeSound() through Animal*", *zoo[i]);
  }
  Ui::note("one loop over Animal*, four different noises, and main knows");
  Ui::note("about none of them. That is the other polymorphism: this one is");
  Ui::note("resolved at run time through the vtable, and it costs an indirect");
  Ui::note("call. The templates above are resolved at compile time and cost");
  Ui::note("nothing. Both are in this loop at once.");

  for (int i = 0; i < kSize; ++i) delete zoo[i];
  Ui::show("Animal::live() after delete", Animal::live());
  Ui::show("Brain::live() after delete", Brain::live());
  Ui::verdict(Animal::live() == 0 && Brain::live() == 0,
              "every new got its delete, through the base pointer");
  Ui::note("delete on an Animal* only runs ~Dog because ~Animal is virtual.");
  Ui::note("Drop the virtual and ~Dog never fires: the Brain is never freed,");
  Ui::note("Brain::live() stays at 4, and this OK turns into a BAD without");
  Ui::note("valgrind having to be installed.");
}

static void deepCopyMatters() {
  Ui::section("copying a Dog: the Brain has to come with it");

  {
    Dog original("Rex");
    original.brain()->setIdea(0, "chase the postman");

    Dog copy(original);
    Ui::show("original's Brain", static_cast<const void*>(original.brain()));
    Ui::show("the copy's Brain", static_cast<const void*>(copy.brain()));
    Ui::verdict(original.brain() != copy.brain(),
                "the copy allocated a Brain of its own");
    Ui::show("the copy's idea 0", copy.brain()->getIdea(0));

    copy.brain()->setIdea(0, "sleep on the sofa");
    Ui::show("after writing to the copy", copy.brain()->getIdea(0));
    Ui::show("the original still thinks", original.brain()->getIdea(0));
    Ui::verdict(original.brain()->getIdea(0) == "chase the postman",
                "writing through one Dog did not reach the other");

    Ui::note("the copy constructor the compiler would have written copies the");
    Ui::note("pointer and stops. Both Dogs would name one Brain: that write");
    Ui::note("above would have changed both, and the second destructor would");
    Ui::note("free memory the first one already returned. Two bugs, one");
    Ui::note("missing constructor - the addresses above are the proof.");

    Dog target("Bo");
    target.brain()->setIdea(0, "bury something");
    const Brain* targetsOldBrain = target.brain();
    target = original;
    Ui::verdict(target.brain() != original.brain() &&
                    target.brain() != targetsOldBrain,
                "operator= deep-copies too, and dropped the old Brain");
    Ui::show("target's name is now", target.getName());
    Ui::show("target's idea 0 is now", target.brain()->getIdea(0));

    Dog& alias = target;  // through a reference, so the compiler stays quiet
    const Brain* before = target.brain();
    target = alias;
    Ui::verdict(target.brain() == before &&
                    target.brain()->getIdea(0) == "chase the postman",
                "self-assignment left it alone");

    const std::string catIdea = "knock the glass off the table";
    Cat felix("Felix");
    felix.brain()->setIdea(7, catIdea);
    Cat felixCopy(felix);
    felixCopy.brain()->setIdea(7, "sit in the box");
    Ui::verdict(felix.brain() != felixCopy.brain() &&
                    felix.brain()->getIdea(7) == catIdea,
                "Cat copies deeply as well - the same code, written twice");

    Ui::show("Animals alive in this block", Animal::live());
    Ui::show("Brains alive in this block", Brain::live());

    Ui::code("Dog& operator=(const Dog& o) {");
    Ui::code("  if (this != &o) {");
    Ui::code("    Dog tmp(o);              // deep copy first");
    Ui::code("    ::swap(_name, tmp._name);");
    Ui::code("    ::swap(_brain, tmp._brain);");
    Ui::code("  }                          // tmp leaves with the old Brain");
    Ui::code("  return *this;");
    Ui::code("}");
    Ui::note("copy and swap, and the swap is the one from whatever.hpp. The");
    Ui::note("deep copy happens before anything here is touched, so if new");
    Ui::note("throws, *this is still the object it was. swap<Brain*> then");
    Ui::note("moves a pointer, not 100 strings, and tmp's destructor deletes");
    Ui::note("the brain we used to own. Cleanup is not written anywhere: it");
    Ui::note("is a consequence of who is holding what when tmp goes out of");
    Ui::note("scope. This is exactly the shape of ::swap - and the reason");
    Ui::note("swapping pointers keeps coming up.");
  }

  Ui::show("Animals alive after the block", Animal::live());
  Ui::show("Brains alive after the block", Brain::live());
  Ui::verdict(Animal::live() == 0 && Brain::live() == 0,
              "every Dog, Cat and Brain in this section is gone");
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
  complexTypes();
  abstractBase();
  arrayOfAnimals();
  deepCopyMatters();
  danglingTrap();

  std::cout << "\n"
            << Ui::kBold << "  takeaway" << Ui::kOff << "  "
            << "assert identity where the contract is a reference; a template\n"
            << "            is a code generator, so its errors and its costs "
               "both land\n"
            << "            at the call site.\n"
            << std::endl;

  if (Ui::failures() != 0) {
    std::cout << Ui::kRed << Ui::kBold << "  " << Ui::failures()
              << " check(s) failed" << Ui::kOff << std::endl;
    return (1);
  }
  return (0);
}
