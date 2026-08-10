/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 06:26:07 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:30:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

// ── Colour macros ────────────────────────────────────────────────────────────
#define C_RESET   "\033[0m"
#define C_BOLD    "\033[1m"
#define C_DIM     "\033[2m"
#define C_RED     "\033[31m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_BLUE    "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_CYAN    "\033[36m"
#define C_WHITE   "\033[37m"

// ── Verbose mode ─────────────────────────────────────────────────────────────
// Default: on.  Compile with -DVERBOSE=0 to suppress explanatory output and
// see only the raw addresses/values.
#ifndef VERBOSE
# define VERBOSE 0
#endif

/**
 * @brief Demonstrates the three ways to access a value in memory in C++:
 *        a variable, a pointer, and a reference.
 *
 * @details
 * All three — @c greet, @c stringPTR, and @c stringREF — ultimately refer to
 * the same memory location, which is why all three address prints are
 * identical and all three value prints show the same string.
 *
 * Compile with @c -DVERBOSE=0 to suppress the explanatory banners and print
 * only the raw data lines (useful for diffing output in automated tests).
 *
 * @return int Always returns 0 (success).
 */
int main(void) {
  std::string  greet     = "Hi this is brain";
  std::string* stringPTR = &greet;
  std::string& stringREF = greet;

  // ── Section 1: Addresses ──────────────────────────────────────────────────
#if VERBOSE
  std::cout << C_BOLD << C_MAGENTA
            << "\n=== ADDRESSES ==="
            << C_RESET << std::endl;
  std::cout << C_DIM
            << "All three should print the SAME address — they all point to\n"
               "the same block of memory that belongs to 'greet'."
            << C_RESET << "\n" << std::endl;
#endif

  std::cout << C_YELLOW << "greet    " << C_RESET
            << C_DIM << "(variable)  " << C_RESET
            << "address : " << C_CYAN << &greet     << C_RESET << std::endl;
  std::cout << C_GREEN  << "stringPTR" << C_RESET
            << C_DIM << "(pointer)   " << C_RESET
            << "holds   : " << C_CYAN << stringPTR  << C_RESET << std::endl;
  std::cout << C_BLUE   << "stringREF" << C_RESET
            << C_DIM << "(reference) " << C_RESET
            << "address : " << C_CYAN << &stringREF << C_RESET << std::endl;

#if VERBOSE
  std::cout << C_DIM
            << "\n→ All three addresses are identical: no copy was made."
            << C_RESET << std::endl;

  // Also show the pointer's OWN address to illustrate it is a separate variable
  std::cout << C_DIM
            << "→ stringPTR itself lives at : "
            << &stringPTR
            << "  (the pointer variable has its own storage)"
            << C_RESET << std::endl;
#endif

  // ── Section 2: Values ─────────────────────────────────────────────────────
#if VERBOSE
  std::cout << C_BOLD << C_MAGENTA
            << "\n=== VALUES ==="
            << C_RESET << std::endl;
  std::cout << C_DIM
            << "Each accessor reads the same underlying string, but the\n"
               "syntax differs: direct name / dereference (*) / alias name."
            << C_RESET << "\n" << std::endl;
#endif

  std::cout << C_YELLOW << "greet    " << C_RESET
            << C_DIM << "(direct)      " << C_RESET
            << "value : " << C_WHITE << C_BOLD << greet      << C_RESET << std::endl;
  std::cout << C_GREEN  << "stringPTR" << C_RESET
            << C_DIM << "(*ptr)        " << C_RESET
            << "value : " << C_WHITE << C_BOLD << *stringPTR << C_RESET << std::endl;
  std::cout << C_BLUE   << "stringREF" << C_RESET
            << C_DIM << "(alias)       " << C_RESET
            << "value : " << C_WHITE << C_BOLD << stringREF  << C_RESET << std::endl;

#if VERBOSE
  std::cout << C_DIM
            << "\n→ All three values are identical: same object, three names."
            << C_RESET << std::endl;

  // ── Section 3: Live mutation demo ─────────────────────────────────────────
  std::cout << C_BOLD << C_MAGENTA
            << "\n=== MUTATION THROUGH POINTER ==="
            << C_RESET << std::endl;
  std::cout << C_DIM
            << "Writing through stringPTR changes 'greet' and is\n"
               "immediately visible through stringREF — they are the same object."
            << C_RESET << "\n" << std::endl;

  *stringPTR = "BraiiiiiiinnnzzzZ...";

  std::cout << C_YELLOW << "greet    " << C_RESET
            << C_DIM << "(after *stringPTR = ...) " << C_RESET
            << "value : " << C_WHITE << C_BOLD << greet     << C_RESET << std::endl;
  std::cout << C_BLUE   << "stringREF" << C_RESET
            << C_DIM << "(after *stringPTR = ...) " << C_RESET
            << "value : " << C_WHITE << C_BOLD << stringREF << C_RESET << std::endl;

  std::cout << C_DIM
            << "\n→ One write, visible from all three accessors."
            << C_RESET << "\n" << std::endl;
#endif

  return 0;
}
