/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:39:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Walkthrough of ex01, continuing ex00's scenario: the badge swipe log of a
// building's access control. Section 1 is the subject's own main, run as
// given; the rest is what that main cannot show.
//
//   make run                 this walkthrough
//   make test                the assertions behind it, then every check below
//   ./build/bin/subject      the subject's main, output asserted exactly
//   ./tests/eval.sh          the 42 evaluation checklist, mechanised
//   ./tests/mutants.sh       breaks Span.cpp on purpose, tests must fail
//   ./tests/compile_fail.sh  the code that must NOT compile
//   valgrind ./build/bin/ex01

#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

#include "Span.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// Copied verbatim from ex00/easyfind.hpp. Exercises in this repo do not share
// code — each one re-vendors what it needs — and copying it unchanged is the
// point of section 8: easyfind was written before Span existed and still
// accepts one, because Span speaks begin()/end()/const_iterator.
template <typename T>
typename T::const_iterator easyfind(const T& container, int value) {
  typename T::const_iterator it =
      std::find(container.begin(), container.end(), value);
  if (it == container.end())
    throw std::out_of_range("easyfind: value not found");
  return (it);
}

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

  static void threw(const std::exception& e) {
    std::cout << "     " << kRed << "throw" << kOff << " " << kDim << e.what()
              << kOff << std::endl;
  }

 private:
  // Declared, never defined, so an accidental Ui u; fails at link time.
  Ui();
  Ui(const Ui& other);
  Ui& operator=(const Ui& other);
  ~Ui();

  static std::string pad(const char* expr) {
    std::string e(expr);
    if (e.size() < 38) e += std::string(38 - e.size(), ' ');
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

// A 32-bit linear congruential generator, so the large runs below are the same
// on every machine. std::rand would do, but its sequence is library-specific
// and an unreproducible number in a walkthrough is just noise.
class Lcg {
 public:
  explicit Lcg(unsigned int seed) : _state(seed) {}
  int next() {
    _state = _state * 1664525u + 1013904223u;
    return static_cast<int>(_state);
  }

 private:
  unsigned int _state;
};

// The naive shortest-span the evaluation sheet warns about: subtract the two
// lowest values. It is here to be shown wrong, not to be used.
static unsigned int naiveShortestSpan(const std::vector<int>& values) {
  std::vector<int> sorted(values);
  std::sort(sorted.begin(), sorted.end());
  return static_cast<unsigned int>(sorted[1]) -
         static_cast<unsigned int>(sorted[0]);
}

// ─────────────────────────────────────────────────────────────────────────────
static void scenario_subject() {
  Ui::section("The subject's example, run exactly as written");

  Span sp = Span(5);
  sp.addNumber(6);
  sp.addNumber(3);
  sp.addNumber(17);
  sp.addNumber(9);
  sp.addNumber(11);

  Ui::show("sp", sp);
  Ui::show("sp.shortestSpan()", sp.shortestSpan());
  Ui::show("sp.longestSpan()", sp.longestSpan());
  Ui::verdict(sp.shortestSpan() == 2 && sp.longestSpan() == 14,
              "the subject's expected 2 and 14");
  Ui::note("tests/subject.cpp runs this same main and asserts the printed");
  Ui::note("text is exactly \"2\\n14\\n\", captured from the stream.");
}

static void scenario_not_the_two_lowest() {
  Ui::section("Why the shortest span is not 'the two lowest, subtracted'");

  int raw[] = {6, 3, 17, 9, 11};
  const std::vector<int> values(raw, raw + 5);

  Span sp(5);
  sp.addNumber(values.begin(), values.end());

  Ui::show("values", sp);
  Ui::code("sorted:  3, 6, 9, 11, 17");
  Ui::show("naive: sorted[1] - sorted[0]", naiveShortestSpan(values));
  Ui::show("Span::shortestSpan()", sp.shortestSpan());
  Ui::verdict(naiveShortestSpan(values) == 3 && sp.shortestSpan() == 2,
              "the naive answer is 3, the right answer is 2 (11 - 9)");
  Ui::note("The smallest gap can sit anywhere in the sorted sequence. What");
  Ui::note("IS guaranteed is that once sorted, the two values forming it are");
  Ui::note("neighbours — so one pass over adjacent pairs finds it, and the");
  Ui::note("O(n^2) all-pairs comparison is never needed.");
  Ui::note("Span.cpp does that pass with std::inner_product, the two-range");
  Ui::note("fold, so there is no hand-written loop in the class at all.");
}

static void scenario_capacity() {
  Ui::section("The invariant: N is a hard ceiling");

  Span sp(3);
  sp.addNumber(1);
  sp.addNumber(2);
  sp.addNumber(3);
  Ui::show("sp", sp);
  Ui::verdict(sp.full(), "full() reports the ceiling was reached");

  Ui::code("sp.addNumber(4);   // one too many");
  try {
    sp.addNumber(4);
    Ui::verdict(false, "the fourth add must not succeed");
  } catch (const Span::SpanFullException& e) {
    Ui::threw(e);
  }
  Ui::show("sp after the failed add", sp);
  Ui::verdict(sp.size() == 3, "the rejected value was not stored");

  // Span() with no N is born full: capacity zero.
  Span empty;
  Ui::show("Span() default", empty);
  try {
    empty.addNumber(1);
  } catch (const Span::SpanFullException& e) {
    Ui::threw(e);
  }
  Ui::note("A default-constructed Span has N = 0, so it is full from birth.");
  Ui::note("That is the honest reading of the canonical form: the default");
  Ui::note("constructor has to produce a valid object, and a zero-capacity");
  Ui::note("Span is valid — it just cannot hold anything.");
}

static void scenario_too_few_elements() {
  Ui::section("Fewer than two values: there is no span to report");

  Span sp(5);
  Ui::code("shortestSpan() on an empty Span");
  try {
    sp.shortestSpan();
  } catch (const Span::NotEnoughElementsException& e) {
    Ui::threw(e);
  }

  sp.addNumber(42);
  Ui::code("longestSpan() with a single value");
  try {
    sp.longestSpan();
  } catch (const Span::NotEnoughElementsException& e) {
    Ui::threw(e);
  }

  sp.addNumber(42);
  Ui::show("two identical values, shortest", sp.shortestSpan());
  Ui::show("two identical values, longest", sp.longestSpan());
  Ui::note("Two equal values are two values: the span is 0, not an error.");
  Ui::note("Returning 0 for the empty case instead would have made a real");
  Ui::note("answer indistinguishable from a refusal to answer.");
}

static void scenario_bulk_insert() {
  Ui::section("Adding numbers without calling addNumber ten thousand times");

  // Any iterator pair. A vector's...
  std::vector<int> fromVector;
  for (int i = 0; i < 4; ++i) fromVector.push_back(i * 3);

  Span a(12);
  a.addNumber(fromVector.begin(), fromVector.end());
  Ui::show("from a std::vector", a);

  // ...a list's, which are not random-access...
  std::list<int> fromList;
  fromList.push_back(100);
  fromList.push_back(200);
  a.addNumber(fromList.begin(), fromList.end());
  Ui::show("+ from a std::list", a);

  // ...two raw pointers, because a pointer IS a random-access iterator...
  int raw[] = {7, 8};
  a.addNumber(raw, raw + 2);
  Ui::show("+ from a C array (pointers)", a);

  // ...and another Span's, now that Span exposes begin()/end().
  Span other(2);
  other.addNumber(-1);
  other.addNumber(-2);
  a.addNumber(other.begin(), other.end());
  Ui::show("+ from another Span", a);

  Ui::note("One member template covers all four, because none of them is a");
  Ui::note("container — they are all just a pair of iterators.");

  // Atomicity: the capacity check runs before the insert, so a range that
  // does not fit changes nothing at all.
  Span tight(4);
  tight.addNumber(1);
  Ui::show("tight before", tight);
  int tooMany[] = {1, 2, 3, 4, 5};
  Ui::code("tight.addNumber(tooMany, tooMany + 5);   // 1 + 5 > 4");
  try {
    tight.addNumber(tooMany, tooMany + 5);
  } catch (const Span::SpanFullException& e) {
    Ui::threw(e);
  }
  Ui::show("tight after", tight);
  Ui::verdict(tight.size() == 1,
              "nothing partially inserted: the check precedes the mutation");
  Ui::note("std::distance is called first and the insert only happens if the");
  Ui::note("whole range fits. Inserting then trimming would have been");
  Ui::note("simpler to write and impossible to recover from.");
}

static void scenario_overflow() {
  Ui::section("The overflow trick: a span no signed int can hold");

  const int kMin = std::numeric_limits<int>::min();
  const int kMax = std::numeric_limits<int>::max();

  Span sp(2);
  sp.addNumber(kMin);
  sp.addNumber(kMax);

  Ui::show("INT_MIN", kMin);
  Ui::show("INT_MAX", kMax);
  Ui::show("longestSpan()", sp.longestSpan());
  Ui::show("UINT_MAX", std::numeric_limits<unsigned int>::max());
  Ui::verdict(sp.longestSpan() == std::numeric_limits<unsigned int>::max(),
              "exactly 4294967295, the true distance");

  Ui::code("maxVal - minVal            // signed: undefined behaviour");
  Ui::code("unsigned(maxVal) - unsigned(minVal)   // defined, and exact");
  Ui::note("Signed overflow is undefined, so the compiler may assume it");
  Ui::note("never happens and optimise on that basis. Unsigned arithmetic is");
  Ui::note("defined to wrap mod 2^32, and the wrapped value is the exact");
  Ui::note("distance for every pair of ints. Two casts replace the 64-bit");
  Ui::note("type C++98 does not have.");
}

static void scenario_easyfind_on_a_span() {
  Ui::section("ex00's easyfind, unchanged, applied to a Span");

  Span sp(6);
  int raw[] = {28800, 28803, 43200, 61200};
  sp.addNumber(raw, raw + 4);
  Ui::show("sp", sp);

  Ui::code("easyfind(sp, 43200)   // the template from ex00, copied verbatim");
  Span::const_iterator it = easyfind(sp, 43200);
  Ui::show("*it", *it);
  Ui::show("index", std::distance(sp.begin(), it));

  try {
    easyfind(sp, 12345);
  } catch (const std::out_of_range& e) {
    Ui::threw(e);
  }

  Ui::note("Nothing in easyfind was touched and nothing in Span was written");
  Ui::note("for it. Span happens to expose begin(), end() and a const_iterator");
  Ui::note("typedef, which is the entire interface easyfind ever required.");
  Ui::note("That is what 'generic' buys: code written before a type existed");
  Ui::note("keeps working on it.");

  // And the same reason makes the whole of <algorithm> available.
  Ui::show("std::count(sp, 28803)",
           std::count(sp.begin(), sp.end(), 28803));
  Ui::show("*std::max_element(sp)", *std::max_element(sp.begin(), sp.end()));
  Ui::verdict(
      std::equal(sp.begin(), sp.end(), raw),
      "std::equal against the source array: insertion order preserved");
}

static void scenario_tailgating() {
  Ui::section("The scenario: catching a tailgater");

  // Badge 4711's swipes today, seconds since midnight. Two of them are three
  // seconds apart, which no human does alone.
  int swipes[] = {28800, 28803, 43200, 43800, 61200};
  Span day(64);
  day.addNumber(swipes, swipes + 5);
  Ui::show("swipe log", day);

  const unsigned int closest = day.shortestSpan();
  const unsigned int spread = day.longestSpan();

  Ui::show("shortestSpan() seconds", closest);
  Ui::show("longestSpan() seconds", spread);
  Ui::show("longest span in hours", spread / 3600);

  const bool alarm = closest < 10;
  Ui::verdict(alarm, "two swipes 3 s apart: a tailgating alarm fires");
  Ui::note("This is the question ex00 could not answer. easyfind asks 'is");
  Ui::note("this exact value present'; shortestSpan asks 'how close do any");
  Ui::note("two of them get', which is not a search at all — it is a sort");
  Ui::note("followed by a fold, and the STL owns both.");

  // Adding a whole extra badge's log is one call.
  int visitor[] = {32000, 32001};
  day.addNumber(visitor, visitor + 2);
  Ui::show("with a visitor's swipes", day);
  Ui::show("shortest now", day.shortestSpan());
  Ui::verdict(day.shortestSpan() == 1, "the visitor is even faster");
}

static void scenario_scale() {
  Ui::section("Scale: the subject asks for 10,000, so here is 100,000");

  Lcg rng(20260815u);

  // The subject's own bar first.
  std::vector<int> tenThousand;
  tenThousand.reserve(10000);
  for (int i = 0; i < 10000; ++i) tenThousand.push_back(rng.next());

  Span big(10000);
  big.addNumber(tenThousand.begin(), tenThousand.end());
  Ui::show("Span(10000) filled, size", big.size());
  Ui::show("shortestSpan()", big.shortestSpan());
  Ui::show("longestSpan()", big.longestSpan());

  // Ten times larger, drawn from a range of about 2^30. The shortest span
  // comes back 0, and that is the data being right rather than Span being
  // wrong: it is the birthday paradox. With n draws from N values the
  // expected number of colliding pairs is n^2 / 2N, which here is about 4.7.
  std::vector<int> hundredThousand;
  hundredThousand.reserve(100000);
  for (int i = 0; i < 100000; ++i) hundredThousand.push_back(rng.next() / 4);

  Span huge(100000);
  huge.addNumber(hundredThousand.begin(), hundredThousand.end());
  Ui::show("Span(100000) random, size", huge.size());
  Ui::show("shortestSpan()", huge.shortestSpan());

  // Don't take Span's word for it — prove the duplicate exists independently.
  std::vector<int> sorted(hundredThousand);
  std::sort(sorted.begin(), sorted.end());
  const bool hasDuplicate =
      std::adjacent_find(sorted.begin(), sorted.end()) != sorted.end();
  Ui::verdict(huge.shortestSpan() == 0 && hasDuplicate,
              "0 because two values really are equal, confirmed separately");
  Ui::note("Worth knowing before trusting a shortest-span alarm on random");
  Ui::note("data: at this scale a collision is not an edge case, it is the");
  Ui::note("expected outcome. Span reports it correctly, which is all it");
  Ui::note("can do; interpreting it is the caller's job.");

  // A collision-free fill, so the answer is knowable in advance: values 37
  // apart, plus one pair placed 1 apart.
  std::vector<int> spaced;
  spaced.reserve(100000);
  for (int i = 0; i < 99998; ++i) spaced.push_back(i * 37);
  spaced.push_back(-100);
  spaced.push_back(-99);

  Span planted(100000);
  planted.addNumber(spaced.begin(), spaced.end());
  Ui::show("Span(100000) spaced by 37", planted.size());
  Ui::show("shortestSpan() (planted 1)", planted.shortestSpan());
  Ui::show("longestSpan()", planted.longestSpan());
  Ui::verdict(planted.shortestSpan() == 1,
              "the planted neighbouring pair is found among 100,000");
  Ui::note("O(n log n) for the sort, one pass for the fold. The naive");
  Ui::note("all-pairs version would have made about 5 billion comparisons");
  Ui::note("here; this returns instantly.");
}

static void scenario_canonical_form() {
  Ui::section("Orthodox canonical form: copies are independent");

  Span original(5);
  original.addNumber(1);
  original.addNumber(10);

  Span copy(original);          // copy constructor
  Span assigned;                // default constructor
  assigned = original;          // copy assignment

  Ui::show("original", original);
  Ui::show("copy(original)", copy);
  Ui::show("assigned = original", assigned);

  copy.addNumber(100);
  Ui::show("after copy.addNumber(100)", copy);
  Ui::show("original is unaffected", original);
  Ui::verdict(original.size() == 2 && copy.size() == 3,
              "std::vector did the deep copy; no raw memory to get wrong");

  Ui::note("Assignment copies N as well as the values: a Span(5) assigned");
  Ui::note("into a Span() becomes a Span(5), not a zero-capacity one that");
  Ui::note("happens to hold two numbers.");
  Ui::show("assigned.maxSize()", assigned.maxSize());

  // Through an alias, because -Wself-assign-overloaded rejects the literal
  // `original = original;` — a warning that is right about real code and in
  // the way here, since self-assignment is exactly what is under test.
  Span& alias = original;
  original = alias;
  Ui::verdict(original.size() == 2, "self-assignment is a no-op, not a wipe");
}

static void scenario_bridge_to_ex02() {
  Ui::section("Handing the scenario to ex02");

  Ui::note("Span answers questions about a set of values it already holds.");
  Ui::note("The access-control system also needs the ORDER events arrived");
  Ui::note("in, and the ability to undo the most recent one — a stack.");
  Ui::note("std::stack has push/pop/top and, deliberately, no iterators, so");
  Ui::note("its contents cannot be fed to Span::addNumber(first, last), to");
  Ui::note("easyfind, or to anything else in <algorithm>.");
  Ui::note("ex02 is the fix: MutantStack keeps every std::stack guarantee");
  Ui::note("and adds the four iterator typedefs, which puts the whole");
  Ui::note("library back within reach.");
  Ui::code("Span day(64);");
  Ui::code("day.addNumber(eventStack.begin(), eventStack.end());  // ex02");
}

int main() {
  Ui::title("ex01 - Span: a bounded set of ints that answers two questions");
  std::cout << Ui::kDim
            << "  Scenario: the swipe log of the badge registry from ex00."
            << Ui::kOff << std::endl;

  scenario_subject();
  scenario_not_the_two_lowest();
  scenario_capacity();
  scenario_too_few_elements();
  scenario_bulk_insert();
  scenario_overflow();
  scenario_easyfind_on_a_span();
  scenario_tailgating();
  scenario_scale();
  scenario_canonical_form();
  scenario_bridge_to_ex02();

  std::cout << "\n"
            << Ui::kDim
            << "  make test runs the assertions, the fuzzer, the evaluation\n"
               "  checklist, the mutation run and the compile-failure cases."
            << Ui::kOff << std::endl
            << std::endl;
  return 0;
}
