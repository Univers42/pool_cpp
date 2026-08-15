/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:10:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Fixed-case tests for easyfind. The randomised half lives in tests/fuzz.cpp
// and the subject's own requirements in tests/subject.cpp.
//
//   make test               build and run everything
//   ./build/bin/test        just this file
//   ./tests/mutants.sh      breaks easyfind.hpp, these tests must fail
//   ./tests/compile_fail.sh the code that must NOT compile
//   valgrind --leak-check=full ./build/bin/test
//
// The section worth reading is "search shape": Probe counts its own
// comparisons, which is how a test proves easyfind scans front to back and
// stops at the first hit rather than merely producing the right answer.

#include <deque>
#include <limits>
#include <list>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "../easyfind.hpp"
#include "check.hpp"

// std::find compares *it == value, so the element type never has to BE an int
// — it only has to compare with one. Probe exploits that to count how many
// comparisons a search performs.
struct Probe {
  int id;
  static int comparisons;

  Probe() : id(0) {}
  explicit Probe(int i) : id(i) {}

  bool operator==(int other) const {
    ++comparisons;
    return id == other;
  }
};
int Probe::comparisons = 0;

// Fills anything with push_back.
template <typename T>
static void fill(T& c, const int* values, std::size_t n) {
  for (std::size_t i = 0; i < n; ++i) c.push_back(values[i]);
}

static const int kData[] = {40, 21, 84, 21, 7};
static const std::size_t kCount = sizeof(kData) / sizeof(kData[0]);

// ─────────────────────────────────────────────────────────────────────────────
static void test_contract() {
  Check::section("returns a usable iterator on a hit");

  std::vector<int> v;
  fill(v, kData, kCount);

  std::vector<int>::iterator it = easyfind(v, 84);
  Check::eq(*it, 84, "dereferences to the value asked for");
  Check::eq(std::distance(v.begin(), it), 2, "at the right position");
  Check::ok(it != v.end(), "and never the end sentinel");

  std::vector<int>::iterator last = v.end();
  --last;
  Check::same(easyfind(v, 40), v.begin(), "the first element is reachable");
  Check::same(easyfind(v, 7), last, "so is the last one");
}

static void test_first_occurrence() {
  Check::section("first occurrence, not any occurrence");

  std::vector<int> v;
  fill(v, kData, kCount);  // 21 sits at index 1 and index 3

  Check::eq(std::distance(v.begin(), easyfind(v, 21)), 1,
            "duplicate resolves to the earlier index");

  // Same guarantee on a node-based container, where "earlier" is not an
  // address comparison but a link order.
  std::list<int> l;
  fill(l, kData, kCount);
  std::list<int>::iterator second = l.begin();
  ++second;
  Check::same(easyfind(l, 21), second, "list agrees on which 21 comes first");

  // A multiset stores both copies; easyfind must still stop at the first.
  std::multiset<int> ms(v.begin(), v.end());
  Check::eq(*easyfind(ms, 21), 21, "multiset hit");
  Check::same(easyfind(ms, 21), ms.lower_bound(21),
              "and it is the first of the equal range");
}

static void test_search_shape() {
  Check::section("search shape: front to back, stopping at the first hit");

  std::vector<Probe> probes;
  for (int i = 0; i < 6; ++i) probes.push_back(Probe(i * 10));

  Probe::comparisons = 0;
  std::vector<Probe>::iterator it = easyfind(probes, 20);
  Check::eq(it->id, 20, "found the probe");
  Check::eq(Probe::comparisons, 3,
            "exactly three comparisons: 0, 10, 20 then stop");

  Probe::comparisons = 0;
  bool threw = false;
  try {
    easyfind(probes, 999);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "a miss on probes throws too");
  Check::eq(Probe::comparisons, 6, "a miss costs exactly one pass, no more");

  Probe::comparisons = 0;
  easyfind(probes, 0);
  Check::eq(Probe::comparisons, 1, "a hit at index 0 costs one comparison");
}

static void test_miss_throws() {
  Check::section("a miss throws, and throws the documented type");

  std::vector<int> v;
  fill(v, kData, kCount);

  bool asExact = false;
  bool asLogic = false;
  bool asBase = false;
  std::string message;

  try {
    easyfind(v, 999);
    Check::ok(false, "a miss must not return normally");
  } catch (const std::out_of_range& e) {
    asExact = true;
    message = e.what();
  }
  try {
    easyfind(v, 999);
  } catch (const std::logic_error&) {
    asLogic = true;
  }
  try {
    easyfind(v, 999);
  } catch (const std::exception&) {
    asBase = true;
  }

  Check::ok(asExact, "std::out_of_range, the type vector::at uses");
  Check::ok(asLogic, "catchable as its base std::logic_error");
  Check::ok(asBase, "catchable as std::exception");
  Check::ok(!message.empty(), "what() is not empty");
  Check::ok(message.find("easyfind") != std::string::npos,
            "what() names the function that failed: " + message);
  Check::ok(message.find("not found") != std::string::npos,
            "what() says what went wrong: " + message);
}

static void test_empty_and_tiny() {
  Check::section("empty and one-element containers");

  std::vector<int> empty;
  bool threw = false;
  try {
    easyfind(empty, 0);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "an empty container throws rather than returning end()");

  // 0 is the value a bugged implementation is most likely to invent.
  threw = false;
  try {
    easyfind(empty, 42);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "and throws for any value, not just zero");

  std::vector<int> one(1, 5);
  Check::same(easyfind(one, 5), one.begin(), "one-element hit");
  threw = false;
  try {
    easyfind(one, 6);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "one-element miss");

  std::list<int> emptyList;
  threw = false;
  try {
    easyfind(emptyList, 1);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "empty list throws as well");
}

static void test_container_untouched() {
  Check::section("easyfind reads, it never writes");

  std::vector<int> v;
  fill(v, kData, kCount);
  const std::vector<int> before(v);

  easyfind(v, 84);
  Check::ok(v == before, "a hit leaves the container identical");

  try {
    easyfind(v, 999);
  } catch (const std::out_of_range&) {
  }
  Check::ok(v == before, "so does a miss");
  Check::eq(v.size(), before.size(), "size unchanged");
}

static void test_iterator_is_a_handle() {
  Check::section("the caller can write, erase and insert through the result");

  std::vector<int> v;
  fill(v, kData, kCount);

  *easyfind(v, 84) = -84;
  Check::eq(v[2], -84, "assignment through the iterator lands in the vector");

  v.erase(easyfind(v, -84));
  Check::eq(v.size(), static_cast<std::size_t>(kCount - 1), "erase shortened it");
  bool threw = false;
  try {
    easyfind(v, -84);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "and the erased value is really gone");

  v.insert(easyfind(v, 7), 99);
  Check::eq(std::distance(v.begin(), easyfind(v, 99)), 3,
            "insert placed 99 right before 7");

  // On a list the same three operations do not invalidate the neighbours,
  // which is what makes a purge loop safe there.
  std::list<int> l;
  fill(l, kData, kCount);
  l.erase(easyfind(l, 21));
  l.erase(easyfind(l, 21));
  Check::eq(l.size(), static_cast<std::size_t>(3), "both 21s purged one by one");
  threw = false;
  try {
    easyfind(l, 21);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "the third lookup is the loop's exit condition");
}

static void test_const_overload() {
  Check::section("the const overload: const in, const_iterator out");

  std::vector<int> v;
  fill(v, kData, kCount);

  const std::vector<int>& ref = v;
  std::vector<int>::const_iterator it = easyfind(ref, 84);
  Check::eq(*it, 84, "found through a const reference");
  Check::eq(std::distance(ref.begin(), it), 2, "at the same position");

  const std::vector<int> object(v);
  Check::eq(*easyfind(object, 40), 40, "found on a const object");

  // The const overload must throw on a miss too. A mutant that only removes
  // the check from one of the two overloads is caught right here.
  bool threw = false;
  try {
    easyfind(ref, 999);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "const miss throws");

  const std::vector<int> emptyConst;
  threw = false;
  try {
    easyfind(emptyConst, 0);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "empty const container throws");

  const std::list<int> constList(v.begin(), v.end());
  Check::eq(*easyfind(constList, 7), 7, "const list");
}

static void test_every_container() {
  Check::section("every sequence container, plus a set");

  std::vector<int> v;
  std::list<int> l;
  std::deque<int> d;
  fill(v, kData, kCount);
  fill(l, kData, kCount);
  fill(d, kData, kCount);
  std::set<int> s(v.begin(), v.end());

  Check::eq(*easyfind(v, 84), 84, "vector");
  Check::eq(*easyfind(l, 84), 84, "list");
  Check::eq(*easyfind(d, 84), 84, "deque");
  Check::eq(*easyfind(s, 84), 84, "set");
  Check::same(easyfind(s, 84), s.find(84),
              "the set's own find agrees with the generic one");

  // deque's iterator is not a pointer and not a node link; it is the third
  // shape in the standard library, so it is worth its own miss.
  bool threw = false;
  try {
    easyfind(d, 999);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "deque miss throws");
}

static void test_value_edges() {
  Check::section("value edges: zero, negatives, INT_MIN, INT_MAX");

  const int kMin = std::numeric_limits<int>::min();
  const int kMax = std::numeric_limits<int>::max();

  std::vector<int> v;
  v.push_back(kMin);
  v.push_back(-1);
  v.push_back(0);
  v.push_back(kMax);

  Check::eq(*easyfind(v, kMin), kMin, "INT_MIN is a value like any other");
  Check::eq(*easyfind(v, kMax), kMax, "INT_MAX too");
  Check::eq(*easyfind(v, 0), 0, "zero is not a sentinel here");
  Check::eq(std::distance(v.begin(), easyfind(v, -1)), 1, "negative value");

  bool threw = false;
  try {
    easyfind(v, kMin + 1);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "a near miss is still a miss");
}

static void test_agrees_with_std_find() {
  Check::section("differential: identical to std::find on every index");

  std::vector<int> v;
  for (int i = 0; i < 200; ++i) v.push_back((i * 37) % 50);

  bool allMatch = true;
  for (int needle = -5; needle < 55; ++needle) {
    std::vector<int>::iterator reference =
        std::find(v.begin(), v.end(), needle);
    if (reference == v.end()) {
      bool threw = false;
      try {
        easyfind(v, needle);
      } catch (const std::out_of_range&) {
        threw = true;
      }
      if (!threw) allMatch = false;
    } else if (easyfind(v, needle) != reference) {
      allMatch = false;
    }
  }
  Check::ok(allMatch,
            "60 needles, hits and misses, same answer as the library");
}

static void test_scale() {
  Check::section("scale: 200k elements, hit at both ends");

  std::vector<int> big;
  big.reserve(200000);
  for (int i = 0; i < 200000; ++i) big.push_back(i);

  Check::eq(*easyfind(big, 0), 0, "first element");
  Check::eq(*easyfind(big, 199999), 199999, "last element");
  Check::eq(std::distance(big.begin(), easyfind(big, 123456)), 123456,
            "an element in the middle");

  bool threw = false;
  try {
    easyfind(big, -1);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  Check::ok(threw, "a full scan that finds nothing still throws");
}

static void test_nested_containers() {
  Check::section("nested containers: the template does not care");

  std::vector<std::vector<int> > matrix;
  for (int row = 0; row < 3; ++row) {
    std::vector<int> line;
    for (int col = 0; col < 4; ++col) line.push_back(row * 10 + col);
    matrix.push_back(line);
  }

  // easyfind on the inner container, reached through the outer one.
  Check::eq(*easyfind(matrix[1], 12), 12, "found inside row 1");
  Check::eq(std::distance(matrix[1].begin(), easyfind(matrix[1], 12)), 2,
            "at the right column");

  int found = 0;
  for (std::size_t row = 0; row < matrix.size(); ++row) {
    try {
      easyfind(matrix[row], 21);
      ++found;
    } catch (const std::out_of_range&) {
    }
  }
  Check::eq(found, 1, "21 lives in exactly one row");
}

static void test_element_type_only_needs_equality() {
  Check::section("the element type only has to compare with an int");

  std::vector<Probe> probes;
  for (int i = 0; i < 4; ++i) probes.push_back(Probe(i + 1));

  Probe::comparisons = 0;
  Check::eq(easyfind(probes, 3)->id, 3, "a struct with operator==(int) works");

  const std::vector<Probe>& constProbes = probes;
  Check::eq(easyfind(constProbes, 1)->id, 1, "and through the const overload");

  Check::ok(Probe::comparisons > 0,
            "the comparisons really went through Probe::operator==");
}

int main() {
  test_contract();
  test_first_occurrence();
  test_search_shape();
  test_miss_throws();
  test_empty_and_tiny();
  test_container_untouched();
  test_iterator_is_a_handle();
  test_const_overload();
  test_every_container();
  test_value_edges();
  test_agrees_with_std_find();
  test_scale();
  test_nested_containers();
  test_element_type_only_needs_equality();

  return Check::report("ex00 easyfind - fixed cases");
}
