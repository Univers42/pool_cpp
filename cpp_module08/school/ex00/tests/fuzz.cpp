/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzz.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Randomised half of the ex00 suite. tests/test.cpp pins the cases I thought
// of; this file goes looking for the ones I did not.
//
//   ./build/bin/fuzz            30000 rounds, seed 42
//   ./build/bin/fuzz 1234       another seed
//   ./build/bin/fuzz 1234 500   another seed, fewer rounds
//
// The oracle is a hand-written linear scan — deliberately NOT std::find, so
// the comparison is against an independent implementation rather than against
// the same library call spelled twice. Every round asks four containers the
// same question and requires the same answer from all of them, which is the
// property the exercise actually claims: the container should not matter.

#include <cstdlib>
#include <deque>
#include <limits>
#include <list>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../easyfind.hpp"
#include "check.hpp"

// The oracle: index of the first match, or -1. No <algorithm> on purpose.
template <typename T>
static long oracleIndex(const T& container, int value) {
  long index = 0;
  for (typename T::const_iterator it = container.begin();
       it != container.end(); ++it, ++index) {
    if (*it == value) return index;
  }
  return -1;
}

// Runs easyfind through the mutable overload and reports what it did in the
// oracle's vocabulary: an index, or -1 for "threw std::out_of_range".
template <typename T>
static long observe(T& container, int value) {
  try {
    return static_cast<long>(
        std::distance(container.begin(), easyfind(container, value)));
  } catch (const std::out_of_range&) {
    return -1;
  }
}

// Same, through the const overload. The two must never disagree.
template <typename T>
static long observeConst(const T& container, int value) {
  try {
    return static_cast<long>(
        std::distance(container.begin(), easyfind(container, value)));
  } catch (const std::out_of_range&) {
    return -1;
  }
}

static std::string describe(const std::vector<int>& data, int needle,
                           long expected, long got, const char* which) {
  std::ostringstream out;
  out << which << ": needle " << needle << " expected " << expected << " got "
      << got << " in [";
  for (std::size_t i = 0; i < data.size() && i < 24; ++i) {
    if (i) out << ", ";
    out << data[i];
  }
  if (data.size() > 24) out << ", ...";
  out << "] (" << data.size() << " elements)";
  return out.str();
}

// ─────────────────────────────────────────────────────────────────────────────
// Small values, short containers: the regime that produces duplicates, empty
// containers and needles that are just barely absent.
static void fuzz_dense(Rng& rng, int rounds) {
  Check::section("dense: short containers, small alphabet, many duplicates");

  int mismatches = 0;
  int hits = 0;
  int misses = 0;
  int emptyRounds = 0;

  for (int round = 0; round < rounds; ++round) {
    const int size = rng.between(0, 12);
    std::vector<int> data;
    for (int i = 0; i < size; ++i) data.push_back(rng.between(-3, 6));
    if (size == 0) ++emptyRounds;

    // Half the needles are drawn from the data so hits stay frequent even as
    // the alphabet grows; the rest probe the gaps.
    int needle;
    if (size > 0 && rng.chance(2))
      needle = data[static_cast<std::size_t>(rng.between(0, size - 1))];
    else
      needle = rng.between(-6, 9);

    const long expected = oracleIndex(data, needle);
    expected < 0 ? ++misses : ++hits;

    std::list<int> asList(data.begin(), data.end());
    std::deque<int> asDeque(data.begin(), data.end());
    const std::vector<int>& asConst = data;

    const long fromVector = observe(data, needle);
    const long fromList = observe(asList, needle);
    const long fromDeque = observe(asDeque, needle);
    const long fromConst = observeConst(asConst, needle);

    if (fromVector != expected) {
      Check::ok(false, describe(data, needle, expected, fromVector, "vector"));
      if (++mismatches > 4) return;
    }
    if (fromList != expected) {
      Check::ok(false, describe(data, needle, expected, fromList, "list"));
      if (++mismatches > 4) return;
    }
    if (fromDeque != expected) {
      Check::ok(false, describe(data, needle, expected, fromDeque, "deque"));
      if (++mismatches > 4) return;
    }
    if (fromConst != expected) {
      Check::ok(false,
                describe(data, needle, expected, fromConst, "const vector"));
      if (++mismatches > 4) return;
    }
  }

  Check::eq(mismatches, 0, "vector, list, deque and const all match the oracle");
  Check::ok(hits > rounds / 8, "the run actually found things");
  Check::ok(misses > rounds / 8, "and actually missed things");
  Check::ok(emptyRounds > 0, "empty containers were covered");
}

// Full 32-bit needles, including the extremes that a signed comparison bug
// would trip over.
static void fuzz_wide(Rng& rng, int rounds) {
  Check::section("wide: full int range, INT_MIN and INT_MAX seeded in");

  const int kMin = std::numeric_limits<int>::min();
  const int kMax = std::numeric_limits<int>::max();
  int mismatches = 0;

  for (int round = 0; round < rounds; ++round) {
    const int size = rng.between(1, 20);
    std::vector<int> data;
    for (int i = 0; i < size; ++i) {
      if (rng.chance(7))
        data.push_back(rng.chance(2) ? kMin : kMax);
      else if (rng.chance(5))
        data.push_back(0);
      else
        data.push_back(static_cast<int>(rng.next()));
    }

    int needle;
    switch (rng.between(0, 3)) {
      case 0:
        needle = kMin;
        break;
      case 1:
        needle = kMax;
        break;
      case 2:
        needle = data[static_cast<std::size_t>(rng.between(0, size - 1))];
        break;
      default:
        needle = static_cast<int>(rng.next());
        break;
    }

    const long expected = oracleIndex(data, needle);
    if (observe(data, needle) != expected) {
      Check::ok(false, describe(data, needle, expected, observe(data, needle),
                                "wide vector"));
      if (++mismatches > 4) return;
    }
  }
  Check::eq(mismatches, 0, "extremes behave like any other value");
}

// A set reorders and de-duplicates its input, so the oracle has to walk the
// set itself. If easyfind ever assumed contiguity or insertion order, this is
// where it breaks.
static void fuzz_set(Rng& rng, int rounds) {
  Check::section("set: the container decides the order, not the caller");

  int mismatches = 0;
  int agreements = 0;

  for (int round = 0; round < rounds; ++round) {
    std::set<int> s;
    const int size = rng.between(0, 10);
    for (int i = 0; i < size; ++i) s.insert(rng.between(-8, 8));

    const int needle = rng.between(-10, 10);
    const long expected = oracleIndex(s, needle);
    const long got = observe(s, needle);

    if (got != expected) {
      std::vector<int> flat(s.begin(), s.end());
      Check::ok(false, describe(flat, needle, expected, got, "set"));
      if (++mismatches > 4) return;
    }

    // And the set's own O(log n) find must land on the same element.
    if (expected >= 0) {
      if (easyfind(s, needle) == s.find(needle)) {
        ++agreements;
      } else {
        Check::ok(false, "set::find and easyfind disagree on the position");
        if (++mismatches > 4) return;
      }
    }
  }

  Check::eq(mismatches, 0, "generic search matches the tree's own order");
  Check::ok(agreements > 0, "and matched set::find on every hit");
}

// The container must come out of a fuzz round byte-identical to how it went
// in, hit or miss. easyfind takes T& — the compiler will not stop it writing.
static void fuzz_read_only(Rng& rng, int rounds) {
  Check::section("read-only: no round ever modified its container");

  int damaged = 0;
  for (int round = 0; round < rounds; ++round) {
    std::vector<int> data;
    const int size = rng.between(0, 16);
    for (int i = 0; i < size; ++i) data.push_back(rng.between(-4, 4));
    const std::vector<int> before(data);

    observe(data, rng.between(-6, 6));
    if (data != before && ++damaged > 4) break;
  }
  Check::eq(damaged, 0, "contents identical before and after every lookup");
}

int main(int argc, char** argv) {
  unsigned int seed = 42;
  int rounds = 30000;
  if (argc > 1) seed = static_cast<unsigned int>(std::atoi(argv[1]));
  if (argc > 2) rounds = std::atoi(argv[2]);
  if (rounds < 1) rounds = 1;

  Rng rng(seed);
  fuzz_dense(rng, rounds);
  fuzz_wide(rng, rounds / 3 + 1);
  fuzz_set(rng, rounds / 3 + 1);
  fuzz_read_only(rng, rounds / 3 + 1);

  std::ostringstream title;
  title << "ex00 easyfind - fuzz (seed " << seed << ", " << rounds
        << " dense rounds)";
  return Check::report(title.str().c_str());
}
