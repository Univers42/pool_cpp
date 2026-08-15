/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cumulative.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// The whole module driven as one pipeline, on the scenario the three exercises
// have been sharing: a building's access-control system.
//
//     MutantStack<int> log        ex02  the event log, in arrival order
//       -> easyfind(log, t)       ex00  was this exact swipe recorded?
//       -> span.addNumber(b, e)   ex01  how close do any two swipes get?
//
// Each exercise ships alone, so ex00's easyfind and ex01's Span are vendored
// below rather than included across directory boundaries. easyfind is copied
// character for character from ex00/easyfind.hpp. Span is ex01's class with
// the bodies moved inline, because a test binary links one translation unit
// and cannot compile ex01/Span.cpp — the interface, the invariants and the
// algorithms are unchanged, and ex01's own suite is what proves them.
//
// What is NOT vendored, and is the actual claim under test, is the joint: the
// argument types. Span::addNumber(first, last) was written months before
// MutantStack existed and names no container anywhere; MutantStack's
// iterators are its underlying deque's, re-exported. That these fit is not a
// coincidence to be admired, it is a property to be asserted.
//
//   ./build/bin/cumulative

#include <algorithm>
#include <exception>
#include <iterator>
#include <limits>
#include <list>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "../MutantStack.hpp"
#include "check.hpp"

// ── ex00, copied verbatim from ex00/easyfind.hpp ─────────────────────────────
template <typename T>
typename T::iterator easyfind(T& container, int value) {
  typename T::iterator it =
      std::find(container.begin(), container.end(), value);
  if (it == container.end())
    throw std::out_of_range("easyfind: value not found");
  return (it);
}

template <typename T>
typename T::const_iterator easyfind(const T& container, int value) {
  typename T::const_iterator it =
      std::find(container.begin(), container.end(), value);
  if (it == container.end())
    throw std::out_of_range("easyfind: value not found");
  return (it);
}

// ── ex01, inlined from ex01/Span.{hpp,cpp} ───────────────────────────────────
struct UnsignedGap {
  unsigned int operator()(int high, int low) const {
    return static_cast<unsigned int>(high) - static_cast<unsigned int>(low);
  }
};

struct Smaller {
  unsigned int operator()(unsigned int a, unsigned int b) const {
    return a < b ? a : b;
  }
};

class Span {
 private:
  unsigned int _maxSize;
  std::vector<int> _vec;

 public:
  typedef std::vector<int>::const_iterator const_iterator;

  class SpanFullException : public std::exception {
   public:
    virtual const char* what() const throw() {
      return "Error: Span is already full, cannot add more numbers.";
    }
  };

  class NotEnoughElementsException : public std::exception {
   public:
    virtual const char* what() const throw() {
      return "Error: Not enough elements to calculate a span (requires at "
             "least 2).";
    }
  };

  Span() : _maxSize(0) {}
  explicit Span(unsigned int N) : _maxSize(N) { _vec.reserve(N); }
  Span(const Span& src) : _maxSize(src._maxSize), _vec(src._vec) {}
  Span& operator=(const Span& rhs) {
    if (this != &rhs) {
      _maxSize = rhs._maxSize;
      _vec = rhs._vec;
    }
    return *this;
  }
  ~Span() {}

  void addNumber(int n) {
    if (_vec.size() >= _maxSize) throw SpanFullException();
    _vec.push_back(n);
  }

  template <typename InputIterator>
  void addNumber(InputIterator first, InputIterator last) {
    if (std::distance(first, last) + _vec.size() > _maxSize) {
      throw SpanFullException();
    }
    _vec.insert(_vec.end(), first, last);
  }

  unsigned int shortestSpan() const {
    if (_vec.size() < 2) throw NotEnoughElementsException();
    std::vector<int> sorted(_vec);
    std::sort(sorted.begin(), sorted.end());
    return std::inner_product(sorted.begin() + 1, sorted.end(), sorted.begin(),
                              std::numeric_limits<unsigned int>::max(),
                              Smaller(), UnsignedGap());
  }

  unsigned int longestSpan() const {
    if (_vec.size() < 2) throw NotEnoughElementsException();
    const int minVal = *std::min_element(_vec.begin(), _vec.end());
    const int maxVal = *std::max_element(_vec.begin(), _vec.end());
    return UnsignedGap()(maxVal, minVal);
  }

  unsigned int size() const { return static_cast<unsigned int>(_vec.size()); }
  unsigned int maxSize() const { return _maxSize; }
  bool empty() const { return _vec.empty(); }
  bool full() const { return _vec.size() >= _maxSize; }
  const_iterator begin() const { return _vec.begin(); }
  const_iterator end() const { return _vec.end(); }
};

// The scenario's data: badge 4711's swipes, seconds since midnight, in the
// order the controller saw them. Two are three seconds apart.
static const int kSwipes[] = {28800, 28803, 43200, 43800, 61200};
static const std::size_t kSwipeCount = sizeof(kSwipes) / sizeof(kSwipes[0]);

static MutantStack<int> buildLog() {
  MutantStack<int> log;
  for (std::size_t i = 0; i < kSwipeCount; ++i) log.push(kSwipes[i]);
  return log;
}

// ─────────────────────────────────────────────────────────────────────────────
static void test_easyfind_over_a_mutantstack() {
  Check::section("ex00 -> ex02: easyfind runs on a MutantStack");

  MutantStack<int> log = buildLog();

  MutantStack<int>::iterator hit = easyfind(log, 43200);
  Check::eq(*hit, 43200, "the noon swipe was recorded");
  Check::eq(std::distance(log.begin(), hit),
            static_cast<std::ptrdiff_t>(2), "at the position it arrived in");

  bool threw = false;
  try {
    easyfind(log, 12345);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "a swipe that never happened throws, as on any container");

  // The const overload picks up too, through a const reference to the stack.
  const MutantStack<int>& frozen = log;
  Check::eq(*easyfind(frozen, 61200), 61200, "and the const overload works");

  // The same call on a plain std::stack does not compile at all — no begin(),
  // no end(), no iterator typedef. tests/compile_fail.sh holds that case,
  // because a test binary cannot assert the absence of a compile error.
  Check::eq(*easyfind(log, 28803), 28803, "the tailgating swipe is in there");
}

static void test_span_fed_from_a_mutantstack() {
  Check::section("ex02 -> ex01: a stack's range fills a Span in one call");

  MutantStack<int> log = buildLog();

  Span day(64);
  day.addNumber(log.begin(), log.end());  // the joint under test

  Check::eq(day.size(), static_cast<unsigned int>(kSwipeCount),
            "every event crossed over in one call");
  Check::ok(std::equal(day.begin(), day.end(), kSwipes),
            "in the order the stack held them");

  Check::eq(day.shortestSpan(), 3u,
            "shortestSpan finds the 3-second pair: the tailgating alarm");
  Check::eq(day.longestSpan(), 32400u,
            "longestSpan is the working day, 9 hours");

  // The reverse range works too, and the answers are order-independent.
  Span reversed(64);
  reversed.addNumber(log.rbegin(), log.rend());
  Check::eq(reversed.shortestSpan(), day.shortestSpan(),
            "filled from rbegin()/rend(), the same answer");
  Check::eq(reversed.longestSpan(), day.longestSpan(), "for both queries");

  // And a const stack hands over const_iterators, which the range overload
  // takes just as happily.
  const MutantStack<int>& frozen = log;
  Span fromConst(64);
  fromConst.addNumber(frozen.begin(), frozen.end());
  Check::eq(fromConst.size(), day.size(), "const_iterators work as a range");
}

static void test_the_capacity_check_still_bites() {
  Check::section("the joint does not weaken either side's guarantees");

  MutantStack<int> log = buildLog();

  // A Span too small for the log must reject the whole range and stay empty.
  Span tooSmall(3);
  bool threw = false;
  try {
    tooSmall.addNumber(log.begin(), log.end());
  } catch (const Span::SpanFullException&) {
    threw = true;
  }
  Check::ok(threw, "5 events into a Span(3) is refused");
  Check::eq(tooSmall.size(), 0u, "and nothing was partially inserted");

  // The stack is untouched by the failed handover.
  Check::eq(log.size(), kSwipeCount, "the log did not lose anything");
  Check::ok(std::equal(log.begin(), log.end(), kSwipes), "nor reorder it");

  // A Span sized exactly right takes it.
  Span exact(static_cast<unsigned int>(kSwipeCount));
  exact.addNumber(log.begin(), log.end());
  Check::ok(exact.full(), "a Span sized to the log ends up exactly full");
}

static void test_live_pipeline() {
  Check::section("the pipeline over a log that keeps changing");

  MutantStack<int> log = buildLog();

  // The controller undoes the last event.
  log.pop();
  Span afterUndo(64);
  afterUndo.addNumber(log.begin(), log.end());
  Check::eq(afterUndo.size(), 4u, "the Span sees the undo");
  Check::eq(afterUndo.longestSpan(), 15000u,
            "and the working day shortened with it");
  Check::eq(afterUndo.shortestSpan(), 3u, "the alarm still stands");

  // A visitor badges in twice in a second: a new shortest span.
  log.push(32000);
  log.push(32001);
  Span afterVisitor(64);
  afterVisitor.addNumber(log.begin(), log.end());
  Check::eq(afterVisitor.shortestSpan(), 1u, "the visitor is faster still");

  // And ex00 can confirm the visitor is in the log, on the same live data.
  Check::eq(*easyfind(log, 32001), 32001, "easyfind sees the new event");

  // The stack half of the interface is unaffected by all the reading.
  Check::eq(log.top(), 32001, "top() is still the most recent event");
  Check::eq(log.size(), static_cast<std::size_t>(6), "and nothing was consumed");
}

static void test_all_three_backings() {
  Check::section("the pipeline does not care what backs the stack");

  MutantStack<int, std::vector<int> > onVector;
  MutantStack<int, std::list<int> > onList;
  for (std::size_t i = 0; i < kSwipeCount; ++i) {
    onVector.push(kSwipes[i]);
    onList.push(kSwipes[i]);
  }

  Span fromVector(64);
  fromVector.addNumber(onVector.begin(), onVector.end());
  Check::eq(fromVector.shortestSpan(), 3u, "vector-backed stack -> Span");

  // A list gives bidirectional iterators, so std::distance walks instead of
  // subtracting. Span's capacity check calls distance before inserting, which
  // is the ponytail comment in ex01 made concrete: the range overload needs
  // forward iterators, and a list's qualify.
  Span fromList(64);
  fromList.addNumber(onList.begin(), onList.end());
  Check::eq(fromList.shortestSpan(), 3u, "list-backed stack -> Span");
  Check::eq(fromList.longestSpan(), fromVector.longestSpan(),
            "identical answers from both");

  Check::eq(*easyfind(onList, 43200), 43200, "easyfind over a list-backed one");
}

int main() {
  test_easyfind_over_a_mutantstack();
  test_span_fed_from_a_mutantstack();
  test_the_capacity_check_still_bites();
  test_live_pipeline();
  test_all_three_backings();

  return Check::report("ex00 + ex01 + ex02 - the module as one pipeline");
}
