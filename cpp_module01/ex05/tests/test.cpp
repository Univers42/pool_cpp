/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Exact-output tests for Harl::complain: captures std::cout via rdbuf
// redirection and byte-compares against the subject's verbatim messages.
// Exits non-zero on any failure.

#include <iostream>
#include <sstream>
#include <string>

#include "Harl.hpp"

static int g_fails = 0;

static std::string capture(Harl& h, const std::string& level) {
  std::ostringstream cap;
  std::streambuf* old = std::cout.rdbuf(cap.rdbuf());
  h.complain(level);
  std::cout.rdbuf(old);
  return cap.str();
}

static void expectEq(const char* name, const std::string& got,
                     const std::string& want) {
  if (got != want) {
    std::cerr << "[FAIL] " << name << "\n       want: '" << want
              << "'\n       got : '" << got << "'\n";
    ++g_fails;
  }
}

static const char* kDebug =
    "[ DEBUG ]\n"
    "I love having extra bacon for my "
    "7XL-double-cheese-triple-pickle-special-ketchup burger. I really do!\n";
static const char* kInfo =
    "[ INFO ]\n"
    "I cannot believe adding extra bacon costs more money. You didn't put "
    "enough bacon in my burger! If you did, I wouldn't be asking for more!\n";
static const char* kWarning =
    "[ WARNING ]\n"
    "I think I deserve to have some extra bacon for free. I've been "
    "coming for years, whereas you started working here just last month.\n";
static const char* kError =
    "[ ERROR ]\n"
    "This is unacceptable! I want to speak to the manager now.\n";

int main() {
  Harl harl;

  // Each level dispatches to the right member function with the subject's
  // exact message.
  expectEq("DEBUG level", capture(harl, "DEBUG"), kDebug);
  expectEq("INFO level", capture(harl, "INFO"), kInfo);
  expectEq("WARNING level", capture(harl, "WARNING"), kWarning);
  expectEq("ERROR level", capture(harl, "ERROR"), kError);

  // Unknown and case-mismatched levels are silent no-ops by design.
  expectEq("unknown level is silent", capture(harl, "MCDONALDS"), "");
  expectEq("lowercase is not a level", capture(harl, "debug"), "");
  expectEq("empty level is silent", capture(harl, ""), "");

  // Harl complains a lot: repeated dispatch stays correct and stateless.
  std::string round;
  round += kDebug;
  round += kInfo;
  round += kWarning;
  round += kError;
  std::string want3;
  std::string got3;
  for (int i = 0; i < 3; ++i) {
    want3 += round;
    got3 += capture(harl, "DEBUG");
    got3 += capture(harl, "INFO");
    got3 += capture(harl, "WARNING");
    got3 += capture(harl, "ERROR");
  }
  expectEq("three full rounds of complaints", got3, want3);

  if (g_fails) {
    std::cerr << g_fails << " ex05 check(s) FAILED\n";
    return 1;
  }
  std::cout << "ex05 tests: all OK\n";
  return 0;
}
