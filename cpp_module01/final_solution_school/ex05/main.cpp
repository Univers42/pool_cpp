/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 17:51:44 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 21:10:58 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Harl.hpp"

// ── Verbose mode ─────────────────────────────────────────────────────────────
// Default: on.  Compile with -DVERBOSE=0 to suppress explanatory banners.
#ifndef VERBOSE
# define VERBOSE 1
#endif

#ifdef TESTS
/**
 * @brief Extended test suite — compile with -DTESTS to enable.
 *
 * @details
 * Each call to `harl.complain()` demonstrates a different path through the
 * pointer-to-member dispatch table:
 *
 * | Call                       | Expected output header |
 * |----------------------------|------------------------|
 * | `complain("DEBUG")`        | `[ DEBUG ]`            |
 * | `complain("INFO")`         | `[ INFO ]`             |
 * | `complain("WARNING")`      | `[ WARNING ]`          |
 * | `complain("ERROR")`        | `[ ERROR ]`            |
 * | `complain("MCDONALDS")`    | *(silent — no match)*  |
 *
 * @return int Always 0 (success).
 */
int main(void) {
  Harl harl;

  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 1. TESTING ALL FOUR LOG LEVELS ===" << C_RESET
            << std::endl;

  std::cout << C_YELLOW << "-> complain(\"DEBUG\")" << C_RESET << std::endl;
  harl.complain("DEBUG");

  std::cout << C_YELLOW << "\n-> complain(\"INFO\")" << C_RESET << std::endl;
  harl.complain("INFO");

  std::cout << C_YELLOW << "\n-> complain(\"WARNING\")" << C_RESET << std::endl;
  harl.complain("WARNING");

  std::cout << C_YELLOW << "\n-> complain(\"ERROR\")" << C_RESET << std::endl;
  harl.complain("ERROR");

  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 2. TESTING UNKNOWN LEVEL ===" << C_RESET << std::endl;
  std::cout << C_YELLOW << "-> complain(\"MCDONALDS\") — should be silent"
            << C_RESET << std::endl;
  harl.complain("MCDONALDS");
  std::cout << C_DIM << "-> (no output — unknown level was silently ignored)"
            << C_RESET << std::endl;

  std::cout << C_BOLD << C_GREEN
            << "\n=== ALL TESTS COMPLETED SUCCESSFULLY ===\n"
            << C_RESET << std::endl;

  return (0);
}

#else
/**
 * @brief Default main — exercises each Harl complaint level once.
 *
 * @details
 * ### How Harl::complain works (pointer-to-member dispatch)
 * Instead of an if/else chain, `complain()` holds two parallel arrays:
 * - `std::string levels[]`  with the four level names.
 * - `void (Harl::*methods[])()` with the four matching member functions.
 *
 * A single loop scans both arrays.  When the input matches `levels[i]`,
 * the method is invoked through `(this->*methods[i])()`.  The result is
 * a clean O(1) dispatch with zero branching — exactly four comparisons
 * in the worst case.
 */
int main(void) {
  Harl harl;

# if VERBOSE
  std::cout << C_BOLD << C_MAGENTA
            << "\n=== Harl 2000 — Pointer-to-Member Dispatch ===\n"
            << C_RESET << std::endl;
  std::cout << C_DIM
            << "complain() maps a level string to a private method using\n"
               "parallel arrays of function pointers — no if/else, no switch."
            << C_RESET << "\n" << std::endl;
# endif

  // ── DEBUG ─────────────────────────────────────────────────────────────────
# if VERBOSE
  std::cout << C_BOLD << C_CYAN << "[ Calling complain(\"DEBUG\") ]" << C_RESET
            << std::endl;
  std::cout << C_DIM << "  Dispatches to Harl::debug() via pointer table."
            << C_RESET << std::endl;
# endif
  harl.complain("DEBUG");

  // ── INFO ──────────────────────────────────────────────────────────────────
# if VERBOSE
  std::cout << C_BOLD << C_CYAN << "\n[ Calling complain(\"INFO\") ]" << C_RESET
            << std::endl;
  std::cout << C_DIM << "  Dispatches to Harl::info() via pointer table."
            << C_RESET << std::endl;
# endif
  harl.complain("INFO");

  // ── WARNING ───────────────────────────────────────────────────────────────
# if VERBOSE
  std::cout << C_BOLD << C_CYAN << "\n[ Calling complain(\"WARNING\") ]" << C_RESET
            << std::endl;
  std::cout << C_DIM << "  Dispatches to Harl::warning() via pointer table."
            << C_RESET << std::endl;
# endif
  harl.complain("WARNING");

  // ── ERROR ─────────────────────────────────────────────────────────────────
# if VERBOSE
  std::cout << C_BOLD << C_CYAN << "\n[ Calling complain(\"ERROR\") ]" << C_RESET
            << std::endl;
  std::cout << C_DIM << "  Dispatches to Harl::error() via pointer table."
            << C_RESET << std::endl;
# endif
  harl.complain("ERROR");

# if VERBOSE
  std::cout << C_DIM
            << "\n  → Each level was resolved at runtime through a single\n"
               "    loop over the pointer-to-member table — no branching."
            << C_RESET << "\n" << std::endl;
# endif

  return 0;
}
#endif
