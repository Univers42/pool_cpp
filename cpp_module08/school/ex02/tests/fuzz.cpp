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

// Randomised half of the ex02 suite.
//
//   ./build/bin/fuzz            2000 rounds, seed 42
//   ./build/bin/fuzz 1234       another seed
//   ./build/bin/fuzz 1234 100   another seed, fewer rounds
//
// The oracle is a std::deque driven through the same random script: push_back
// for push, pop_back for pop, back() for top. That is the subject's own
// acceptance criterion generalised — it says a MutantStack must be
// indistinguishable from a plain sequence container, so the test generates
// thousands of scripts and demands exactly that, forwards and in reverse,
// after every single operation.
//
// The same script runs against all three sensible backings, because the
// iterator typedefs come from Container and a bug that only shows up on a
// list-backed stack is a bug nobody would find by hand.

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include "../MutantStack.hpp"
#include "check.hpp"

static std::string dump(const std::deque<int>& model) {
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < model.size() && i < 20; ++i) {
    if (i) out << ", ";
    out << model[i];
  }
  if (model.size() > 20) out << ", ...";
  out << "] (" << model.size() << ")";
  return out.str();
}

// One randomised script against one backing. Returns the number of times the
// stack and the model disagreed.
template <typename Stack>
static int replay(Rng& rng, int steps, const char* backing) {
  Stack ms;
  std::deque<int> model;
  int divergences = 0;

  for (int step = 0; step < steps && divergences <= 3; ++step) {
    const int action = rng.between(0, 9);

    if (action <= 4) {
      // push, the most common operation so the stack actually grows
      const int value = rng.between(-1000, 1000);
      ms.push(value);
      model.push_back(value);
    } else if (action <= 7) {
      // pop, but only when there is something to pop: popping an empty
      // std::stack is undefined behaviour, not an exception
      if (!model.empty()) {
        ms.pop();
        model.pop_back();
      }
    } else if (action == 8) {
      // write through a random position, which only MutantStack can do
      if (!model.empty()) {
        const std::size_t index =
            static_cast<std::size_t>(rng.between(0, static_cast<int>(model.size()) - 1));
        typename Stack::iterator it = ms.begin();
        std::advance(it, static_cast<std::ptrdiff_t>(index));
        const int value = rng.between(-1000, 1000);
        *it = value;
        model[index] = value;
      }
    } else {
      // copy, mutate the copy, and require the original to be unmoved
      Stack copy(ms);
      copy.push(12345);
      if (copy.size() != ms.size() + 1) {
        Check::ok(false, std::string(backing) + ": a copy shared its size");
        ++divergences;
      }
      if (ms.size() != model.size()) {
        Check::ok(false, std::string(backing) + ": the original grew with the copy");
        ++divergences;
      }
    }

    // After every step: same count, same top, same contents both ways.
    if (ms.size() != model.size()) {
      std::ostringstream why;
      why << backing << ": size " << ms.size() << " != " << model.size();
      Check::ok(false, why.str());
      ++divergences;
      continue;
    }
    if (ms.empty() != model.empty()) {
      Check::ok(false, std::string(backing) + ": empty() disagreed");
      ++divergences;
      continue;
    }
    if (!model.empty() && ms.top() != model.back()) {
      std::ostringstream why;
      why << backing << ": top " << ms.top() << " != back " << model.back();
      Check::ok(false, why.str());
      ++divergences;
      continue;
    }
    if (!std::equal(ms.begin(), ms.end(), model.begin())) {
      Check::ok(false, std::string(backing) + ": forward order diverged from " +
                           dump(model));
      ++divergences;
      continue;
    }
    if (!std::equal(ms.rbegin(), ms.rend(), model.rbegin())) {
      Check::ok(false, std::string(backing) + ": reverse order diverged from " +
                           dump(model));
      ++divergences;
      continue;
    }
    if (std::distance(ms.begin(), ms.end()) !=
        static_cast<std::ptrdiff_t>(model.size())) {
      Check::ok(false, std::string(backing) + ": range length != size()");
      ++divergences;
    }
  }
  return divergences;
}

static void fuzz_backings(Rng& rng, int rounds) {
  Check::section("random scripts: three backings against a std::deque shadow");

  int total = 0;
  for (int round = 0; round < rounds; ++round) {
    const int steps = rng.between(1, 40);
    total += replay<MutantStack<int> >(rng, steps, "deque");
    total += replay<MutantStack<int, std::vector<int> > >(rng, steps, "vector");
    total += replay<MutantStack<int, std::list<int> > >(rng, steps, "list");
    if (total > 3) break;
  }
  Check::eq(total, 0, "no divergence in any script, on any container");
}

// The subject's criterion, generated: build the same random sequence into a
// MutantStack and a std::list and require identical readings.
static void fuzz_matches_a_list(Rng& rng, int rounds) {
  Check::section("the subject's criterion, generated instead of eyeballed");

  int wrong = 0;
  for (int round = 0; round < rounds && wrong <= 3; ++round) {
    MutantStack<int> ms;
    std::list<int> lst;

    const int steps = rng.between(1, 30);
    for (int step = 0; step < steps; ++step) {
      if (rng.chance(3) && !lst.empty()) {
        ms.pop();
        lst.pop_back();
      } else {
        const int value = rng.between(-50, 50);
        ms.push(value);
        lst.push_back(value);
      }
    }

    if (ms.size() != lst.size()) {
      Check::ok(false, "size differs from the std::list");
      ++wrong;
      continue;
    }
    if (!lst.empty() && ms.top() != lst.back()) {
      Check::ok(false, "top() differs from list::back()");
      ++wrong;
      continue;
    }
    const std::vector<int> fromStack(ms.begin(), ms.end());
    const std::vector<int> fromList(lst.begin(), lst.end());
    if (fromStack != fromList) {
      Check::ok(false, "iteration differs from the std::list");
      ++wrong;
      continue;
    }
    const std::vector<int> revStack(ms.rbegin(), ms.rend());
    const std::vector<int> revList(lst.rbegin(), lst.rend());
    if (revStack != revList) {
      Check::ok(false, "reverse iteration differs from the std::list");
      ++wrong;
    }
  }
  Check::eq(wrong, 0, "every generated script read identically through both");
}

// Slicing to the base must preserve every value, every time.
static void fuzz_slicing(Rng& rng, int rounds) {
  Check::section("slicing to std::stack keeps the contents");

  int wrong = 0;
  for (int round = 0; round < rounds && wrong <= 3; ++round) {
    MutantStack<int> ms;
    std::deque<int> model;
    const int steps = rng.between(0, 25);
    for (int step = 0; step < steps; ++step) {
      const int value = rng.between(-100, 100);
      ms.push(value);
      model.push_back(value);
    }

    std::stack<int> sliced(ms);
    if (sliced.size() != model.size()) {
      Check::ok(false, "the slice lost elements");
      ++wrong;
      continue;
    }
    // Drain it and compare against the model, newest first.
    bool ok = true;
    for (std::deque<int>::reverse_iterator it = model.rbegin();
         it != model.rend(); ++it) {
      if (sliced.top() != *it) {
        ok = false;
        break;
      }
      sliced.pop();
    }
    if (!ok) {
      Check::ok(false, "the slice's order diverged from " + dump(model));
      ++wrong;
      continue;
    }
    if (ms.size() != model.size()) {
      Check::ok(false, "draining the slice consumed the original");
      ++wrong;
    }
  }
  Check::eq(wrong, 0, "every slice was a faithful, independent copy");
}

// Assignment is the member most likely to be written wrong, and the one the
// compiler would have generated correctly. Both must behave the same.
static void fuzz_assignment(Rng& rng, int rounds) {
  Check::section("assignment replaces, and leaves both sides independent");

  int wrong = 0;
  for (int round = 0; round < rounds && wrong <= 3; ++round) {
    MutantStack<int> source;
    MutantStack<int> target;
    std::deque<int> model;

    const int sourceSteps = rng.between(0, 20);
    for (int i = 0; i < sourceSteps; ++i) {
      const int value = rng.between(-100, 100);
      source.push(value);
      model.push_back(value);
    }
    const int targetSteps = rng.between(0, 20);
    for (int i = 0; i < targetSteps; ++i) target.push(rng.between(-100, 100));

    target = source;

    if (target.size() != model.size() ||
        !std::equal(target.begin(), target.end(), model.begin())) {
      Check::ok(false, "assignment did not replace the target's contents");
      ++wrong;
      continue;
    }

    target.push(777);
    if (source.size() != model.size()) {
      Check::ok(false, "the source followed the target after assignment");
      ++wrong;
      continue;
    }

    // Self-assignment through an alias must be a no-op, not a wipe.
    MutantStack<int>& alias = source;
    source = alias;
    if (source.size() != model.size() ||
        !std::equal(source.begin(), source.end(), model.begin())) {
      Check::ok(false, "self-assignment damaged the stack");
      ++wrong;
    }
  }
  Check::eq(wrong, 0, "assignment behaved on every generated pair");
}

int main(int argc, char** argv) {
  unsigned int seed = 42;
  int rounds = 2000;
  if (argc > 1) seed = static_cast<unsigned int>(std::atoi(argv[1]));
  if (argc > 2) rounds = std::atoi(argv[2]);
  if (rounds < 1) rounds = 1;

  Rng rng(seed);
  fuzz_backings(rng, rounds / 4 + 1);
  fuzz_matches_a_list(rng, rounds);
  fuzz_slicing(rng, rounds / 2 + 1);
  fuzz_assignment(rng, rounds / 2 + 1);

  std::ostringstream title;
  title << "ex02 MutantStack - fuzz (seed " << seed << ", " << rounds
        << " rounds)";
  return Check::report(title.str().c_str());
}
