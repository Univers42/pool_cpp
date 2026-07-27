/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:43:11 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Assert-style Span tests (C++98). Exits non-zero if any check fails.
// Absorbs the old fuzzy.cpp scenarios (INT extremes, zeros, range
// boundaries, 100k random) but actually asserts instead of printing.

#include <climits>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "../Span.hpp"

static int g_fails = 0;

#define CHECK(cond)                                                          \
  do {                                                                       \
    if (!(cond)) {                                                           \
      std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << ": " << #cond   \
                << std::endl;                                                \
      ++g_fails;                                                             \
    }                                                                        \
  } while (0)

static void testSubjectExample() {
  Span sp(5);
  sp.addNumber(6);
  sp.addNumber(3);
  sp.addNumber(17);
  sp.addNumber(9);
  sp.addNumber(11);
  CHECK(sp.shortestSpan() == 2);
  CHECK(sp.longestSpan() == 14);
}

static void testExceptions() {
  Span sp(1);
  bool threw = false;
  try {
    sp.shortestSpan();  // empty
  } catch (Span::NotEnoughElementsException&) {
    threw = true;
  }
  CHECK(threw);

  sp.addNumber(42);
  threw = false;
  try {
    sp.longestSpan();  // one element
  } catch (Span::NotEnoughElementsException&) {
    threw = true;
  }
  CHECK(threw);

  threw = false;
  try {
    sp.addNumber(43);  // full
  } catch (Span::SpanFullException&) {
    threw = true;
  }
  CHECK(threw);
}

static void testRangeInsertBoundary() {
  Span sp(10);
  std::vector<int> five(5, 42);
  std::vector<int> six(6, 84);
  sp.addNumber(five.begin(), five.end());  // 5/10: fits
  bool threw = false;
  try {
    sp.addNumber(six.begin(), six.end());  // 6 into 5 slots: must throw
  } catch (Span::SpanFullException&) {
    threw = true;
  }
  CHECK(threw);
  sp.addNumber(five.begin(), five.end());  // rejected insert added nothing
  CHECK(sp.shortestSpan() == 0);
  CHECK(sp.longestSpan() == 0);
}

static void testIntExtremes() {
  Span sp(2);
  sp.addNumber(INT_MAX);
  sp.addNumber(INT_MIN);
  CHECK(sp.longestSpan() == UINT_MAX);  // no signed overflow
  CHECK(sp.shortestSpan() == UINT_MAX);
}

static void testBigDeterministic() {
  const int n = 20000;
  std::vector<int> seq;
  seq.reserve(n);
  for (int i = 0; i < n; ++i) seq.push_back(i * 3);
  Span sp(n);
  sp.addNumber(seq.begin(), seq.end());
  CHECK(sp.shortestSpan() == 3);
  CHECK(sp.longestSpan() == static_cast<unsigned int>((n - 1) * 3));
}

static void testBigRandom() {
  const int n = 100000;
  std::srand(42);
  std::vector<int> data;
  data.reserve(n);
  for (int i = 0; i < n; ++i)
    data.push_back(std::rand() * (std::rand() % 2 ? 1 : -1));
  Span sp(n);
  sp.addNumber(data.begin(), data.end());
  CHECK(sp.shortestSpan() <= sp.longestSpan());
  bool threw = false;
  try {
    sp.addNumber(0);  // exactly full after the range insert
  } catch (Span::SpanFullException&) {
    threw = true;
  }
  CHECK(threw);

  // Oversized range insert must be ATOMIC: it throws and inserts nothing.
  // Proof without a size getter: after the failed range, exactly 3 singles
  // still fit (so size was still 1), and the 4th overflows.
  {
    Span atomic(4);
    atomic.addNumber(1);
    std::vector<int> five(5, 7);
    bool rangeThrew = false;
    try {
      atomic.addNumber(five.begin(), five.end());
    } catch (Span::SpanFullException&) {
      rangeThrew = true;
    }
    CHECK(rangeThrew);
    atomic.addNumber(2);
    atomic.addNumber(3);
    atomic.addNumber(4);  // fits: proves the failed range inserted nothing
    bool fullThrew = false;
    try {
      atomic.addNumber(5);
    } catch (Span::SpanFullException&) {
      fullThrew = true;
    }
    CHECK(fullThrew);
  }
}

static void testCanonicalForm() {
  Span a(3);
  a.addNumber(1);
  a.addNumber(10);
  Span b(a);  // copy ctor
  CHECK(b.longestSpan() == 9);
  Span c;
  c = a;  // copy assignment
  CHECK(c.shortestSpan() == 9);
  a.addNumber(5);  // deep copy: a's change must not affect b or c
  CHECK(a.shortestSpan() == 4);
  CHECK(b.longestSpan() == 9);
  CHECK(c.longestSpan() == 9);
}

int main() {
  testSubjectExample();
  testExceptions();
  testRangeInsertBoundary();
  testIntExtremes();
  testBigDeterministic();
  testBigRandom();
  testCanonicalForm();
  if (g_fails) {
    std::cerr << g_fails << " check(s) failed" << std::endl;
    return 1;
  }
  std::cout << "All Span tests passed" << std::endl;
  return 0;
}
