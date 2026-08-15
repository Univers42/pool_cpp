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

// The subject's requirements for ex00, one sentence at a time, each turned
// into an assertion. tests/test.cpp goes well past this; the point of keeping
// them apart is that if a refactor breaks something, this file says whether it
// broke the exercise or only broke one of my own extra guarantees.
//
// The subject asks for:
//   1. a function template easyfind accepting a type T;
//   2. two parameters, the first of type T, the second an int;
//   3. T assumed to be a container of integers;
//   4. find the FIRST occurrence of the second parameter in the first;
//   5. if no occurrence is found, throw an exception or return an error value;
//   6. associative containers do not have to be handled.
//
// The evaluation sheet adds one more, and it is the one that fails people:
//   7. it HAS to use STL algorithms — a manual iterator loop counts as wrong.
//      That is a property of the source, not of the runtime, so it is checked
//      by tests/eval.sh, which greps the header for a hand-rolled loop.

#include <algorithm>
#include <deque>
#include <list>
#include <set>
#include <stdexcept>
#include <vector>

#include "../easyfind.hpp"
#include "check.hpp"

static void requirement_1_and_2_signature() {
  Check::section("1+2. a template taking (T, int)");

  std::vector<int> v;
  v.push_back(1);

  // If easyfind were not a template, this would not compile for three
  // different container types below. If it did not take an int second, the
  // literal would not bind.
  Check::eq(*easyfind(v, 1), 1, "easyfind(container, int) compiles and runs");

  // The second parameter really is an int, not a T::value_type deduced from
  // the container: a plain int literal is enough, no cast, no explicit
  // template argument list.
  const int asVariable = 1;
  Check::eq(*easyfind(v, asVariable), 1, "an int variable works the same");
}

static void requirement_3_container_of_integers() {
  Check::section("3. any container of integers");

  const int values[] = {5, 6, 7};
  std::vector<int> asVector(values, values + 3);
  std::list<int> asList(values, values + 3);
  std::deque<int> asDeque(values, values + 3);

  Check::eq(*easyfind(asVector, 6), 6, "std::vector<int>");
  Check::eq(*easyfind(asList, 6), 6, "std::list<int>");
  Check::eq(*easyfind(asDeque, 6), 6, "std::deque<int>");
}

static void requirement_4_first_occurrence() {
  Check::section("4. the FIRST occurrence");

  const int values[] = {9, 4, 9, 4, 9};
  std::vector<int> v(values, values + 5);

  Check::eq(std::distance(v.begin(), easyfind(v, 9)), 0, "first 9 is index 0");
  Check::eq(std::distance(v.begin(), easyfind(v, 4)), 1, "first 4 is index 1");

  // Stated as the library states it: same iterator std::find would give.
  Check::same(easyfind(v, 9), std::find(v.begin(), v.end(), 9),
              "identical to std::find's answer");
}

static void requirement_5_no_occurrence() {
  Check::section("5. no occurrence: this implementation throws");

  std::vector<int> v;
  v.push_back(1);
  v.push_back(2);

  bool threw = false;
  try {
    easyfind(v, 3);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "a miss throws std::out_of_range");

  // The subject allows an error value instead. Choosing the exception means
  // there is no error value to test for — which is the point: a caller cannot
  // forget to check something that does not exist.
  std::vector<int> empty;
  threw = false;
  try {
    easyfind(empty, 0);
  } catch (const std::exception&) {
    threw = true;
  }
  Check::ok(threw, "including on an empty container");
}

static void requirement_6_associative_not_required() {
  Check::section("6. associative containers: not required, still work");

  // The subject excuses them. std::set happens to work anyway, because its
  // value_type is int and it exposes begin()/end() — nothing in easyfind ever
  // asked for contiguity. std::map does not and cannot: its value_type is a
  // pair, so *it == int does not compile. tests/compile_fail.sh proves that.
  std::set<int> s;
  s.insert(3);
  s.insert(1);
  s.insert(2);

  Check::eq(*easyfind(s, 2), 2, "std::set<int> works for free");
  Check::eq(*s.begin(), 1, "and the set is still a set, sorted");
}

int main() {
  requirement_1_and_2_signature();
  requirement_3_container_of_integers();
  requirement_4_first_occurrence();
  requirement_5_no_occurrence();
  requirement_6_associative_not_required();

  return Check::report("ex00 easyfind - the subject's own requirements");
}
