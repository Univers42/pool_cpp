/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:14:46 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Walkthrough of ex00, told through one running scenario: the badge registry
// of a building's access-control system. Every section answers a question a
// real caller would ask, and the last one hands the scenario to ex01.
//
//   make run                 this walkthrough
//   make test                the assertions behind it, then every check below
//   ./tests/test.cpp         the fixed cases
//   ./tests/fuzz.cpp         random containers against an independent oracle
//   ./tests/eval.sh          the 42 evaluation checklist, mechanised
//   ./tests/mutants.sh       breaks easyfind.hpp on purpose, tests must fail
//   ./tests/compile_fail.sh  the code that must NOT compile
//   valgrind ./build/bin/ex00

#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <set>
#include <string>
#include <vector>

#include "easyfind.hpp"

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

  // Any container of ints, printed the same way — which is the whole lesson of
  // this exercise applied to output instead of search.
  template <typename T>
  static std::string join(const T& container) {
    std::string out("{");
    for (typename T::const_iterator it = container.begin();
         it != container.end(); ++it) {
      if (it != container.begin()) out += ", ";
      out += number(*it);
    }
    return out + "}";
  }

  static std::string number(int v) {
    if (v == 0) return "0";
    const bool negative = v < 0;
    // Negate in unsigned so INT_MIN does not overflow on the way out.
    unsigned int magnitude =
        negative ? 0u - static_cast<unsigned int>(v) : static_cast<unsigned int>(v);
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

// The running scenario: badge numbers known to the building, and the swipe
// log of one of them, in seconds since midnight.
static const int kBadges[] = {4021, 1337, 9002, 4711, 1337, 8080};
static const std::size_t kBadgeCount = sizeof(kBadges) / sizeof(kBadges[0]);

// Fills any container that has push_back from the badge table.
template <typename T>
static void fillBadges(T& container) {
  for (std::size_t i = 0; i < kBadgeCount; ++i) container.push_back(kBadges[i]);
}

// ─────────────────────────────────────────────────────────────────────────────
static void scenario_contract() {
  Ui::section("The contract: an iterator on success, an exception on a miss");

  std::vector<int> registry;
  fillBadges(registry);
  Ui::show("registry", Ui::join(registry));

  Ui::code("easyfind(registry, 9002)");
  std::vector<int>::iterator hit = easyfind(registry, 9002);
  Ui::show("*hit", *hit);
  Ui::show("index (hit - begin)", std::distance(registry.begin(), hit));

  Ui::code("easyfind(registry, 5555)   // not issued");
  try {
    easyfind(registry, 5555);
    Ui::verdict(false, "a miss must not return");
  } catch (const std::out_of_range& e) {
    Ui::threw(e);
  }
  Ui::note("std::find answers a miss with end(), a sentinel a caller can");
  Ui::note("forget to test. Converting it to a throw means every value that");
  Ui::note("comes back is dereferenceable — the same deal vector::at offers.");
}

static void scenario_any_container() {
  Ui::section("One function template, four containers");

  std::vector<int> asVector;
  std::list<int> asList;
  std::deque<int> asDeque;
  fillBadges(asVector);
  fillBadges(asList);
  fillBadges(asDeque);
  std::set<int> asSet(asVector.begin(), asVector.end());

  Ui::show("vector -> *easyfind(v, 4711)", *easyfind(asVector, 4711));
  Ui::show("list   -> *easyfind(l, 4711)", *easyfind(asList, 4711));
  Ui::show("deque  -> *easyfind(d, 4711)", *easyfind(asDeque, 4711));
  Ui::show("set    -> *easyfind(s, 4711)", *easyfind(asSet, 4711));

  Ui::note("Not one line of container-specific code exists. The template's");
  Ui::note("entire vocabulary is begin(), end() and typename T::iterator, so");
  Ui::note("any type that speaks it is already supported — including ones");
  Ui::note("that did not exist when easyfind was written.");
}

static void scenario_const_overload() {
  Ui::section("const containers: the second overload earns its keep");

  std::vector<int> mutableRegistry;
  fillBadges(mutableRegistry);
  const std::vector<int>& frozen = mutableRegistry;

  // Deduction picks easyfind(const T&) here, which returns const_iterator.
  std::vector<int>::const_iterator it = easyfind(frozen, 8080);
  Ui::show("*easyfind(frozen, 8080)", *it);
  Ui::note("Overload resolution, not a cast: a const argument prefers the");
  Ui::note("const T& overload, which returns T::const_iterator. Writing");
  Ui::note("through it is a compile error — see tests/compile_fail.sh.");

  // A temporary is const-qualified for binding purposes too.
  Ui::show("temporary vector, value found",
           *easyfind(std::vector<int>(1, 42), 42));
}

static void scenario_iterator_is_a_handle() {
  Ui::section("The return value is a handle, not a copy");

  std::vector<int> registry;
  fillBadges(registry);
  Ui::show("before", Ui::join(registry));

  // Badge 9002 is reissued: write straight through the iterator.
  *easyfind(registry, 9002) = 9003;
  Ui::code("*easyfind(registry, 9002) = 9003;");
  Ui::show("after reissue", Ui::join(registry));

  // Badge 8080 is destroyed: erase at the position we were handed.
  registry.erase(easyfind(registry, 8080));
  Ui::code("registry.erase(easyfind(registry, 8080));");
  Ui::show("after erase", Ui::join(registry));

  // A visitor badge is slotted in front of an existing one.
  registry.insert(easyfind(registry, 4711), 7);
  Ui::code("registry.insert(easyfind(registry, 4711), 7);");
  Ui::show("after insert", Ui::join(registry));

  Ui::note("Returning the iterator rather than the value or a bool is why");
  Ui::note("all three edits are one line: position, value and write access");
  Ui::note("arrive together.");
}

static void scenario_first_match() {
  Ui::section("First match wins, and distance turns it into an index");

  std::vector<int> registry;
  fillBadges(registry);
  Ui::show("registry", Ui::join(registry));
  Ui::note("1337 appears at index 1 and index 4 (a cloned badge).");

  const std::ptrdiff_t index =
      std::distance(registry.begin(), easyfind(registry, 1337));
  Ui::show("index of the first 1337", index);
  Ui::verdict(index == 1, "std::find scans front to back and stops early");

  // Finding the second one is the same call on the remaining range — the
  // reason easyfind hands back a position instead of a value.
  std::vector<int>::iterator after = easyfind(registry, 1337);
  ++after;
  std::vector<int>::iterator second = std::find(after, registry.end(), 1337);
  Ui::show("index of the second 1337",
           std::distance(registry.begin(), second));
}

static void scenario_cost_model() {
  Ui::section("Same answer, different price");

  std::set<int> sorted;
  for (std::size_t i = 0; i < kBadgeCount; ++i) sorted.insert(kBadges[i]);

  const int wanted = 9002;
  std::set<int>::iterator linear = easyfind(sorted, wanted);
  std::set<int>::iterator logarithmic = sorted.find(wanted);

  Ui::show("easyfind(set, 9002)", *linear);
  Ui::show("set::find(9002)", *logarithmic);
  Ui::verdict(linear == logarithmic, "identical iterators");
  Ui::note("std::find walks the tree in order: O(n). set::find descends it:");
  Ui::note("O(log n). Generic code that only promises begin()/end() cannot");
  Ui::note("know a container has a better search, which is the price of");
  Ui::note("being container-agnostic — worth knowing, not worth hiding.");
}

static void scenario_failure_is_inert() {
  Ui::section("A failed lookup changes nothing");

  std::vector<int> registry;
  fillBadges(registry);
  const std::size_t sizeBefore = registry.size();
  const std::vector<int> snapshot(registry);

  try {
    easyfind(registry, -1);
  } catch (const std::exception& e) {
    Ui::threw(e);
  }

  Ui::verdict(registry.size() == sizeBefore, "size untouched");
  Ui::verdict(registry == snapshot, "contents untouched");
  Ui::note("easyfind takes T& only so it can hand back a mutable iterator;");
  Ui::note("it never writes. The const overload proves it at compile time.");
}

static void scenario_catch_hierarchy() {
  Ui::section("Catching it: the exception type is a promise");

  std::vector<int> registry;
  fillBadges(registry);

  bool asExact = false;
  bool asLogic = false;
  bool asBase = false;

  try {
    easyfind(registry, 0);
  } catch (const std::out_of_range&) {
    asExact = true;
  }
  try {
    easyfind(registry, 0);
  } catch (const std::logic_error&) {
    asLogic = true;  // out_of_range derives from logic_error
  }
  try {
    easyfind(registry, 0);
  } catch (const std::exception&) {
    asBase = true;
  }

  Ui::verdict(asExact, "catch (const std::out_of_range&)");
  Ui::verdict(asLogic, "catch (const std::logic_error&)   — its base");
  Ui::verdict(asBase, "catch (const std::exception&)     — its base's base");
  Ui::note("Catching by const reference, never by value: a by-value catch");
  Ui::note("slices a derived exception down to the type you named.");
}

static void scenario_refusals() {
  Ui::section("What it refuses, on purpose");

  Ui::code("easyfind(aCArray, 42)        // no T::iterator");
  Ui::code("easyfind(aMap, 42)           // value_type is a pair, not an int");
  Ui::code("easyfind(vectorOfStrings, 42)  // no operator==(string, int)");
  Ui::code("std::vector<int>::iterator i = easyfind(constVec, 42);");

  Ui::note("All four are rejected by the compiler, which is the right place");
  Ui::note("for them. tests/compile_fail.sh builds each one and fails if any");
  Ui::note("of them ever starts compiling — a test suite cannot assert that.");
  Ui::note("The subject explicitly excuses associative containers; a set");
  Ui::note("still works because its value_type really is int.");
}

static void scenario_reconcile() {
  Ui::section("Putting it to work: revoking a list of badges");

  std::list<int> active;
  fillBadges(active);
  Ui::show("active", Ui::join(active));

  int revokedIds[] = {1337, 5555, 4021};
  const std::size_t revokedCount = sizeof(revokedIds) / sizeof(revokedIds[0]);

  int removed = 0;
  int unknown = 0;
  for (std::size_t i = 0; i < revokedCount; ++i) {
    try {
      // list::erase invalidates only the erased iterator, so the loop can
      // keep going with the container it just mutated.
      active.erase(easyfind(active, revokedIds[i]));
      ++removed;
      std::cout << "     " << Ui::kGreen << "revoked" << Ui::kOff << " "
                << Ui::number(revokedIds[i]) << std::endl;
    } catch (const std::out_of_range&) {
      ++unknown;
      std::cout << "     " << Ui::kYellow << "unknown" << Ui::kOff << " "
                << Ui::number(revokedIds[i]) << Ui::kDim
                << "  (never issued — nothing to do)" << Ui::kOff << std::endl;
    }
  }

  Ui::show("active after", Ui::join(active));
  Ui::show("revoked / unknown", Ui::number(removed) + " / " +
                                    Ui::number(unknown));
  Ui::note("The clone of badge 1337 survives: one revocation erases one");
  Ui::note("entry, because easyfind finds the first match. Revoking every");
  Ui::note("copy means looping until it throws — a miss as a loop condition.");

  int purged = 0;
  for (;;) {
    try {
      active.erase(easyfind(active, 1337));
      ++purged;
    } catch (const std::out_of_range&) {
      break;
    }
  }
  Ui::show("extra 1337s purged", purged);
  Ui::show("active finally", Ui::join(active));
}

static void scenario_bridge_to_ex01() {
  Ui::section("Handing the scenario to ex01");

  // Badge 4711's swipes today, seconds since midnight.
  std::deque<int> swipes;
  swipes.push_back(28800);  // 08:00:00
  swipes.push_back(28803);  // 08:00:03  <- three seconds later: tailgating?
  swipes.push_back(43200);  // 12:00:00
  swipes.push_back(61200);  // 17:00:00
  Ui::show("swipe log (s since midnight)", Ui::join(swipes));

  Ui::code("easyfind(swipes, 43200)   // did badge 4711 swipe at noon?");
  Ui::show("found at index",
           std::distance(swipes.begin(), easyfind(swipes, 43200)));

  Ui::note("easyfind answers 'is this exact value present'. The next");
  Ui::note("question — 'how close together are the two nearest swipes' —");
  Ui::note("is not a search at all, and no amount of std::find will answer");
  Ui::note("it. That is ex01: Span::shortestSpan() on this same log reports");
  Ui::note("3 seconds, which is how a tailgating alarm gets built.");
}

int main() {
  Ui::title("ex00 - easyfind: one algorithm, every container");
  std::cout << Ui::kDim
            << "  Scenario: the badge registry of a building's access control."
            << Ui::kOff << std::endl;

  scenario_contract();
  scenario_any_container();
  scenario_const_overload();
  scenario_iterator_is_a_handle();
  scenario_first_match();
  scenario_cost_model();
  scenario_failure_is_inert();
  scenario_catch_hierarchy();
  scenario_refusals();
  scenario_reconcile();
  scenario_bridge_to_ex01();

  std::cout << "\n"
            << Ui::kDim
            << "  make test runs the assertions, the fuzzer, the evaluation\n"
               "  checklist, the mutation run and the compile-failure cases."
            << Ui::kOff << std::endl
            << std::endl;
  return 0;
}
