/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:04:33 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/13 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Walkthrough of ex02. Section 1 is the subject's own main, run as given.
// The rest is what that main cannot show, mostly exception safety, which is
// the part of this exercise that took me the longest.
//
//   make run                 this walkthrough
//   make test                the assertions behind it
//   ./tests/mutants.sh       breaks Array.hpp on purpose, tests must fail
//   ./tests/compile_fail.sh  the code that must NOT compile
//   valgrind ./build/bin/ex02

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#include "Array.hpp"


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
    if (e.size() < 34) e += std::string(34 - e.size(), ' ');
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


// Throws from copy-assignment, which is what cloneBuffer does element by
// element. So it goes off halfway through a copy, with some elements already
// done. That is exactly when a half-built array leaks its buffer.
struct Bomb {
  static int countdown;
  int v;

  Bomb() : v(0) {}
  Bomb(const Bomb& o) : v(o.v) {}
  Bomb& operator=(const Bomb& o) {
    if (countdown > 0 && --countdown == 0) throw std::string("Bomb went off");
    v = o.v;
    return *this;
  }
  ~Bomb() {}
};
int Bomb::countdown = -1;

// Throws from the default constructor instead, so it goes off inside
// new T[n](), before there is any buffer to copy into.
struct Mine {
  static int fuse;
  int v;

  Mine() : v(0) {
    if (fuse > 0 && --fuse == 0) throw std::string("Mine went off");
  }
  Mine(const Mine& o) : v(o.v) {}
  Mine& operator=(const Mine& o) {
    if (this != &o) v = o.v;
    return *this;
  }
  ~Mine() {}
};
int Mine::fuse = -1;

// Counts live instances, so I can show nothing was left behind.
struct Tracked {
  static int alive;
  int v;
  Tracked() : v(0) { ++alive; }
  Tracked(const Tracked& o) : v(o.v) { ++alive; }
  Tracked& operator=(const Tracked& o) {
    v = o.v;
    return *this;
  }
  ~Tracked() { --alive; }
};
int Tracked::alive = 0;


#define MAX_VAL 750

// The subject's main, unchanged, just wrapped so I can report on it.
static bool subjectScenario() {
  Ui::section("the subject's evaluation main, verbatim");

  Array<int> numbers(MAX_VAL);
  int* mirror = new int[MAX_VAL];
  srand(time(NULL));
  for (int i = 0; i < MAX_VAL; i++) {
    const int value = rand();
    numbers[i] = value;
    mirror[i] = value;
  }
  // SCOPE: copies die here; originals must survive untouched (deep copy)
  {
    Array<int> tmp = numbers;
    Array<int> test(tmp);
  }

  bool intact = true;
  for (int i = 0; i < MAX_VAL; i++) {
    if (mirror[i] != numbers[i]) intact = false;
  }
  Ui::verdict(intact, "750 values survived two copies going out of scope");

  try {
    numbers[-2] = 0;
  } catch (const std::exception& e) {
    Ui::show("numbers[-2] threw", e.what());
  }
  try {
    numbers[MAX_VAL] = 0;
  } catch (const std::exception& e) {
    Ui::show("numbers[750] threw", e.what());
  }

  for (int i = 0; i < MAX_VAL; i++) {
    numbers[i] = rand();
  }
  delete[] mirror;

  Ui::note("that inner scope is the whole test: if the copies shared one");
  Ui::note("buffer, destroying them would free memory `numbers` still owns,");
  Ui::note("and the comparison loop would read freed memory. Deep copy is");
  Ui::note("what makes the two survivors independent.");
  return intact;
}

static void valueInitialisation() {
  Ui::section("new T[n]() - the parentheses are the whole feature");

  Array<int> zeroed(5);
  bool allZero = true;
  for (unsigned int i = 0; i < zeroed.size(); ++i)
    if (zeroed[i] != 0) allZero = false;
  Ui::verdict(allZero, "Array<int>(5) holds five zeros, not five garbage ints");

  Array<std::string> words(3);
  Ui::show("Array<string>(3)[0] is empty", words[0].empty() ? "yes" : "no");
  Ui::show("its size()", words.size());

  Ui::code("new T[n]    // default-initialised: ints hold garbage");
  Ui::code("new T[n]()  // value-initialised:   ints hold 0");
  Ui::note("for a class type the two are identical - the default constructor");
  Ui::note("runs either way. For int/double/pointers, only the second zeroes");
  Ui::note("them. Same rule as int* p = new int(); giving *p == 0.");
  Ui::note("It costs one pass over the buffer; determinism is worth it.");
}

static void deepCopy() {
  Ui::section("the Rule of Three, and what happens without it");

  Array<int> original(3);
  original[0] = 10;
  original[1] = 20;
  original[2] = 30;

  Array<int> viaCtor(original);
  Array<int> viaAssign;
  viaAssign = original;

  viaCtor[0] = -1;
  viaAssign[1] = -2;

  Ui::show("original[0] after copy edited", original[0]);
  Ui::show("original[1] after assign edited", original[1]);
  Ui::show("viaCtor[0]", viaCtor[0]);
  Ui::show("viaAssign[1]", viaAssign[1]);
  Ui::verdict(original[0] == 10 && original[1] == 20,
              "editing either copy left the original alone");

  // Through a reference: writing a = a directly is a compiler warning, and
  // two names for one object is how this actually happens anyway.
  Array<int>& alias = original;
  original = alias;
  Ui::verdict(original.size() == 3 && original[2] == 30,
              "self-assignment did not free the buffer it was about to read");

  Ui::note("the compiler-generated copy constructor would copy the POINTER, so");
  Ui::note("two Arrays would own one buffer: the second destructor runs");
  Ui::note("delete[] on freed memory. Owning raw memory means writing all");
  Ui::note("three - destructor, copy constructor, copy assignment.");

  Ui::code("if (this != &rhs) { ... }   // an optimisation, not a safety net");
  Ui::note("the usual story is that this guard is what stops operator= from");
  Ui::note("freeing the buffer it is about to read. Here it is not: cloneBuffer");
  Ui::note("builds the new buffer BEFORE delete[] _array runs, so a self-");
  Ui::note("assignment reads live memory and would be correct without it.");
  Ui::note("Deleting the guard is an equivalent mutant - tests/mutants.sh says");
  Ui::note("so out loud, and valgrind agrees. What it actually buys is the");
  Ui::note("pointless allocate-and-copy it skips, which is what the test");
  Ui::note("asserts: self-assignment constructs zero new elements.");
  Ui::note("Ordering the operations correctly made the guard redundant. In a");
  Ui::note("free-first implementation it would be load-bearing.");
}

static void boundsChecking() {
  Ui::section("operator[] throws, and the unsigned index is why -2 is caught");

  Array<int> a(3);
  a[0] = 1;

  try {
    a[3] = 0;
  } catch (const std::exception& e) {
    Ui::show("a[3] on size 3", e.what());
  }

  try {
    a[-2] = 0;
  } catch (const std::exception& e) {
    Ui::show("a[-2]", e.what());
  }

  Ui::show("what -2 becomes as unsigned", static_cast<unsigned int>(-2));
  Ui::note("the parameter is unsigned int, so -2 converts to 4294967294 before");
  Ui::note("the function is even entered. One check, index >= _size, catches");
  Ui::note("both ends: there is no negative case to test for separately.");

  // Different function, and easy to never actually run.
  const Array<int>& frozen = a;
  Ui::show("const a[0] (const overload)", frozen[0]);
  bool constThrew = false;
  try {
    (void)frozen[3];
  } catch (const std::exception&) {
    constThrew = true;
  }
  Ui::verdict(constThrew, "the const overload bounds-checks too");

  Ui::note("two overloads, and which one runs depends on the constness of the");
  Ui::note("Array, not of the index. A test that only ever uses a non-const");
  Ui::note("Array compiles the const version and never executes it.");

  // Nested in the template, so every instantiation has its own type. They
  // are all std::exception though.
  try {
    Array<std::string> s(1);
    (void)s[9];
  } catch (const Array<std::string>::OutOfBoundsException& e) {
    Ui::show("caught by its exact nested type", e.what());
  }
  Ui::note("Array<int>::OutOfBoundsException and Array<string>::Out... are");
  Ui::note("DIFFERENT types - the nested class is a member of each");
  Ui::note("instantiation. Catching std::exception& is what makes one handler");
  Ui::note("work for all of them, which is what the subject's main does.");
}

static void exceptionSafety() {
  Ui::section("the saga: what happens when T itself throws");

  Ui::code("static T* cloneBuffer(const T* src, unsigned int n) {");
  Ui::code("  T* fresh = new T[n]();");
  Ui::code("  try { for (...) fresh[i] = src[i]; }");
  Ui::code("  catch (...) { delete[] fresh; throw; }   // no leak");
  Ui::code("  return fresh;                            // only now is it safe");
  Ui::code("}");

  // Goes off mid-copy, inside the copy constructor.
  {
    Array<Bomb> source(4);
    Bomb::countdown = 2;
    bool threw = false;
    try {
      Array<Bomb> victim(source);
      (void)victim;
    } catch (const std::string& e) {
      threw = true;
      Ui::show("copy ctor, T throws mid-copy", e);
    }
    Bomb::countdown = -1;
    Ui::verdict(threw, "the exception propagated out of the copy constructor");
    Ui::note("the old code allocated into _array and copied in place, so a");
    Ui::note("throw here left a buffer owned by an object whose constructor");
    Ui::note("never finished - no destructor ever runs for it. valgrind said");
    Ui::note("16 bytes definitely lost: four Bombs nobody owned.");
  }

  // Goes off in new T[n]() instead.
  {
    Array<Mine> target(3);
    Array<Mine> source(5);
    Mine::fuse = 2;
    bool threw = false;
    try {
      target = source;
    } catch (const std::string& e) {
      threw = true;
      Ui::show("operator=, T throws in new T[n]()", e);
    }
    Mine::fuse = -1;
    Ui::verdict(threw, "the exception propagated out of operator=");
    Ui::show("target.size() afterwards", target.size());
    Ui::verdict(target.size() == 3,
                "the target still has its ORIGINAL 3 elements");
    Ui::note("this is the mirror bug: the old operator= freed _array first and");
    Ui::note("allocated second, so a throw from new T[n]() left _array");
    Ui::note("dangling - and the destructor then freed it a second time.");
    Ui::note("Both bugs died with one rule: build the new thing completely");
    Ui::note("before touching the old one. That is the STRONG guarantee -");
    Ui::note("the operation either succeeds or changes nothing at all.");
  }

  // And nothing should be left behind on the way.
  Tracked::alive = 0;
  {
    Array<Tracked> a(4);
    Array<Tracked> b(a);
    Array<Tracked> c;
    c = b;
    Ui::show("live Tracked objects inside scope", Tracked::alive);
  }
  Ui::show("live Tracked objects after scope", Tracked::alive);
  Ui::verdict(Tracked::alive == 0, "every element was destroyed exactly once");
}

static void distinctTypes() {
  Ui::section("each instantiation is a separate type");

  Array<int> ints(2);
  Array<std::string> strings(2);
  ints[0] = 42;
  strings[0] = "forty-two";

  Ui::show("Array<int>[0]", ints[0]);
  Ui::show("Array<string>[0]", strings[0]);

  Ui::code("Array<int> a = strings;   // does not compile");
  Ui::note("Array<int> and Array<string> share a name and nothing else: there");
  Ui::note("is no conversion between them and no common base class. Two");
  Ui::note("independent classes the compiler happened to write from one recipe.");

  // The space in > > is not style, C++98 needs it.
  Array<Array<int> > grid(2);
  grid[0] = ints;
  Array<Array<int> > gridCopy(grid);
  gridCopy[0][0] = 999;
  Ui::show("grid[0][0] after editing the copy", grid[0][0]);
  Ui::verdict(grid[0][0] == 42, "the deep copy recursed through both layers");
  Ui::code("Array<Array<int> > grid;   // the space in > > is required in C++98");
  Ui::note("without it >> lexes as the right-shift operator. C++11 fixed the");
  Ui::note("parse; in C++98 the space is not style, it is syntax.");
  Ui::note("Nesting works for free because Array<int> is copyable - the outer");
  Ui::note("Array only ever asks its element type for new T[n]() and T::=.");

  Ui::code("Array<int> a = 5;   // does not compile: the ctor is explicit");
  Ui::note("explicit Array(unsigned int) stops a bare 5 from silently becoming");
  Ui::note("a 5-element array in any function taking an Array<int>.");
}

static void emptyArrays() {
  Ui::section("empty arrays: two ways to get one, both must work");

  Array<int> defaulted;
  Array<int> explicitZero(0);

  Ui::show("Array<int>().size()", defaulted.size());
  Ui::show("Array<int>(0).size()", explicitZero.size());

  bool bothThrow = true;
  try {
    (void)defaulted[0];
    bothThrow = false;
  } catch (const std::exception&) {
  }
  try {
    (void)explicitZero[0];
    bothThrow = false;
  } catch (const std::exception&) {
  }
  Ui::verdict(bothThrow, "index 0 throws on both - there is no element 0");

  Array<int> copyOfEmpty(explicitZero);
  Ui::verdict(copyOfEmpty.size() == 0, "copying an empty array stays empty");

  // This one is here because a mutant survived without it.
  Array<int> full(4);
  full[0] = 7;
  full = defaulted;
  Ui::show("size after `full = empty`", full.size());
  Ui::verdict(full.size() == 0, "assigning an empty array truly empties it");

  Ui::note("the default ctor allocates nothing (_array is NULL); Array(0) does");
  Ui::note("call new T[0](), which is legal and returns a real, unique pointer");
  Ui::note("that must still be delete[]d. Both report size 0.");
  Ui::note("`full = empty` is here because a broken operator= that early-");
  Ui::note("returned on an empty right-hand side passed every test I had.");
}

int main(void) {
  Ui::title("ex02 - Array.hpp : a class template that owns memory");
  std::cout << "  " << Ui::kDim
            << "templates meet the Rule of Three, and then meet exceptions"
            << Ui::kOff << std::endl;

  subjectScenario();
  valueInitialisation();
  deepCopy();
  boundsChecking();
  exceptionSafety();
  distinctTypes();
  emptyArrays();

  std::cout << "\n"
            << Ui::kBold << "  takeaway" << Ui::kOff << "  "
            << "correct-looking code held a leak and a double free for weeks.\n"
            << "            Exception safety was invisible until I wrote types "
               "whose\n"
            << "            constructors and copies throw on purpose.\n"
            << std::endl;
  return 0;
}
