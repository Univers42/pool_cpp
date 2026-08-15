/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Test-only support, not part of the turn-in: a section-aware assertion
// counter, so a failure says which scenario it came from instead of printing
// a bare line number, and a deterministic generator so the fuzzer replays.
//
// Everything is a static member function, which makes it implicitly inline and
// safe to include from several test translation units.

#ifndef CPP_MODULE08_EX00_TESTS_CHECK_HPP_
#define CPP_MODULE08_EX00_TESTS_CHECK_HPP_

#include <iostream>
#include <sstream>
#include <string>

class Check {
 public:
  static void section(const char* name) {
    if (g_sectionCount >= kMaxSections) return;
    Section& s = g_sections[g_sectionCount++];
    s.name = name;
    s.checks = 0;
    s.failed = 0;
  }

  static void ok(bool cond, const std::string& detail) {
    if (g_sectionCount == 0) section("(no section)");
    Section& s = g_sections[g_sectionCount - 1];
    ++s.checks;
    if (cond) return;
    ++s.failed;
    if (g_errorCount < kMaxErrors)
      g_errors[g_errorCount++] = std::string(s.name) + " - " + detail;
  }

  // Same as ok(), but prints both operands when it fails — which is the
  // difference between "index wrong" and a usable bug report.
  template <typename A, typename B>
  static void eq(const A& got, const B& want, const std::string& detail) {
    std::ostringstream why;
    why << detail << " (got " << got << ", want " << want << ")";
    ok(got == want, why.str());
  }

  // eq() for operands that have no operator<<, iterators above all. Same
  // comparison, no report of the values, because there is nothing to print.
  template <typename A, typename B>
  static void same(const A& got, const B& want, const std::string& detail) {
    ok(got == want, detail);
  }

  static int report(const char* title) {
    int checks = 0;
    int failed = 0;
    std::cout << "\n" << title << "\n" << std::endl;
    for (int i = 0; i < g_sectionCount; ++i) {
      const Section& s = g_sections[i];
      // 52 - len is unsigned, so a long name would wrap and throw.
      const std::string::size_type len = std::string(s.name).length();
      const std::string dots(len < 52 ? 52 - len : 1, '.');
      std::cout << "  " << s.name << " " << dots << " " << s.checks << " "
                << (s.failed ? "FAILED" : "ok") << std::endl;
      checks += s.checks;
      failed += s.failed;
    }
    std::cout << std::endl;
    if (!failed) {
      std::cout << "  " << g_sectionCount << " sections, " << checks
                << " checks, all passed" << std::endl;
      return 0;
    }
    std::cout << "  " << failed << " of " << checks << " checks FAILED"
              << std::endl
              << std::endl;
    for (int i = 0; i < g_errorCount; ++i)
      std::cout << "    " << g_errors[i] << std::endl;
    return 1;
  }

 private:
  // Declared, never defined: an accidental Check c; fails at link time.
  Check();
  Check(const Check& other);
  Check& operator=(const Check& other);
  ~Check();

  struct Section {
    const char* name;
    int checks;
    int failed;
  };

  static const int kMaxSections = 64;
  static const int kMaxErrors = 64;
  static Section g_sections[kMaxSections];
  static std::string g_errors[kMaxErrors];
  static int g_sectionCount;
  static int g_errorCount;
};

Check::Section Check::g_sections[Check::kMaxSections];
std::string Check::g_errors[Check::kMaxErrors];
int Check::g_sectionCount = 0;
int Check::g_errorCount = 0;

// A 32-bit linear congruential generator (the constants are Numerical
// Recipes'). std::rand is not forbidden here, but its sequence differs between
// libraries, and a fuzz failure that cannot be replayed on the machine that
// found it is not worth much. This one is the same everywhere.
class Rng {
 public:
  explicit Rng(unsigned int seed) : _state(seed) {}

  unsigned int next() {
    _state = _state * 1664525u + 1013904223u;
    return _state;
  }

  // Uniform-ish in [low, high]. The modulo bias is irrelevant at these ranges
  // and the alternative costs a rejection loop.
  int between(int low, int high) {
    const unsigned int span = static_cast<unsigned int>(high) -
                              static_cast<unsigned int>(low) + 1u;
    return static_cast<int>(static_cast<unsigned int>(low) + next() % span);
  }

  bool chance(unsigned int oneIn) { return next() % oneIn == 0; }

 private:
  unsigned int _state;
};

#endif  // CPP_MODULE08_EX00_TESTS_CHECK_HPP_
