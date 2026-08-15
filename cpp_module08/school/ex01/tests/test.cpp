/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:45:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Fixed-case tests for Span. The randomised half is tests/fuzz.cpp, which
// checks every query against an independent O(n^2) oracle, and the subject's
// own example is tests/subject.cpp with its output asserted verbatim.
//
//   make test               build and run everything
//   ./build/bin/test        just this file
//   ./tests/mutants.sh      breaks Span.cpp, these tests must fail
//   ./tests/compile_fail.sh the code that must NOT compile
//   valgrind --leak-check=full ./build/bin/test
//
// The two sections worth reading are "shortest is not the two lowest", which
// is the mistake the evaluation sheet singles out, and "range insert is
// atomic", which pins the strong guarantee the class actually offers.

#include <algorithm>
#include <limits>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include "../Span.hpp"
#include "check.hpp"

static const unsigned int kUintMax = std::numeric_limits<unsigned int>::max();
static const int kIntMin = std::numeric_limits<int>::min();
static const int kIntMax = std::numeric_limits<int>::max();

// Builds a Span from an array, sized exactly.
static Span spanOf(const int* values, unsigned int n) {
  Span s(n);
  s.addNumber(values, values + n);
  return s;
}

// The O(n^2) definition of the two queries, straight from the subject's
// wording: the smallest and largest distance between ANY two stored numbers.
// Slow, obviously correct, and independent of everything Span does.
static unsigned int bruteShortest(const std::vector<int>& v) {
  unsigned int best = kUintMax;
  for (std::size_t i = 0; i < v.size(); ++i) {
    for (std::size_t j = i + 1; j < v.size(); ++j) {
      const unsigned int a = static_cast<unsigned int>(v[i]);
      const unsigned int b = static_cast<unsigned int>(v[j]);
      const unsigned int gap = v[i] < v[j] ? b - a : a - b;
      if (gap < best) best = gap;
    }
  }
  return best;
}

static unsigned int bruteLongest(const std::vector<int>& v) {
  unsigned int best = 0;
  for (std::size_t i = 0; i < v.size(); ++i) {
    for (std::size_t j = i + 1; j < v.size(); ++j) {
      const unsigned int a = static_cast<unsigned int>(v[i]);
      const unsigned int b = static_cast<unsigned int>(v[j]);
      const unsigned int gap = v[i] < v[j] ? b - a : a - b;
      if (gap > best) best = gap;
    }
  }
  return best;
}

// ─────────────────────────────────────────────────────────────────────────────
static void test_subject_example() {
  Check::section("the subject's example: 2 and 14");

  Span sp = Span(5);
  sp.addNumber(6);
  sp.addNumber(3);
  sp.addNumber(17);
  sp.addNumber(9);
  sp.addNumber(11);

  Check::eq(sp.shortestSpan(), 2u, "shortestSpan is 11 - 9");
  Check::eq(sp.longestSpan(), 14u, "longestSpan is 17 - 3");
}

static void test_shortest_is_not_the_two_lowest() {
  Check::section("shortest span is not 'the two lowest, subtracted'");

  // The evaluation sheet calls this out by name. Every case here is one where
  // the naive answer and the right answer differ.
  const int subject[] = {6, 3, 17, 9, 11};       // low pair 3, real 2
  const int backLoaded[] = {0, 100, 900, 901};   // low pair 100, real 1
  const int middle[] = {0, 50, 51, 1000};        // low pair 50, real 1
  const int negatives[] = {-100, -3, 40, 41};    // low pair 97, real 1

  Check::eq(spanOf(subject, 5).shortestSpan(), 2u, "subject example");
  Check::eq(spanOf(backLoaded, 4).shortestSpan(), 1u,
            "smallest gap at the top of the range");
  Check::eq(spanOf(middle, 4).shortestSpan(), 1u,
            "smallest gap in the middle");
  Check::eq(spanOf(negatives, 4).shortestSpan(), 1u,
            "smallest gap after the sign change");

  // And the naive answer really is different, so these cases have teeth.
  std::vector<int> v(backLoaded, backLoaded + 4);
  std::sort(v.begin(), v.end());
  const unsigned int naive = static_cast<unsigned int>(v[1]) -
                             static_cast<unsigned int>(v[0]);
  Check::eq(naive, 100u, "the naive algorithm would have answered 100");

  // Order of insertion must not matter: the same multiset in any order.
  const int shuffled[] = {901, 0, 900, 100};
  Check::eq(spanOf(shuffled, 4).shortestSpan(), 1u,
            "insertion order does not change the answer");
  Check::eq(spanOf(shuffled, 4).longestSpan(), 901u, "nor for the longest");
}

static void test_longest_span() {
  Check::section("longest span is max - min");

  const int values[] = {6, 3, 17, 9, 11};
  Check::eq(spanOf(values, 5).longestSpan(), 14u, "17 - 3");

  const int sorted[] = {1, 2, 3, 4};
  Check::eq(spanOf(sorted, 4).longestSpan(), 3u, "already sorted");

  const int reversed[] = {4, 3, 2, 1};
  Check::eq(spanOf(reversed, 4).longestSpan(), 3u, "reverse sorted");

  const int allSame[] = {7, 7, 7};
  Check::eq(spanOf(allSame, 3).longestSpan(), 0u, "all equal: 0");
  Check::eq(spanOf(allSame, 3).shortestSpan(), 0u, "and the shortest too");

  const int twoOnly[] = {-5, 5};
  Check::eq(spanOf(twoOnly, 2).longestSpan(), 10u, "the minimum viable Span");
  Check::eq(spanOf(twoOnly, 2).shortestSpan(), 10u,
            "with two values the two answers coincide");
}

static void test_capacity() {
  Check::section("N is a hard ceiling");

  Span sp(3);
  sp.addNumber(1);
  sp.addNumber(2);
  Check::eq(sp.size(), 2u, "two stored");
  Check::ok(!sp.full(), "not full yet");
  sp.addNumber(3);
  Check::ok(sp.full(), "full now");

  bool threw = false;
  std::string message;
  try {
    sp.addNumber(4);
  } catch (const Span::SpanFullException& e) {
    threw = true;
    message = e.what();
  }
  Check::ok(threw, "the fourth addNumber throws SpanFullException");
  Check::ok(message.find("full") != std::string::npos,
            "what() says the Span is full: " + message);
  Check::eq(sp.size(), 3u, "and the rejected value was not stored");

  // A rejected add must not corrupt the accounting: still exactly full.
  try {
    sp.addNumber(5);
  } catch (const Span::SpanFullException&) {
  }
  Check::eq(sp.size(), 3u, "a second rejection changes nothing either");

  Check::eq(sp.maxSize(), 3u, "maxSize is still N");
}

static void test_born_full() {
  Check::section("Span(0) and the default Span are full from birth");

  Span zero(0);
  Check::ok(zero.full(), "Span(0) is full");
  Check::ok(zero.empty(), "and empty at the same time");
  Check::eq(zero.maxSize(), 0u, "capacity zero");

  bool threw = false;
  try {
    zero.addNumber(1);
  } catch (const Span::SpanFullException&) {
    threw = true;
  }
  Check::ok(threw, "nothing fits in it");

  Span defaulted;
  Check::eq(defaulted.maxSize(), 0u, "the default ctor means N = 0");
  threw = false;
  try {
    defaulted.addNumber(1);
  } catch (const Span::SpanFullException&) {
    threw = true;
  }
  Check::ok(threw, "same for the default-constructed one");
}

static void test_too_few_elements() {
  Check::section("fewer than two values: both queries refuse");

  Span empty(5);
  bool shortThrew = false;
  bool longThrew = false;
  std::string message;
  try {
    empty.shortestSpan();
  } catch (const Span::NotEnoughElementsException& e) {
    shortThrew = true;
    message = e.what();
  }
  try {
    empty.longestSpan();
  } catch (const Span::NotEnoughElementsException&) {
    longThrew = true;
  }
  Check::ok(shortThrew, "shortestSpan on an empty Span throws");
  Check::ok(longThrew, "longestSpan on an empty Span throws");
  Check::ok(message.find("2") != std::string::npos,
            "what() names the requirement: " + message);

  Span one(5);
  one.addNumber(42);
  shortThrew = false;
  longThrew = false;
  try {
    one.shortestSpan();
  } catch (const Span::NotEnoughElementsException&) {
    shortThrew = true;
  }
  try {
    one.longestSpan();
  } catch (const Span::NotEnoughElementsException&) {
    longThrew = true;
  }
  Check::ok(shortThrew, "one value is not enough for shortestSpan");
  Check::ok(longThrew, "nor for longestSpan");

  // Exactly two is enough, including two equal ones — a zero span is an
  // answer, not a refusal.
  one.addNumber(42);
  Check::eq(one.shortestSpan(), 0u, "two equal values give a span of 0");
  Check::eq(one.longestSpan(), 0u, "for both queries");
}

static void test_exception_hierarchy() {
  Check::section("both exceptions are catchable as std::exception");

  Span full(1);
  full.addNumber(1);
  bool asBase = false;
  try {
    full.addNumber(2);
  } catch (const std::exception&) {
    asBase = true;
  }
  Check::ok(asBase, "SpanFullException derives from std::exception");

  Span thin(5);
  asBase = false;
  try {
    thin.shortestSpan();
  } catch (const std::exception&) {
    asBase = true;
  }
  Check::ok(asBase, "NotEnoughElementsException does too");

  // They must not be the same type, or a caller could not tell "no room" from
  // "no answer".
  bool caughtWrongOne = false;
  try {
    thin.shortestSpan();
  } catch (const Span::SpanFullException&) {
    caughtWrongOne = true;
  } catch (const Span::NotEnoughElementsException&) {
  }
  Check::ok(!caughtWrongOne, "the two exception types are distinguishable");
}

static void test_range_insert() {
  Check::section("addNumber(first, last) takes any iterator pair");

  const int raw[] = {5, 6, 7};
  std::vector<int> asVector(raw, raw + 3);
  std::list<int> asList(raw, raw + 3);

  Span fromVector(3);
  fromVector.addNumber(asVector.begin(), asVector.end());
  Check::eq(fromVector.size(), 3u, "from a vector");

  Span fromList(3);
  fromList.addNumber(asList.begin(), asList.end());
  Check::eq(fromList.size(), 3u, "from a list (not random access)");

  Span fromPointers(3);
  fromPointers.addNumber(raw, raw + 3);
  Check::eq(fromPointers.size(), 3u, "from two raw pointers");

  Span fromSpan(3);
  fromSpan.addNumber(fromVector.begin(), fromVector.end());
  Check::eq(fromSpan.size(), 3u, "from another Span");

  // All four hold the same values in the same order.
  Check::ok(std::equal(fromVector.begin(), fromVector.end(), raw),
            "vector source: contents and order preserved");
  Check::ok(std::equal(fromList.begin(), fromList.end(), raw),
            "list source: same");
  Check::ok(std::equal(fromSpan.begin(), fromSpan.end(), raw),
            "Span source: same");

  // Mixing single and range adds accumulates.
  Span mixed(6);
  mixed.addNumber(1);
  mixed.addNumber(raw, raw + 3);
  mixed.addNumber(9);
  Check::eq(mixed.size(), 5u, "single, range and single together");
  const int expected[] = {1, 5, 6, 7, 9};
  Check::ok(std::equal(mixed.begin(), mixed.end(), expected),
            "in the order they were added");

  // An empty range is a legal no-op, even on a Span with no room left.
  Span noRoom(1);
  noRoom.addNumber(0);
  bool threw = false;
  try {
    noRoom.addNumber(raw, raw);
  } catch (const std::exception&) {
    threw = true;
  }
  Check::ok(!threw, "an empty range never throws, even on a full Span");
  Check::eq(noRoom.size(), 1u, "and adds nothing");
}

static void test_range_insert_is_atomic() {
  Check::section("a range that does not fit inserts nothing at all");

  Span sp(4);
  sp.addNumber(1);

  const int tooMany[] = {2, 3, 4, 5, 6};  // 1 + 5 > 4
  bool threw = false;
  try {
    sp.addNumber(tooMany, tooMany + 5);
  } catch (const Span::SpanFullException&) {
    threw = true;
  }
  Check::ok(threw, "an oversized range throws SpanFullException");
  Check::eq(sp.size(), 1u, "and left the Span exactly as it was");

  // The behavioural version of the same claim, which holds even without a
  // size() getter: if nothing was inserted, exactly three singles still fit.
  sp.addNumber(2);
  sp.addNumber(3);
  sp.addNumber(4);
  threw = false;
  try {
    sp.addNumber(5);
  } catch (const Span::SpanFullException&) {
    threw = true;
  }
  Check::ok(threw, "exactly three more fit, so the capacity was untouched");

  // A range that fits exactly must succeed: the check is >, not >=.
  Span exact(3);
  const int three[] = {1, 2, 3};
  threw = false;
  try {
    exact.addNumber(three, three + 3);
  } catch (const std::exception&) {
    threw = true;
  }
  Check::ok(!threw, "a range that fills the Span exactly is accepted");
  Check::eq(exact.size(), 3u, "all three stored");

  // One past exact must fail.
  Span oneShort(2);
  threw = false;
  try {
    oneShort.addNumber(three, three + 3);
  } catch (const Span::SpanFullException&) {
    threw = true;
  }
  Check::ok(threw, "one element too many is rejected");
  Check::eq(oneShort.size(), 0u, "and nothing was inserted");
}

static void test_overflow() {
  Check::section("INT_MIN to INT_MAX: the span no signed type can hold");

  Span extremes(2);
  extremes.addNumber(kIntMin);
  extremes.addNumber(kIntMax);
  Check::eq(extremes.longestSpan(), kUintMax, "longest is exactly UINT_MAX");
  Check::eq(extremes.shortestSpan(), kUintMax,
            "with two values, so is the shortest");

  // A third value in the middle changes the shortest but not the longest.
  Span three(3);
  three.addNumber(kIntMin);
  three.addNumber(0);
  three.addNumber(kIntMax);
  Check::eq(three.longestSpan(), kUintMax, "longest unchanged");
  Check::eq(three.shortestSpan(), 2147483647u,
            "shortest is now 0 - INT_MIN, itself larger than INT_MAX");

  // Adjacent extremes: a gap of 1 across the sign boundary.
  Span boundary(2);
  boundary.addNumber(-1);
  boundary.addNumber(0);
  Check::eq(boundary.shortestSpan(), 1u, "-1 to 0 is a distance of 1");

  Span negatives(3);
  negatives.addNumber(-10);
  negatives.addNumber(-20);
  negatives.addNumber(-13);
  Check::eq(negatives.shortestSpan(), 3u, "all-negative values, shortest");
  Check::eq(negatives.longestSpan(), 10u, "all-negative values, longest");

  // Cross-check both against the brute-force definition.
  std::vector<int> v;
  v.push_back(kIntMin);
  v.push_back(kIntMax);
  v.push_back(0);
  v.push_back(-1);
  Span mixed(4);
  mixed.addNumber(v.begin(), v.end());
  Check::eq(mixed.shortestSpan(), bruteShortest(v),
            "shortest agrees with the O(n^2) definition");
  Check::eq(mixed.longestSpan(), bruteLongest(v),
            "longest agrees with the O(n^2) definition");
}

static void test_queries_are_const_and_pure() {
  Check::section("both queries are const and leave the Span alone");

  const int raw[] = {6, 3, 17, 9, 11};
  Span sp(5);
  sp.addNumber(raw, raw + 5);

  sp.shortestSpan();
  sp.longestSpan();
  Check::ok(std::equal(sp.begin(), sp.end(), raw),
            "shortestSpan sorts a COPY: stored order is untouched");
  Check::eq(sp.size(), 5u, "and nothing was added or removed");

  // Callable on a const Span at all, which is the compile-time half.
  const Span& frozen = sp;
  Check::eq(frozen.shortestSpan(), 2u, "callable through a const reference");
  Check::eq(frozen.longestSpan(), 14u, "both of them");

  // Repeatable: same answer twice in a row.
  Check::eq(sp.shortestSpan(), sp.shortestSpan(), "shortestSpan is stable");
  Check::eq(sp.longestSpan(), sp.longestSpan(), "longestSpan is stable");
}

static void test_canonical_form() {
  Check::section("orthodox canonical form");

  Span original(5);
  original.addNumber(1);
  original.addNumber(10);

  Span copy(original);
  Check::eq(copy.size(), 2u, "copy ctor copies the values");
  Check::eq(copy.maxSize(), 5u, "and the capacity");
  Check::eq(copy.longestSpan(), 9u, "and answers the same");

  Span assigned;
  assigned = original;
  Check::eq(assigned.size(), 2u, "assignment copies the values");
  Check::eq(assigned.maxSize(), 5u,
            "and N, so a Span() target does not stay at capacity 0");

  // Independence in both directions.
  copy.addNumber(100);
  Check::eq(original.size(), 2u, "mutating the copy leaves the original");
  assigned.addNumber(200);
  Check::eq(original.size(), 2u, "mutating the assignee leaves it too");
  original.addNumber(300);
  Check::eq(copy.size(), 3u, "and the reverse");

  // Self-assignment through an alias, so the compiler does not fold it away.
  Span& alias = original;
  original = alias;
  Check::eq(original.size(), 3u, "self-assignment keeps the values");
  Check::eq(original.maxSize(), 5u, "and the capacity");

  // Assigning a smaller-capacity Span over a larger one shrinks N as well.
  Span small(2);
  small.addNumber(7);
  Span big(100);
  big = small;
  Check::eq(big.maxSize(), 2u, "N follows the source, not the target");
  bool threw = false;
  try {
    big.addNumber(8);
    big.addNumber(9);
  } catch (const Span::SpanFullException&) {
    threw = true;
  }
  Check::ok(threw, "and the new, smaller ceiling is enforced");
}

static void test_accessors_and_printing() {
  Check::section("accessors and operator<<");

  Span sp(4);
  Check::ok(sp.empty(), "a fresh Span is empty");
  Check::ok(!sp.full(), "and not full");
  Check::same(sp.begin(), sp.end(), "begin() == end() when empty");

  sp.addNumber(3);
  sp.addNumber(-1);
  Check::eq(sp.size(), 2u, "size tracks the additions");
  Check::eq(static_cast<long>(std::distance(sp.begin(), sp.end())), 2L,
            "and so does the iterator range");
  Check::ok(!sp.empty(), "no longer empty");

  std::ostringstream out;
  out << sp;
  Check::eq(out.str(), std::string("Span(2/4) [3, -1]"),
            "operator<< prints size, capacity and contents in order");

  Span emptySpan(3);
  std::ostringstream emptyOut;
  emptyOut << emptySpan;
  Check::eq(emptyOut.str(), std::string("Span(0/3) []"), "empty Span prints");
}

static void test_stl_compatibility() {
  Check::section("a Span is a range, so <algorithm> works on it");

  const int raw[] = {28800, 28803, 43200, 61200};
  Span sp(6);
  sp.addNumber(raw, raw + 4);

  Check::eq(static_cast<long>(std::count(sp.begin(), sp.end(), 43200)), 1L,
            "std::count");
  Check::eq(*std::max_element(sp.begin(), sp.end()), 61200,
            "std::max_element");
  Check::eq(*std::min_element(sp.begin(), sp.end()), 28800,
            "std::min_element");
  Check::same(std::find(sp.begin(), sp.end(), 43200), sp.begin() + 2,
              "std::find lands on the right position");
  Check::ok(std::equal(sp.begin(), sp.end(), raw),
            "std::equal against the source array");

  // Copying a Span's contents out is a one-liner for the same reason.
  std::vector<int> copied(sp.begin(), sp.end());
  Check::eq(copied.size(), static_cast<std::size_t>(4), "range construction");

  // This is what makes the ex00 -> ex01 handover in main.cpp work: the only
  // interface easyfind ever needed is the one exercised right here.
  Check::eq(*(sp.begin() + 1), 28803, "const_iterator is random access");
}

static void test_scale() {
  Check::section("scale: 10,000 and 100,000 with known answers");

  // Values 5 apart, so the shortest span is 5 and the longest is 5*(n-1).
  std::vector<int> spaced;
  spaced.reserve(10000);
  for (int i = 0; i < 10000; ++i) spaced.push_back(i * 5);

  Span ten(10000);
  ten.addNumber(spaced.begin(), spaced.end());
  Check::eq(ten.size(), 10000u, "the subject's 10,000 fit");
  Check::eq(ten.shortestSpan(), 5u, "every gap is 5, so the shortest is 5");
  Check::eq(ten.longestSpan(), 49995u, "and the longest is 5 * 9999");

  // Same, ten times bigger, with one planted pair 1 apart placed off the
  // grid so it is the unique minimum.
  std::vector<int> big;
  big.reserve(100000);
  for (int i = 0; i < 99998; ++i) big.push_back(i * 5);
  big.push_back(-100);
  big.push_back(-99);

  Span hundred(100000);
  hundred.addNumber(big.begin(), big.end());
  Check::eq(hundred.size(), 100000u, "100,000 fit");
  Check::eq(hundred.shortestSpan(), 1u, "the planted pair is the minimum");
  Check::eq(hundred.longestSpan(),
            static_cast<unsigned int>(99997 * 5 + 100),
            "longest spans the whole range including the negative pair");

  // Duplicates at scale collapse the shortest span to zero.
  std::vector<int> withDupes(spaced);
  withDupes.push_back(spaced[42]);
  Span dupes(static_cast<unsigned int>(withDupes.size()));
  dupes.addNumber(withDupes.begin(), withDupes.end());
  Check::eq(dupes.shortestSpan(), 0u, "a repeated value means a span of 0");
}

int main() {
  test_subject_example();
  test_shortest_is_not_the_two_lowest();
  test_longest_span();
  test_capacity();
  test_born_full();
  test_too_few_elements();
  test_exception_hierarchy();
  test_range_insert();
  test_range_insert_is_atomic();
  test_overflow();
  test_queries_are_const_and_pure();
  test_canonical_form();
  test_accessors_and_printing();
  test_stl_compatibility();
  test_scale();

  return Check::report("ex01 Span - fixed cases");
}
