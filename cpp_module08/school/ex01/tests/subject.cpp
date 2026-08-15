/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subject.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// The subject's requirements for ex01, one at a time, each turned into an
// assertion — including its example main, run verbatim with std::cout
// captured, so "prints 2 then 14" is checked as text rather than eyeballed.
//
// The subject asks for:
//   1. a class Span storing a maximum of N integers, N an unsigned int and
//      the only constructor parameter;
//   2. addNumber() to add a single number, throwing once N are stored;
//   3. shortestSpan() and longestSpan() over all stored numbers;
//   4. an exception when fewer than two numbers are stored;
//   5. the example main printing 2 then 14;
//   6. handling at least 10,000 numbers.
//
// The evaluation sheet adds:
//   7. member functions should use STL algorithms as much as possible, and
//      the shortest span "can't be done only by subtracting the two lowest
//      numbers" — checked here by construction and by tests/eval.sh, which
//      greps Span.cpp for the hand-written loop that would replace them;
//   8. "a way to add numbers that's more practical than calling addNumber()
//      repeatedly" — the iterator-range overload, requirement 9 below.

#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "../Span.hpp"
#include "check.hpp"

// The subject's main, character for character, with its output captured.
static std::string runSubjectMain() {
  std::ostringstream captured;
  std::streambuf* saved = std::cout.rdbuf(captured.rdbuf());

  {
    Span sp = Span(5);

    sp.addNumber(6);
    sp.addNumber(3);
    sp.addNumber(17);
    sp.addNumber(9);
    sp.addNumber(11);

    std::cout << sp.shortestSpan() << std::endl;
    std::cout << sp.longestSpan() << std::endl;
  }

  std::cout.rdbuf(saved);
  return captured.str();
}

static void requirement_1_capacity_n() {
  Check::section("1. stores at most N, N the only ctor parameter");

  const unsigned int n = 5;
  Span sp(n);
  Check::eq(sp.maxSize(), n, "Span(5) has capacity 5");

  for (unsigned int i = 0; i < n; ++i) sp.addNumber(static_cast<int>(i));
  Check::eq(sp.size(), n, "exactly N fit");
  Check::ok(sp.full(), "and then it is full");
}

static void requirement_2_add_number_throws() {
  Check::section("2. addNumber throws once N are stored");

  Span sp(2);
  sp.addNumber(1);
  sp.addNumber(2);

  bool threw = false;
  try {
    sp.addNumber(3);
  } catch (const std::exception&) {
    threw = true;
  }
  Check::ok(threw, "the N+1'th addNumber throws");
  Check::eq(sp.size(), 2u, "and stores nothing");
}

static void requirement_3_the_two_queries() {
  Check::section("3. shortestSpan and longestSpan over all stored numbers");

  Span sp(5);
  sp.addNumber(6);
  sp.addNumber(3);
  sp.addNumber(17);
  sp.addNumber(9);
  sp.addNumber(11);

  Check::eq(sp.shortestSpan(), 2u, "shortest is 11 - 9, not 6 - 3");
  Check::eq(sp.longestSpan(), 14u, "longest is 17 - 3");
}

static void requirement_4_too_few_numbers() {
  Check::section("4. zero or one stored number: an exception, not an answer");

  Span none(5);
  bool threwShort = false;
  bool threwLong = false;
  try {
    none.shortestSpan();
  } catch (const std::exception&) {
    threwShort = true;
  }
  try {
    none.longestSpan();
  } catch (const std::exception&) {
    threwLong = true;
  }
  Check::ok(threwShort && threwLong, "no numbers stored: both throw");

  Span one(5);
  one.addNumber(1);
  threwShort = false;
  threwLong = false;
  try {
    one.shortestSpan();
  } catch (const std::exception&) {
    threwShort = true;
  }
  try {
    one.longestSpan();
  } catch (const std::exception&) {
    threwLong = true;
  }
  Check::ok(threwShort && threwLong, "one number stored: both throw");
}

static void requirement_5_example_output() {
  Check::section("5. the example main prints 2 then 14");

  const std::string output = runSubjectMain();
  Check::eq(output, std::string("2\n14\n"),
            "captured stdout, byte for byte");
}

static void requirement_6_ten_thousand() {
  Check::section("6. handles at least 10,000 numbers");

  Span sp(10000);
  for (int i = 0; i < 10000; ++i) sp.addNumber(i * 7);

  Check::eq(sp.size(), 10000u, "all 10,000 stored");
  Check::eq(sp.shortestSpan(), 7u, "shortest gap across 10,000 values");
  Check::eq(sp.longestSpan(), 69993u, "longest gap across 10,000 values");

  bool threw = false;
  try {
    sp.addNumber(1);
  } catch (const std::exception&) {
    threw = true;
  }
  Check::ok(threw, "and the ceiling still holds at 10,000");
}

static void requirement_7_stl_not_the_two_lowest() {
  Check::section("7. the shortest span is not the two lowest, subtracted");

  // If shortestSpan were "sorted[1] - sorted[0]" this would answer 100.
  Span sp(4);
  sp.addNumber(0);
  sp.addNumber(100);
  sp.addNumber(900);
  sp.addNumber(901);
  Check::eq(sp.shortestSpan(), 1u, "901 - 900, at the top of the range");

  // And if it were "max - min" it would answer 901.
  Check::eq(sp.longestSpan(), 901u, "the longest is a separate question");
}

static void requirement_8_bulk_add() {
  Check::section("8. a more practical way to add than one call at a time");

  std::vector<int> values;
  for (int i = 0; i < 10000; ++i) values.push_back(i * 3);

  Span sp(10000);
  sp.addNumber(values.begin(), values.end());  // one call, 10,000 numbers

  Check::eq(sp.size(), 10000u, "one call filled the whole Span");
  Check::eq(sp.shortestSpan(), 3u, "and the values arrived intact");

  // It is a template over the iterator type, so a pointer pair works too.
  const int raw[] = {1, 2, 3};
  Span fromArray(3);
  fromArray.addNumber(raw, raw + 3);
  Check::eq(fromArray.size(), 3u, "two pointers are an iterator pair");
}

int main() {
  requirement_1_capacity_n();
  requirement_2_add_number_throws();
  requirement_3_the_two_queries();
  requirement_4_too_few_numbers();
  requirement_5_example_output();
  requirement_6_ten_thousand();
  requirement_7_stl_not_the_two_lowest();
  requirement_8_bulk_add();

  return Check::report("ex01 Span - the subject's own requirements");
}
