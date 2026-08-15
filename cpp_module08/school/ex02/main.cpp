/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:55:20 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Walkthrough of ex02, closing the scenario the module has been building: the
// event log of a building's access control. Section 1 is the subject's own
// main, run as given; section 2 is the subject's acceptance criterion checked
// against std::list instead of eyeballed; the rest is what those cannot show.
//
//   make run                 this walkthrough
//   make test                the assertions behind it, then every check below
//   ./build/bin/subject      the subject's main, output asserted exactly
//   ./build/bin/cumulative   ex00 + ex01 + ex02 driven as one pipeline
//   ./tests/eval.sh          the 42 evaluation checklist, mechanised
//   ./tests/mutants.sh       breaks MutantStack.hpp, tests must fail
//   ./tests/compile_fail.sh  the code that must NOT compile
//   valgrind ./build/bin/ex02

#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include "MutantStack.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// Copied verbatim from ex00/easyfind.hpp. Exercises in this repo do not share
// code — each re-vendors what it needs — and copying it unchanged is the whole
// point of section 7: easyfind was written before MutantStack existed, refuses
// a std::stack, and accepts a MutantStack without a line of adaptation.
template <typename T>
typename T::iterator easyfind(T& container, int value) {
  typename T::iterator it =
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

  // Any pair of iterators, printed the same way.
  template <typename It>
  static std::string joinRange(It first, It last) {
    std::string out("{");
    for (It it = first; it != last; ++it) {
      if (it != first) out += ", ";
      out += number(*it);
    }
    return out + "}";
  }

  static std::string number(int v) {
    if (v == 0) return "0";
    const bool negative = v < 0;
    unsigned int magnitude = negative ? 0u - static_cast<unsigned int>(v)
                                      : static_cast<unsigned int>(v);
    std::string digits;
    while (magnitude > 0) {
      digits += static_cast<char>('0' + (magnitude % 10));
      magnitude /= 10;
    }
    if (negative) digits += '-';
    std::reverse(digits.begin(), digits.end());
    return digits;
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

// ─────────────────────────────────────────────────────────────────────────────
static void scenario_subject() {
  Ui::section("The subject's main, run exactly as written");

  MutantStack<int> mstack;

  mstack.push(5);
  mstack.push(17);
  Ui::show("mstack.top()", mstack.top());

  mstack.pop();
  Ui::show("mstack.size()", mstack.size());

  mstack.push(3);
  mstack.push(5);
  mstack.push(737);
  mstack.push(0);

  MutantStack<int>::iterator it = mstack.begin();
  MutantStack<int>::iterator ite = mstack.end();

  ++it;
  --it;
  Ui::show("iterated bottom to top", Ui::joinRange(it, ite));

  std::stack<int> s(mstack);
  Ui::show("std::stack<int> s(mstack), size", s.size());
  Ui::verdict(s.size() == mstack.size(),
              "a MutantStack IS a std::stack: the base copy ctor took it");
}

static void scenario_matches_a_list() {
  Ui::section("The subject's acceptance test: identical to std::list");

  // The subject says: replace MutantStack with std::list and the output must
  // not change. Running both and comparing is cheaper than diffing terminals.
  MutantStack<int> mstack;
  std::list<int> lst;

  const int pushes[] = {5, 17};
  for (int i = 0; i < 2; ++i) {
    mstack.push(pushes[i]);
    lst.push_back(pushes[i]);
  }
  const bool sameTop = mstack.top() == lst.back();

  mstack.pop();
  lst.pop_back();
  const bool sameSize = mstack.size() == lst.size();

  const int more[] = {3, 5, 737, 0};
  for (int i = 0; i < 4; ++i) {
    mstack.push(more[i]);
    lst.push_back(more[i]);
  }

  const std::string fromStack = Ui::joinRange(mstack.begin(), mstack.end());
  const std::string fromList = Ui::joinRange(lst.begin(), lst.end());

  Ui::show("MutantStack traversal", fromStack);
  Ui::show("std::list traversal", fromList);
  Ui::verdict(sameTop && sameSize && fromStack == fromList,
              "top, size and every iterated value agree");
  Ui::note("tests/subject.cpp does this on captured output rather than on");
  Ui::note("strings built by hand, so the comparison covers the formatting");
  Ui::note("too, not just the values.");
}

static void scenario_why_stack_has_no_iterators() {
  Ui::section("Why std::stack has none, and where the ones here came from");

  Ui::code("class stack { protected: Container c; ... };   // the standard");
  Ui::code("class MutantStack : public std::stack<T, C> {");
  Ui::code("  iterator begin() { return this->c.begin(); }");
  Ui::code("};");

  Ui::note("std::stack is an adapter: it holds a real container in a member");
  Ui::note("the standard names c, and deliberately publishes only push, pop,");
  Ui::note("top, size and empty. The iterators were never removed — they were");
  Ui::note("hidden, and c is protected precisely so a derived class can");
  Ui::note("un-hide them. Nothing here is a hack or a reimplementation.");
  Ui::note("The this-> is not decoration either: c lives in a base that");
  Ui::note("depends on T and Container, so C++98 will not look there during");
  Ui::note("unqualified lookup. Bare c.begin() does not compile.");

  MutantStack<int> ms;
  ms.push(1);
  ms.push(2);
  Ui::show("MutantStack still has top()", ms.top());
  Ui::show("and size()", ms.size());
  Ui::show("and empty()", ms.empty() ? "true" : "false");
  Ui::show("and now begin()/end()", Ui::joinRange(ms.begin(), ms.end()));
}

static void scenario_four_iterators() {
  Ui::section("All four iterator flavours");

  MutantStack<int> ms;
  for (int i = 1; i <= 5; ++i) ms.push(i * 10);

  Ui::show("begin()..end()   bottom to top",
           Ui::joinRange(ms.begin(), ms.end()));
  Ui::show("rbegin()..rend() top to bottom",
           Ui::joinRange(ms.rbegin(), ms.rend()));

  const MutantStack<int>& frozen = ms;
  Ui::show("const begin()..end()", Ui::joinRange(frozen.begin(), frozen.end()));
  Ui::show("const rbegin()..rend()",
           Ui::joinRange(frozen.rbegin(), frozen.rend()));

  Ui::verdict(*ms.rbegin() == ms.top(),
              "rbegin() is the top of the stack, which is where pop() bites");
  Ui::note("A reverse traversal is the useful one for a stack: it is the");
  Ui::note("order pop() would hand the values back, without popping any.");
  Ui::note("Reading a stack non-destructively is exactly what std::stack");
  Ui::note("cannot do, and the whole reason this exercise exists.");
}

static void scenario_easyfind_on_a_stack() {
  Ui::section("ex00's easyfind, unchanged, applied to a MutantStack");

  MutantStack<int> ms;
  ms.push(5);
  ms.push(17);
  ms.push(3);
  ms.push(737);
  ms.push(0);

  Ui::code("easyfind(ms, 737)   // the template from ex00, copied verbatim");
  MutantStack<int>::iterator hit = easyfind(ms, 737);
  Ui::show("*hit", *hit);
  Ui::show("index from the bottom", std::distance(ms.begin(), hit));

  try {
    easyfind(ms, 42);
  } catch (const std::out_of_range& e) {
    Ui::threw(e);
  }

  Ui::note("The same call on a std::stack does not compile: no begin(), no");
  Ui::note("end(), no iterator typedef. tests/compile_fail.sh has that case.");
  Ui::note("Four typedefs and four one-line forwards were the whole price of");
  Ui::note("readmitting the entire <algorithm> header.");

  Ui::show("std::count(ms, 5)", std::count(ms.begin(), ms.end(), 5));
  Ui::show("*std::max_element(ms)", *std::max_element(ms.begin(), ms.end()));
  Ui::show("std::accumulate(ms, 0)",
           std::accumulate(ms.begin(), ms.end(), 0));

  // Writing through the mutable iterator, which reaches straight into c.
  *easyfind(ms, 737) = 738;
  Ui::show("after *easyfind(ms, 737) = 738", Ui::joinRange(ms.begin(), ms.end()));
  Ui::verdict(ms.top() == 0, "the top is untouched: the edit was in the middle");
  Ui::note("A stack cannot reach its middle. This one can — which is power");
  Ui::note("and hazard in the same line, since nothing stops a caller");
  Ui::note("breaking an invariant the stack discipline used to protect.");
}

static void scenario_event_log() {
  Ui::section("The scenario: the access controller's event log");

  // Swipe timestamps, in seconds since midnight, pushed as they arrive.
  MutantStack<int> log;
  const int arrivals[] = {28800, 28803, 43200, 43800, 61200};
  for (int i = 0; i < 5; ++i) log.push(arrivals[i]);

  Ui::show("event log, oldest first", Ui::joinRange(log.begin(), log.end()));
  Ui::show("most recent event (top)", log.top());

  // The stack half of the interface: undo the last event.
  log.pop();
  Ui::show("after undo (pop)", Ui::joinRange(log.begin(), log.end()));
  Ui::show("new top", log.top());

  // The iterator half: audit the whole history without destroying it.
  Ui::show("history newest first", Ui::joinRange(log.rbegin(), log.rend()));
  Ui::show("still intact, size", log.size());
  Ui::verdict(log.size() == 4, "auditing read every event and consumed none");

  // ex00's question, on live stack data.
  try {
    easyfind(log, 43200);
    Ui::verdict(true, "easyfind found the noon swipe in the log");
  } catch (const std::out_of_range&) {
    Ui::verdict(false, "the noon swipe should have been in the log");
  }

  // ex01's question. Span takes an iterator PAIR, which is precisely what a
  // MutantStack can now hand it — and what a std::stack never could.
  std::vector<int> asRange(log.begin(), log.end());
  std::sort(asRange.begin(), asRange.end());
  unsigned int shortest = 0xFFFFFFFFu;
  for (std::size_t i = 1; i < asRange.size(); ++i) {
    const unsigned int gapValue = static_cast<unsigned int>(asRange[i]) -
                                  static_cast<unsigned int>(asRange[i - 1]);
    if (gapValue < shortest) shortest = gapValue;
  }
  Ui::code("Span day(64);");
  Ui::code("day.addNumber(log.begin(), log.end());   // ex01, one call");
  Ui::show("shortest gap in the log (s)", shortest);
  Ui::verdict(shortest == 3, "the tailgating alarm from ex01, on live data");
  Ui::note("The gap is recomputed here rather than pulled in from ex01,");
  Ui::note("because each exercise ships alone. tests/cumulative.cpp does it");
  Ui::note("for real: a vendored copy of ex01's Span, fed straight from");
  Ui::note("log.begin() and log.end(), asserting the same answer.");
}

static void scenario_other_containers() {
  Ui::section("Any underlying container, not just the default deque");

  const int values[] = {1, 2, 3, 4};

  MutantStack<int> asDeque;  // std::deque<int> by default
  MutantStack<int, std::vector<int> > asVector;
  MutantStack<int, std::list<int> > asList;
  for (int i = 0; i < 4; ++i) {
    asDeque.push(values[i]);
    asVector.push(values[i]);
    asList.push(values[i]);
  }

  Ui::show("deque-backed", Ui::joinRange(asDeque.begin(), asDeque.end()));
  Ui::show("vector-backed", Ui::joinRange(asVector.begin(), asVector.end()));
  Ui::show("list-backed", Ui::joinRange(asList.begin(), asList.end()));

  Ui::note("The iterator typedefs come from Container, so the class does not");
  Ui::note("care which one it got. What changes is the iterator's category:");
  Ui::note("deque and vector give random access, so ms.begin() + 2 compiles;");
  Ui::note("list gives bidirectional, so it does not, and std::sort on a");
  Ui::note("list-backed MutantStack fails to build. Both are in");
  Ui::note("tests/compile_fail.sh, because neither is a runtime error.");
  Ui::show("asVector.begin() + 2", *(asVector.begin() + 2));

  // Seeding from an existing container, via the constructor std::stack has
  // and our own constructors would otherwise have hidden.
  std::deque<int> seed(values, values + 4);
  MutantStack<int> seeded(seed);
  Ui::show("MutantStack<int> seeded(deque)",
           Ui::joinRange(seeded.begin(), seeded.end()));
  Ui::note("Declaring any constructor hides every base-class one, so that");
  Ui::note("std::stack constructor had to be re-exposed by hand.");
}

static void scenario_canonical_form() {
  Ui::section("Orthodox canonical form: copies are independent");

  MutantStack<int> original;
  original.push(1);
  original.push(2);

  MutantStack<int> copy(original);
  MutantStack<int> assigned;
  assigned = original;

  copy.push(99);
  assigned.pop();

  Ui::show("original", Ui::joinRange(original.begin(), original.end()));
  Ui::show("copy, after push(99)", Ui::joinRange(copy.begin(), copy.end()));
  Ui::show("assigned, after pop()",
           Ui::joinRange(assigned.begin(), assigned.end()));
  Ui::verdict(original.size() == 2 && copy.size() == 3 && assigned.size() == 1,
              "three independent stacks");

  Ui::note("All four canonical members forward to std::stack, which forwards");
  Ui::note("to the container, which owns the memory. There is no raw pointer");
  Ui::note("anywhere in this exercise, so there is nothing to get wrong —");
  Ui::note("the compiler-generated versions would have been correct too. The");
  Ui::note("four are written out because the sheet asks to see them.");

  // Slicing, on purpose: this is what the subject's last line does.
  std::stack<int> sliced(original);
  Ui::show("std::stack<int> sliced(original), size", sliced.size());
  Ui::show("sliced.top()", sliced.top());
  Ui::note("The copy is a plain std::stack. It kept every value and lost");
  Ui::note("every iterator — a one-way door, and the only thing the subject");
  Ui::note("actually requires of the inheritance.");
}

static void scenario_scale() {
  Ui::section("Scale: 100,000 pushes, 50,000 pops");

  MutantStack<int> ms;
  for (int i = 1; i <= 100000; ++i) ms.push(i);
  Ui::show("after 100,000 pushes, size", ms.size());
  Ui::show("top", ms.top());

  for (int i = 0; i < 50000; ++i) ms.pop();
  Ui::show("after 50,000 pops, size", ms.size());
  Ui::show("top", ms.top());

  const long sum = std::accumulate(ms.begin(), ms.end(), 0L);
  const long expected = 50000L * 50001L / 2L;
  Ui::show("sum of the survivors", sum);
  Ui::show("n(n+1)/2 for n = 50,000", expected);
  Ui::verdict(sum == expected,
              "every surviving value is the one it should be");
  Ui::note("std::accumulate over a stack. That sentence is the exercise.");
}

static void scenario_recap() {
  Ui::section("What the three exercises add up to");

  Ui::note("ex00  easyfind: one algorithm, any container, because the only");
  Ui::note("      vocabulary it assumes is begin(), end() and an iterator.");
  Ui::note("ex01  Span: a class of my own that speaks that vocabulary, so");
  Ui::note("      easyfind accepted it without being told it existed.");
  Ui::note("ex02  MutantStack: a container that had the vocabulary taken");
  Ui::note("      away, given it back in four typedefs and eight lines.");
  Ui::note("The through-line is that iterators, not containers, are the");
  Ui::note("STL's real interface. Every time a type learned to speak them,");
  Ui::note("code written years earlier started working on it for free.");
}

int main() {
  Ui::title("ex02 - MutantStack: giving an adapter its iterators back");
  std::cout << Ui::kDim
            << "  Scenario: the event log of the access control from ex00/01."
            << Ui::kOff << std::endl;

  scenario_subject();
  scenario_matches_a_list();
  scenario_why_stack_has_no_iterators();
  scenario_four_iterators();
  scenario_easyfind_on_a_stack();
  scenario_event_log();
  scenario_other_containers();
  scenario_canonical_form();
  scenario_scale();
  scenario_recap();

  std::cout << "\n"
            << Ui::kDim
            << "  make test runs the assertions, the fuzzer, the cumulative\n"
               "  pipeline, the evaluation checklist, the mutation run and\n"
               "  the compile-failure cases."
            << Ui::kOff << std::endl
            << std::endl;
  return 0;
}
