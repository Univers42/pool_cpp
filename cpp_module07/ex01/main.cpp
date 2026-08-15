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
#include "iter.hpp"


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
};

const char* const Ui::kBold = "\033[1m";
const char* const Ui::kDim = "\033[2m";
const char* const Ui::kGreen = "\033[92m";
const char* const Ui::kCyan = "\033[96m";
const char* const Ui::kYellow = "\033[93m";
const char* const Ui::kRed = "\033[91m";
const char* const Ui::kOff = "\033[0m";
int Ui::g_section = 0;


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
  edges();

  std::cout << "\n"
            << Ui::kBold << "  takeaway" << Ui::kOff << "  "
            << "deduction is the design: T carries const-ness, F carries the\n"
            << "            callable. What iter cannot deduce - the length - "
               "is exactly\n"
            << "            where the bugs live.\n"
            << std::endl;
  return 0;
}
