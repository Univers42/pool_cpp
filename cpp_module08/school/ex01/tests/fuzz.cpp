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

// Randomised half of the ex01 suite.
//
//   ./build/bin/fuzz            4000 rounds, seed 42
//   ./build/bin/fuzz 1234       another seed
//   ./build/bin/fuzz 1234 200   another seed, fewer rounds
//
// Two oracles, both deliberately dumber than Span:
//   * the O(n^2) definition of "shortest/longest distance between any two
//     stored numbers", straight from the subject's wording. Span's sort-then-
//     fold is an optimisation of exactly this, and an optimisation is only
//     worth having if it can be shown to agree with the thing it replaced.
//   * a std::vector shadow model, replayed through the same random sequence of
//     operations, which catches state bugs a query-only test never would:
//     capacity accounting drifting after a rejected insert, order lost, or a
//     copy quietly sharing storage.

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include "../Span.hpp"
#include "check.hpp"

static const unsigned int kUintMax = std::numeric_limits<unsigned int>::max();

// ── the O(n^2) oracle ────────────────────────────────────────────────────────
static unsigned int gap(int a, int b) {
  const unsigned int ua = static_cast<unsigned int>(a);
  const unsigned int ub = static_cast<unsigned int>(b);
  return a < b ? ub - ua : ua - ub;
}

static unsigned int bruteShortest(const std::vector<int>& v) {
  unsigned int best = kUintMax;
  for (std::size_t i = 0; i < v.size(); ++i)
    for (std::size_t j = i + 1; j < v.size(); ++j)
      if (gap(v[i], v[j]) < best) best = gap(v[i], v[j]);
  return best;
}

static unsigned int bruteLongest(const std::vector<int>& v) {
  unsigned int best = 0;
  for (std::size_t i = 0; i < v.size(); ++i)
    for (std::size_t j = i + 1; j < v.size(); ++j)
      if (gap(v[i], v[j]) > best) best = gap(v[i], v[j]);
  return best;
}

static std::string dump(const std::vector<int>& v) {
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < v.size() && i < 20; ++i) {
    if (i) out << ", ";
    out << v[i];
  }
  if (v.size() > 20) out << ", ...";
  out << "] (" << v.size() << ")";
  return out.str();
}

// Draws a value from one of several regimes, so a run covers dense clusters,
// full-range noise and the signed extremes rather than only the middle.
static int draw(Rng& rng, int regime) {
  switch (regime) {
    case 0:
      return rng.between(-5, 5);            // heavy duplicates, tiny gaps
    case 1:
      return rng.between(-1000, 1000);      // ordinary
    case 2:
      return static_cast<int>(rng.next());  // full 32-bit
    default:
      if (rng.chance(3)) return std::numeric_limits<int>::min();
      if (rng.chance(3)) return std::numeric_limits<int>::max();
      return static_cast<int>(rng.next());  // extremes mixed in
  }
}

// ─────────────────────────────────────────────────────────────────────────────
static void fuzz_queries(Rng& rng, int rounds) {
  Check::section("queries agree with the O(n^2) definition");

  int mismatches = 0;
  int zeroSpans = 0;
  int hugeSpans = 0;

  for (int round = 0; round < rounds; ++round) {
    const int regime = rng.between(0, 3);
    const unsigned int count =
        static_cast<unsigned int>(rng.between(2, 60));

    std::vector<int> values;
    for (unsigned int i = 0; i < count; ++i) values.push_back(draw(rng, regime));

    Span sp(count);
    sp.addNumber(values.begin(), values.end());

    const unsigned int wantShort = bruteShortest(values);
    const unsigned int wantLong = bruteLongest(values);
    if (wantShort == 0) ++zeroSpans;
    if (wantLong > 2000000000u) ++hugeSpans;

    if (sp.shortestSpan() != wantShort) {
      std::ostringstream why;
      why << "shortestSpan " << sp.shortestSpan() << " != " << wantShort
          << " for " << dump(values);
      Check::ok(false, why.str());
      if (++mismatches > 4) return;
    }
    if (sp.longestSpan() != wantLong) {
      std::ostringstream why;
      why << "longestSpan " << sp.longestSpan() << " != " << wantLong
          << " for " << dump(values);
      Check::ok(false, why.str());
      if (++mismatches > 4) return;
    }
  }

  Check::eq(mismatches, 0, "every round matched brute force");
  Check::ok(zeroSpans > 0, "duplicate values (span 0) were exercised");
  Check::ok(hugeSpans > 0, "spans past INT_MAX were exercised");
}

// A random sequence of operations against a std::vector shadow. Span and the
// model must agree on size, on contents, and on which calls threw.
static void fuzz_state_machine(Rng& rng, int rounds) {
  Check::section("state machine: Span tracks a std::vector shadow exactly");

  int divergences = 0;
  int rejectedSingles = 0;
  int rejectedRanges = 0;
  int acceptedRanges = 0;

  for (int round = 0; round < rounds; ++round) {
    const unsigned int capacity =
        static_cast<unsigned int>(rng.between(0, 24));
    Span sp(capacity);
    std::vector<int> model;

    const int steps = rng.between(1, 30);
    for (int step = 0; step < steps; ++step) {
      if (rng.chance(3)) {
        // Range add, sometimes deliberately too large.
        const int length = rng.between(0, 8);
        std::vector<int> chunk;
        for (int i = 0; i < length; ++i) chunk.push_back(draw(rng, 1));

        const bool fits = model.size() + chunk.size() <= capacity;
        bool threw = false;
        try {
          sp.addNumber(chunk.begin(), chunk.end());
        } catch (const Span::SpanFullException&) {
          threw = true;
        }

        if (threw == fits) {
          std::ostringstream why;
          why << "range of " << chunk.size() << " into " << model.size() << "/"
              << capacity << (threw ? " threw" : " was accepted")
              << " but should not have been";
          Check::ok(false, why.str());
          if (++divergences > 4) return;
        }
        if (fits) {
          model.insert(model.end(), chunk.begin(), chunk.end());
          ++acceptedRanges;
        } else {
          ++rejectedRanges;
        }
      } else {
        // Single add.
        const int value = draw(rng, 1);
        const bool fits = model.size() < capacity;
        bool threw = false;
        try {
          sp.addNumber(value);
        } catch (const Span::SpanFullException&) {
          threw = true;
        }
        if (threw == fits) {
          Check::ok(false, "addNumber(int) disagreed with the model on room");
          if (++divergences > 4) return;
        }
        if (fits)
          model.push_back(value);
        else
          ++rejectedSingles;
      }

      // After every step: same count, same values, same order.
      if (sp.size() != model.size()) {
        std::ostringstream why;
        why << "size " << sp.size() << " != model " << model.size();
        Check::ok(false, why.str());
        if (++divergences > 4) return;
      }
      if (!std::equal(sp.begin(), sp.end(), model.begin())) {
        Check::ok(false, "contents diverged from the model: " + dump(model));
        if (++divergences > 4) return;
      }
      if (sp.full() != (model.size() >= capacity)) {
        Check::ok(false, "full() disagreed with the model");
        if (++divergences > 4) return;
      }
    }

    // And the queries still answer correctly at the end of the sequence.
    if (model.size() >= 2) {
      if (sp.shortestSpan() != bruteShortest(model) ||
          sp.longestSpan() != bruteLongest(model)) {
        Check::ok(false, "queries wrong after a random op sequence: " +
                             dump(model));
        if (++divergences > 4) return;
      }
    } else {
      bool threw = false;
      try {
        sp.shortestSpan();
      } catch (const Span::NotEnoughElementsException&) {
        threw = true;
      }
      if (!threw) {
        Check::ok(false, "a Span with fewer than 2 values answered anyway");
        if (++divergences > 4) return;
      }
    }
  }

  Check::eq(divergences, 0, "Span and the model never disagreed");
  Check::ok(rejectedSingles > 0, "single adds were rejected at capacity");
  Check::ok(rejectedRanges > 0, "oversized ranges were rejected");
  Check::ok(acceptedRanges > 0, "and ranges that fit were accepted");
}

// The strong guarantee: a rejected range must leave the Span byte-identical.
static void fuzz_atomicity(Rng& rng, int rounds) {
  Check::section("atomicity: a rejected range changes nothing");

  int damaged = 0;
  int rejections = 0;

  for (int round = 0; round < rounds; ++round) {
    const unsigned int capacity =
        static_cast<unsigned int>(rng.between(1, 20));
    Span sp(capacity);

    std::vector<int> seeded;
    const int prefill = rng.between(0, static_cast<int>(capacity));
    for (int i = 0; i < prefill; ++i) seeded.push_back(draw(rng, 1));
    sp.addNumber(seeded.begin(), seeded.end());

    // A range guaranteed not to fit.
    const int overshoot =
        static_cast<int>(capacity) - prefill + rng.between(1, 6);
    std::vector<int> chunk;
    for (int i = 0; i < overshoot; ++i) chunk.push_back(draw(rng, 1));

    bool threw = false;
    try {
      sp.addNumber(chunk.begin(), chunk.end());
    } catch (const Span::SpanFullException&) {
      threw = true;
      ++rejections;
    }

    if (!threw) {
      Check::ok(false, "an oversized range was accepted");
      if (++damaged > 4) return;
      continue;
    }
    if (sp.size() != seeded.size() ||
        !std::equal(sp.begin(), sp.end(), seeded.begin())) {
      Check::ok(false, "the Span changed despite the throw: " + dump(seeded));
      if (++damaged > 4) return;
    }
  }

  Check::eq(damaged, 0, "every rejected insert left the Span untouched");
  Check::ok(rejections > 0, "and rejections really happened");
}

// Copies must be independent: mutating one may never be visible in the other.
static void fuzz_canonical_form(Rng& rng, int rounds) {
  Check::section("canonical form: copies never share storage");

  int shared = 0;
  for (int round = 0; round < rounds; ++round) {
    const unsigned int capacity =
        static_cast<unsigned int>(rng.between(2, 20));
    Span original(capacity);
    const int prefill = rng.between(0, static_cast<int>(capacity) - 1);
    for (int i = 0; i < prefill; ++i) original.addNumber(draw(rng, 1));

    const std::vector<int> before(original.begin(), original.end());

    Span viaCtor(original);
    Span viaAssign;
    viaAssign = original;

    // Mutate each copy and require the original to be unmoved.
    viaCtor.addNumber(draw(rng, 1));
    viaAssign.addNumber(draw(rng, 1));

    const std::vector<int> after(original.begin(), original.end());
    if (before != after) {
      Check::ok(false, "the original changed when a copy did");
      if (++shared > 4) return;
    }
    if (viaCtor.maxSize() != capacity || viaAssign.maxSize() != capacity) {
      Check::ok(false, "a copy did not inherit N");
      if (++shared > 4) return;
    }
    if (viaCtor.size() != before.size() + 1 ||
        viaAssign.size() != before.size() + 1) {
      Check::ok(false, "a copy did not receive the values");
      if (++shared > 4) return;
    }
  }
  Check::eq(shared, 0, "no copy ever shared state with its source");
}

// A handful of large runs where the answer is constructed, not brute-forced:
// values spaced k apart give shortest == k and longest == k * (n - 1).
static void fuzz_scale(Rng& rng, int rounds) {
  Check::section("scale: constructed answers on 10k-plus element Spans");

  int wrong = 0;
  for (int round = 0; round < rounds; ++round) {
    const int step = rng.between(1, 50);
    const unsigned int count =
        static_cast<unsigned int>(rng.between(10000, 40000));
    const int origin = rng.between(-100000, 100000);

    std::vector<int> values;
    values.reserve(count);
    for (unsigned int i = 0; i < count; ++i)
      values.push_back(origin + static_cast<int>(i) * step);

    // Shuffle deterministically so the input is not already sorted.
    for (std::size_t i = values.size(); i > 1; --i) {
      const std::size_t j =
          static_cast<std::size_t>(rng.between(0, static_cast<int>(i) - 1));
      std::swap(values[i - 1], values[j]);
    }

    Span sp(count);
    sp.addNumber(values.begin(), values.end());

    const unsigned int wantShort = static_cast<unsigned int>(step);
    const unsigned int wantLong =
        static_cast<unsigned int>(step) * (count - 1);
    if (sp.shortestSpan() != wantShort || sp.longestSpan() != wantLong) {
      std::ostringstream why;
      why << count << " values spaced " << step << ": got "
          << sp.shortestSpan() << "/" << sp.longestSpan() << ", want "
          << wantShort << "/" << wantLong;
      Check::ok(false, why.str());
      if (++wrong > 2) return;
    }
  }
  Check::eq(wrong, 0, "arithmetic progressions answer exactly, shuffled or not");
}

int main(int argc, char** argv) {
  unsigned int seed = 42;
  int rounds = 4000;
  if (argc > 1) seed = static_cast<unsigned int>(std::atoi(argv[1]));
  if (argc > 2) rounds = std::atoi(argv[2]);
  if (rounds < 1) rounds = 1;

  Rng rng(seed);
  fuzz_queries(rng, rounds);
  fuzz_state_machine(rng, rounds / 4 + 1);
  fuzz_atomicity(rng, rounds / 2 + 1);
  fuzz_canonical_form(rng, rounds / 2 + 1);
  fuzz_scale(rng, rounds / 500 + 2);

  std::ostringstream title;
  title << "ex01 Span - fuzz (seed " << seed << ", " << rounds
        << " query rounds)";
  return Check::report(title.str().c_str());
}
