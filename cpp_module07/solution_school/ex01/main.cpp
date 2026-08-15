/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 11:37:09 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/13 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Walkthrough of ex01. iter is one loop; nearly everything worth knowing is
// in what the two template parameters deduce.
//
//   make run                 this walkthrough
//   make test                the assertions behind it
//   ./tests/mutants.sh       breaks the header on purpose, tests must fail
//   ./tests/compile_fail.sh  the code that must NOT compile

#include <cstddef>
#include <iostream>
#include <string>

#include "bigint.hpp"
#include "iter.hpp"
#include "vect2.hpp"


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

  // Every check in this file goes through here, and a failure is remembered
  // rather than only printed - main() returns non-zero on it, so `make run`
  // stops being a thing you have to read carefully to trust.
  static void verdict(bool ok, const char* text) {
    if (!ok) ++g_failures;
    std::cout << "     " << (ok ? kGreen : kRed) << (ok ? "OK  " : "BAD ")
              << kOff << text << std::endl;
  }

  static int failures() { return g_failures; }

  // Leaves the line open for a callback to print onto.
  static void inlineLabel(const char* expr) {
    std::cout << "     " << pad(expr) << kGreen;
  }

  static void endInline() { std::cout << kOff << std::endl; }

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


void increment(int& n) { n++; }

void printInt(const int& n) { std::cout << n << " "; }

// The subject wants an instantiated function template to work as a callback.
// print<std::string> is that; the bare name print would not be.
template <typename T>
void print(const T& elem) {
  std::cout << elem << " ";
}

// Returns something. F does not care, the value just gets dropped.
int discardMe(const int& n) { return n * 2; }

// Any object with operator() works, since F is the whole type and not a
// function-pointer signature.
struct Tally {
  int calls;
  int sum;

  Tally() : calls(0), sum(0) {}
  Tally(const Tally& o) : calls(o.calls), sum(o.sum) {}
  Tally& operator=(const Tally& o) {
    if (this != &o) {
      calls = o.calls;
      sum = o.sum;
    }
    return *this;
  }
  ~Tally() {}

  void operator()(const int& n) {
    ++calls;
    sum += n;
  }
};

// Same idea but writing through borrowed pointers. This is the way round the
// by-value copy in section 4.
struct TallyInto {
  int* calls;
  int* sum;

  TallyInto() : calls(NULL), sum(NULL) {}
  TallyInto(int* c, int* s) : calls(c), sum(s) {}
  // The copy gets the same pointers, so it still writes to my counters.
  TallyInto(const TallyInto& o) : calls(o.calls), sum(o.sum) {}
  TallyInto& operator=(const TallyInto& o) {
    if (this != &o) {
      calls = o.calls;
      sum = o.sum;
    }
    return *this;
  }
  ~TallyInto() {}

  void operator()(const int& n) const {
    ++(*calls);
    *sum += n;
  }
};

// operator() is itself a template, so one object handles any element type.
struct PrintAny {
  PrintAny() {}
  PrintAny(const PrintAny&) {}
  PrintAny& operator=(const PrintAny&) { return *this; }
  ~PrintAny() {}

  template <typename T>
  void operator()(const T& elem) const {
    std::cout << elem << " ";
  }
};

struct Student {
  std::string name;
  int grade;

  Student() : name(""), grade(0) {}
  Student(const Student& o) : name(o.name), grade(o.grade) {}
  Student& operator=(const Student& o) {
    if (this != &o) {
      name = o.name;
      grade = o.grade;
    }
    return *this;
  }
  ~Student() {}
};

void promote(Student& s) { s.grade++; }
void printStudent(const Student& s) {
  std::cout << s.name << "(" << s.grade << ") ";
}

// Not in the subject. Binds a reference to the array rather than a pointer,
// so N is deduced and a decayed pointer will not bind at all. That turns the
// length bug into a compile error.
template <typename T, size_t N>
static size_t lengthOf(T (&)[N]) {
  return N;
}


// ---------------------------------------------------------------------------
// Callbacks for the complex element types in vect2.hpp and bigint.hpp.
// Nothing below touches iter, which is still the same five lines it was in
// section 1.

// Two prototypes that are indistinguishable at the call site. Only one of
// them changes the array, and nothing warns you about the other.
void bumpByCopy(vect2 v) { ++v; }
void bumpByRef(vect2& v) { ++v; }

// Configuration in, nothing out. Copied by value like every functor, and
// completely indifferent to it - which is the other half of section 4.
struct Translate {
  vect2 by;

  Translate() : by() {}
  explicit Translate(const vect2& v) : by(v) {}
  Translate(const Translate& o) : by(o.by) {}
  Translate& operator=(const Translate& o) {
    if (this != &o) by = o.by;
    return *this;
  }
  ~Translate() {}

  void operator()(vect2& p) const { p += by; }
};

struct ScaleBy {
  int factor;

  ScaleBy() : factor(1) {}
  explicit ScaleBy(int n) : factor(n) {}
  ScaleBy(const ScaleBy& o) : factor(o.factor) {}
  ScaleBy& operator=(const ScaleBy& o) {
    if (this != &o) factor = o.factor;
    return *this;
  }
  ~ScaleBy() {}

  void operator()(vect2& p) const { p *= factor; }
};

// Results, so they leave through pointers. It reads the element through a
// const vect2& and writes the corners through a plain vect2*, which puts
// both of vect2's operator[] overloads in one function.
struct BoundsInto {
  vect2* lo;
  vect2* hi;
  int* seen;

  BoundsInto() : lo(NULL), hi(NULL), seen(NULL) {}
  BoundsInto(vect2* l, vect2* h, int* s) : lo(l), hi(h), seen(s) {}
  BoundsInto(const BoundsInto& o) : lo(o.lo), hi(o.hi), seen(o.seen) {}
  BoundsInto& operator=(const BoundsInto& o) {
    if (this != &o) {
      lo = o.lo;
      hi = o.hi;
      seen = o.seen;
    }
    return *this;
  }
  ~BoundsInto() {}

  void operator()(const vect2& p) const {
    if (*seen == 0) {
      *lo = p;
      *hi = p;
      *seen = 1;
      return;
    }
    for (int axis = 0; axis < 2; ++axis) {
      if (p[axis] < (*lo)[axis]) (*lo)[axis] = p[axis];
      if (p[axis] > (*hi)[axis]) (*hi)[axis] = p[axis];
    }
  }
};

// A callback whose parameter is a reference to an ARRAY. iter hands it one
// row of a 2D array, and it runs iter again on that row - the inner length
// is part of the type, so this level does not have to be told it.
template <typename T, size_t N>
void printRow(const T (&row)[N]) {
  std::cout << "[ ";
  ::iter(row, N, print<T>);
  std::cout << "] ";
}

template <size_t N>
void bumpRow(vect2 (&row)[N]) {
  ::iter(row, N, bumpByRef);
}

// Horner, left to right: every digit multiplies what came before by ten.
// Feed it an int[] and a bigint comes out - the array and the result do not
// have to be the same kind of thing.
struct HornerInto {
  bigint* acc;

  HornerInto() : acc(NULL) {}
  explicit HornerInto(bigint* a) : acc(a) {}
  HornerInto(const HornerInto& o) : acc(o.acc) {}
  HornerInto& operator=(const HornerInto& o) {
    if (this != &o) acc = o.acc;
    return *this;
  }
  ~HornerInto() {}

  void operator()(const int& digit) const {
    *acc <<= bigint(1);
    *acc += bigint(digit);
  }
};

// Writes the sequence INTO the array it is walking, carrying the two terms it
// needs behind pointers so the by-value copy still reaches them.
struct FibInto {
  bigint* prev;
  bigint* cur;

  FibInto() : prev(NULL), cur(NULL) {}
  FibInto(bigint* p, bigint* c) : prev(p), cur(c) {}
  FibInto(const FibInto& o) : prev(o.prev), cur(o.cur) {}
  FibInto& operator=(const FibInto& o) {
    if (this != &o) {
      prev = o.prev;
      cur = o.cur;
    }
    return *this;
  }
  ~FibInto() {}

  void operator()(bigint& slot) const {
    slot = *prev;
    const bigint next = *prev + *cur;
    *prev = *cur;
    *cur = next;
  }
};

struct SumInto {
  bigint* total;

  SumInto() : total(NULL) {}
  explicit SumInto(bigint* t) : total(t) {}
  SumInto(const SumInto& o) : total(o.total) {}
  SumInto& operator=(const SumInto& o) {
    if (this != &o) total = o.total;
    return *this;
  }
  ~SumInto() {}

  void operator()(const bigint& n) const { *total += n; }
};

// The same accumulator holding its total by value instead. On int that
// mistake is invisible; here the copy is a heap allocation and bigint counts
// it, so section 11 can price the trap rather than assert it.
struct SumByValue {
  bigint total;

  SumByValue() : total() {}
  SumByValue(const SumByValue& o) : total(o.total) {}
  SumByValue& operator=(const SumByValue& o) {
    if (this != &o) total = o.total;
    return *this;
  }
  ~SumByValue() {}

  void operator()(const bigint& n) { total += n; }
};


static void subjectScenario() {
  Ui::section("the subject's own scenario");

  int ints[] = {1, 2, 3, 4, 5};
  const size_t intLen = sizeof(ints) / sizeof(ints[0]);

  Ui::inlineLabel("before increment");
  ::iter(ints, intLen, printInt);
  Ui::endInline();

  ::iter(ints, intLen, increment);

  Ui::inlineLabel("after ::iter(.., increment)");
  ::iter(ints, intLen, printInt);
  Ui::endInline();

  const std::string strs[] = {"Hello", "42", "C++98"};
  Ui::inlineLabel("const strings, print<string>");
  ::iter(strs, sizeof(strs) / sizeof(strs[0]), print<std::string>);
  Ui::endInline();

  Ui::note("three required cases: mutate through T&, read through const T&,");
  Ui::note("and accept an instantiated function template as the callback.");
}

static void twoParametersDoTheWork() {
  Ui::section("why iter has TWO template parameters, not one");

  Ui::code("template <typename T, typename F>");
  Ui::code("void iter(T* array, const size_t length, F f);");

  int nums[] = {3, 1, 2};

  // A function name decays: F = void (*)(const int&).
  Ui::inlineLabel("F = function pointer");
  ::iter(nums, 3, printInt);
  Ui::endInline();

  // Same type, just named.
  void (*fp)(const int&) = printInt;
  Ui::inlineLabel("F = a pointer VARIABLE");
  ::iter(nums, 3, fp);
  Ui::endInline();

  // An object, not a function pointer at all.
  Ui::inlineLabel("F = a functor object");
  ::iter(nums, 3, PrintAny());
  Ui::endInline();

  // The return value goes nowhere.
  ::iter(nums, 3, discardMe);
  Ui::show("F may return a value", "iter ignores it");

  Ui::note("if the parameter were spelled void (*f)(T&), only the first two");
  Ui::note("would work and the const-ref callback would already be out.");
  Ui::note("Deducing F as the callable's WHOLE type is what makes one");
  Ui::note("template accept f(int&), f(const int&), print<T>, a functor, and");
  Ui::note("a functor whose operator() is itself a template.");
  Ui::note("The price: when the callable does not fit, C++98 gives you an");
  Ui::note("error novel from inside the template instead of at the call.");
}

static void constRidesOnT() {
  Ui::section("T carries the const-ness of the array");

  int mutableInts[] = {10, 20, 30};
  const int frozenInts[] = {10, 20, 30};

  ::iter(mutableInts, 3, increment);
  Ui::inlineLabel("non-const array + mutator");
  ::iter(mutableInts, 3, printInt);
  Ui::endInline();

  Ui::inlineLabel("const array + reader");
  ::iter(frozenInts, 3, printInt);
  Ui::endInline();

  Ui::code("::iter(frozenInts, 3, increment);   // does not compile");
  Ui::note("passing a const int[] deduces T = const int, so array[i] is a");
  Ui::note("const lvalue and simply will not bind to increment's int&:");
  Ui::note("\"binding reference of type int& to const int discards");
  Ui::note("qualifiers\". Const-correctness enforced by deduction alone - no");
  Ui::note("second overload was written, no const_cast, no runtime check.");
  Ui::note("tests/compile_fail.sh compiles that line and proves it fails.");
}

static void functorsAreCopied() {
  Ui::section("the trap: F is taken BY VALUE, so state stays behind");

  int nums[] = {5, 10, 15};

  Tally counter;
  ::iter(nums, 3, counter);
  Ui::show("counter.calls after iter", counter.calls);
  Ui::show("counter.sum after iter", counter.sum);
  Ui::verdict(counter.calls == 0, "the original functor never saw a thing");

  Ui::note("iter's parameter is `F f`, a copy. Every ++calls landed on that");
  Ui::note("copy, which died when iter returned. std::for_each has exactly");
  Ui::note("this shape, which is why it RETURNS the functor - iter does not,");
  Ui::note("so the state has to live somewhere the copy still points at.");

  int calls = 0, sum = 0;
  ::iter(nums, 3, TallyInto(&calls, &sum));
  Ui::show("calls, via a pointer to state", calls);
  Ui::show("sum, via a pointer to state", sum);
  Ui::verdict(calls == 3 && sum == 30, "copying the functor copied the pointer");

  Ui::note("the fix is not to stop copying - it is to make the copy harmless.");
  Ui::note("TallyInto holds int*, so every copy writes to the same counters.");
}

static void exactlyLengthCalls() {
  Ui::section("the callback runs exactly length times");

  const int values[] = {1, 2, 3};
  int calls = 0, sum = 0;
  ::iter(values, 3, TallyInto(&calls, &sum));
  Ui::show("length 3 -> calls", calls);

  calls = 0;
  sum = 0;
  ::iter(values, 0, TallyInto(&calls, &sum));
  Ui::show("length 0 -> calls", calls);

  // iter takes a pointer, so any slice works.
  calls = 0;
  sum = 0;
  ::iter(values + 1, 2, TallyInto(&calls, &sum));
  Ui::show("values + 1, length 2 -> sum", sum);

  Ui::note("my first test suite checked the SUM of visited values, and a");
  Ui::note("mutant that looped length + 1 times survived it - the extra read");
  Ui::note("only changed the sum when the adjacent stack byte happened to be");
  Ui::note("non-zero. Counting invocations is value-independent, so it fails");
  Ui::note("on an off-by-one in either direction, every time.");
}

static void lengthIsNotDeduced() {
  Ui::section("length is passed by hand, and here is why that hurts");

  int nums[] = {1, 2, 3, 4, 5, 6, 7};

  Ui::show("sizeof(nums)/sizeof(nums[0])", sizeof(nums) / sizeof(nums[0]));

  // Two variables on purpose. As one expression this is sizeof(pointer) over
  // sizeof(element), which both compilers warn about, and -Werror would then
  // refuse to build the demo of the bug itself.
  const size_t pointerBytes = sizeof(int*);
  const size_t elementBytes = sizeof(int);
  Ui::show("sizeof(int*) / sizeof(int)", pointerBytes / elementBytes);

  Ui::note("the second line is the classic bug: the moment an array is passed");
  Ui::note("to a function it decays to a pointer and the size is gone, so the");
  Ui::note("sizeof trick silently yields 2 on this machine instead of 7.");
  Ui::note("iter takes T*, so the length MUST be computed at the call site.");
  Ui::note("the mistake is common enough that both g++ and clang ship a");
  Ui::note("dedicated warning for it, -Wsizeof-pointer-div.");

  Ui::show("lengthOf(nums), deduced", lengthOf(nums));
  Ui::code("template <typename T, size_t N> size_t lengthOf(T (&)[N]);");
  Ui::note("binding a reference to the ARRAY instead of a pointer lets N be");
  Ui::note("deduced, and a decayed pointer will not bind at all - the bug");
  Ui::note("becomes a compile error. Not part of the subject, but it is the");
  Ui::note("C++98 answer to the problem iter's signature leaves open.");
}

static void anyElementType() {
  Ui::section("any element type, including your own");

  Student class42[3];
  class42[0].name = "ana";
  class42[0].grade = 90;
  class42[1].name = "ben";
  class42[1].grade = 75;
  class42[2].name = "cyd";
  class42[2].grade = 60;

  Ui::inlineLabel("before promote");
  ::iter(class42, 3, printStudent);
  Ui::endInline();

  ::iter(class42, 3, promote);

  Ui::inlineLabel("after ::iter(.., promote)");
  ::iter(class42, 3, printStudent);
  Ui::endInline();

  // One object, three element types, because operator() is a template.
  double reals[] = {1.5, 2.5};
  std::string words[] = {"generic", "enough"};
  Ui::inlineLabel("PrintAny on int/double/string");
  int ints[] = {7, 8};
  ::iter(ints, 2, PrintAny());
  ::iter(reals, 2, PrintAny());
  ::iter(words, 2, PrintAny());
  Ui::endInline();

  Ui::note("Student needed no base class and no interface. The requirement is");
  Ui::note("just that f(array[i]) is a valid expression - checked once, at");
  Ui::note("instantiation, and costing nothing at run time.");
}

static void complexElements() {
  Ui::section("a real class as the element: vect2");

  vect2 path[] = {vect2(0, 0), vect2(3, 1), vect2(4, 4), vect2(1, 3)};
  const size_t pathLen = lengthOf(path);

  Ui::inlineLabel("the path");
  ::iter(path, pathLen, print<vect2>);
  Ui::endInline();

  ::iter(path, pathLen, Translate(vect2(10, 10)));
  Ui::inlineLabel("after Translate({10, 10})");
  ::iter(path, pathLen, print<vect2>);
  Ui::endInline();

  ::iter(path, pathLen, ScaleBy(2));
  Ui::inlineLabel("after ScaleBy(2)");
  ::iter(path, pathLen, print<vect2>);
  Ui::endInline();

  Ui::note("section 4 called the by-value functor a trap. It is only a trap");
  Ui::note("for accumulators: Translate and ScaleBy carry INPUT, so iter's");
  Ui::note("copy is worth exactly as much as the original. Whether F may be");
  Ui::note("copied is not a property of iter, it is a property of the functor");
  Ui::note("- which is why std::for_each's shape is not a design mistake.");

  ::iter(path, pathLen, bumpByCopy);
  Ui::inlineLabel("after iter(.., bumpByCopy)");
  ::iter(path, pathLen, print<vect2>);
  Ui::endInline();

  ::iter(path, pathLen, bumpByRef);
  Ui::inlineLabel("after iter(.., bumpByRef)");
  ::iter(path, pathLen, print<vect2>);
  Ui::endInline();

  Ui::code("void bumpByCopy(vect2 v)  { ++v; }   // element untouched");
  Ui::code("void bumpByRef (vect2& v) { ++v; }   // element incremented");
  Ui::note("the two calls are character for character identical. iter writes");
  Ui::note("f(array[i]) and lets the callback's own parameter decide whether");
  Ui::note("that is the element or a copy of it. \"Any prototype\" cuts both");
  Ui::note("ways: the useless one is just as valid, and just as quiet.");

  vect2 lo, hi;
  int seen = 0;
  ::iter(path, pathLen, BoundsInto(&lo, &hi, &seen));
  Ui::show("bounding box lo", lo);
  Ui::show("bounding box hi", hi);
  Ui::verdict(lo == vect2(21, 21) && hi == vect2(29, 29),
              "one read-only pass computed the box");

  const vect2 frozen[] = {vect2(5, 6), vect2(7, 8)};
  Ui::inlineLabel("a const vect2 array");
  ::iter(frozen, lengthOf(frozen), print<vect2>);
  Ui::endInline();

  Ui::code("::iter(frozen, 2, bumpByRef);   // does not compile");
  Ui::note("and vect2's two operator[] overloads are chosen the same way:");
  Ui::note("BoundsInto reads p[axis] through a const vect2& and gets the one");
  Ui::note("returning int, then writes (*lo)[axis] through a plain vect2* and");
  Ui::note("gets the one returning int&. Section 3 showed const riding on T");
  Ui::note("for a primitive; on a class it reaches every member it calls.");
}

static void arraysOfArrays() {
  Ui::section("an array of arrays, and iter calling itself");

  vect2 quads[2][4] = {
      {vect2(0, 0), vect2(1, 0), vect2(1, 1), vect2(0, 1)},
      {vect2(5, 5), vect2(6, 5), vect2(6, 6), vect2(5, 6)}};

  Ui::inlineLabel("two quads");
  ::iter(quads, 2, printRow<vect2, 4>);
  Ui::endInline();

  ::iter(quads, 2, bumpRow<4>);
  Ui::inlineLabel("after iter(.., bumpRow<4>)");
  ::iter(quads, 2, printRow<vect2, 4>);
  Ui::endInline();

  Ui::code("template <typename T, size_t N> void printRow(const T (&)[N]);");
  Ui::note("T deduces to vect2[4] here - an array type - so iter's T* is");
  Ui::note("vect2 (*)[4] and array[i] is a whole row. The callback binds a");
  Ui::note("reference to that row and runs iter on it, so the outer call");
  Ui::note("walks rows and the inner one walks corners.");
  Ui::note("printRow<vect2, 4> is the subject's \"instantiated function");
  Ui::note("template as a callback\" requirement showing up a second time,");
  Ui::note("one level down, without iter knowing anything about it.");

  Ui::show("outer length, passed by hand", 2);
  Ui::show("inner length, deduced as N", 4);
  Ui::note("and this is section 6's problem cut exactly in half. The row");
  Ui::note("arrives as a reference to an array, so its length is part of its");
  Ui::note("type and cannot be got wrong. The outer array decayed to a");
  Ui::note("pointer on the way into iter, so that 2 is still mine to get");
  Ui::note("right. Same array, both answers, one call apart.");

  const vect2 sealed[2][2] = {{vect2(1, 1), vect2(2, 2)},
                              {vect2(3, 3), vect2(4, 4)}};
  Ui::inlineLabel("const, and a 2x2");
  ::iter(sealed, 2, printRow<vect2, 2>);
  Ui::endInline();
  Ui::note("T is const vect2[2] on that one, and const-ness reaches the");
  Ui::note("inner iter too: bumpRow would be refused at the outer call.");
}

static void bigintScenario() {
  Ui::section("bigint: the visit order IS the answer");

  // 2^127 - 1, one digit per slot. An int array in, a number no builtin can
  // hold out - the input and the result need not be the same kind of thing.
  const int mersenne[] = {1, 7, 0, 1, 4, 1, 1, 8, 3, 4, 6, 0, 4,
                          6, 9, 2, 3, 1, 7, 3, 1, 6, 8, 7, 3, 0,
                          3, 7, 1, 5, 8, 8, 4, 1, 0, 5, 7, 2, 7};
  const size_t nDigits = lengthOf(mersenne);

  bigint acc;
  ::iter(mersenne, nDigits, HornerInto(&acc));
  Ui::show("Horner over an int[39]", acc);
  Ui::show("digits in the result", acc.digits());

  int backwards[39];
  for (size_t i = 0; i < nDigits; ++i) backwards[i] = mersenne[nDigits - 1 - i];
  bigint other;
  ::iter(backwards, nDigits, HornerInto(&other));
  Ui::verdict(other != acc,
              "the same digits backwards give a different number");

  Ui::note("every other section here would have passed just as well with the");
  Ui::note("loop running downwards. Horner would not: each digit multiplies");
  Ui::note("everything before it by ten, so the order iter visits in is not a");
  Ui::note("detail of the implementation, it is part of the contract. That is");
  Ui::note("why the suite records the SEQUENCE of visited elements and not a");
  Ui::note("count or a sum - a backwards loop matches both of those.");

  // The array is the output this time: 100 slots filled in one pass, with the
  // two terms the recurrence needs carried behind pointers.
  bigint fib[100];
  bigint prev(0), cur(1);
  ::iter(fib, lengthOf(fib), FibInto(&prev, &cur));
  Ui::show("fib[10] after one iter pass", fib[10]);
  Ui::show("fib[99]", fib[99]);

  bigint total;
  ::iter(fib, lengthOf(fib), SumInto(&total));
  Ui::show("sum of fib[0..99]", total);
  Ui::verdict(fib[99] == bigint("218922995834555169026"),
              "fib[99] is the known value: the pass ran forwards, and in full");
  Ui::verdict(total + bigint(1) == cur, "and sum F(0..99) + 1 == F(101)");

  Ui::note("the identity ties the two passes together, and it is worth being");
  Ui::note("exact about what it does not buy. Anything that hits both passes");
  Ui::note("the same way cancels out of it: reverse iter and the sum does not");
  Ui::note("move, because addition commutes and the array still holds the");
  Ui::note("same 100 numbers. Break bigint's carry and both sides break");
  Ui::note("together. I wrote that identity thinking it proved the order and");
  Ui::note("only found out otherwise by mutating iter and watching it keep");
  Ui::note("holding. It is the known value on the line above that pins this");
  Ui::note("down - a check that survives the mutation was never a check.");

  SumByValue collector;
  const size_t before = bigint::copies();
  ::iter(fib, lengthOf(fib), collector);
  const size_t charged = bigint::copies() - before;
  Ui::show("bigint copies charged to iter", charged);
  Ui::show("collector.total afterwards", collector.total);
  Ui::verdict(collector.total == bigint(),
              "the by-value accumulator never saw the array");

  bigint viaPointer;
  const size_t beforePtr = bigint::copies();
  ::iter(fib, lengthOf(fib), SumInto(&viaPointer));
  Ui::show("same run, state behind a ptr", bigint::copies() - beforePtr);
  Ui::verdict(viaPointer == total, "and it gets the right answer as well");

  Ui::note("section 4 made this point with an int counter, where the copy is");
  Ui::note("free and the bug is the only cost. On bigint the copy is a heap");
  Ui::note("allocation, and it is charged once per iter call, not once per");
  Ui::note("element - F is copied into the parameter and then reused for all");
  Ui::note("100 invocations. A functor holding pointers copies nothing at");
  Ui::note("all, which is why every ...Into type in this file is shaped that");
  Ui::note("way rather than being told to hold its own state.");
}

static void edges() {
  Ui::section("edges: empty ranges and a null pointer");

  int calls = 0, sum = 0;
  int* nothing = NULL;
  ::iter(nothing, 0, TallyInto(&calls, &sum));
  Ui::verdict(calls == 0, "NULL with length 0 never dereferences anything");

  Ui::note("the loop condition is checked before the first read, so length 0");
  Ui::note("touches the pointer zero times - NULL is fine as long as the");
  Ui::note("length agrees. iter cannot validate that for you: it has a");
  Ui::note("pointer and a number, and it trusts the number.");

  const int one[] = {99};
  Ui::inlineLabel("single-element array");
  ::iter(one, 1, printInt);
  Ui::endInline();
}

int main(void) {
  Ui::title("ex01 - iter.hpp : a generic algorithm");
  std::cout << "  " << Ui::kDim
            << "one loop, and two template parameters that do all the work"
            << Ui::kOff << std::endl;

  subjectScenario();
  twoParametersDoTheWork();
  constRidesOnT();
  functorsAreCopied();
  exactlyLengthCalls();
  lengthIsNotDeduced();
  anyElementType();
  complexElements();
  arraysOfArrays();
  bigintScenario();
  edges();

  std::cout << "\n"
            << Ui::kBold << "  takeaway" << Ui::kOff << "  "
            << "deduction is the design: T carries const-ness, F carries the\n"
            << "            callable. What iter cannot deduce - the length - "
               "is exactly\n"
            << "            where the bugs live.\n"
            << std::endl;

  if (Ui::failures() != 0) {
    std::cout << Ui::kRed << Ui::kBold << "  " << Ui::failures()
              << " check(s) failed" << Ui::kOff << std::endl;
    return 1;
  }
  return 0;
}
