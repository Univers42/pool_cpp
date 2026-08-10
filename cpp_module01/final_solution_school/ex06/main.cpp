/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 05:44:57 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 21:12:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

#include "Harl.hpp"

// ── Verbose mode ─────────────────────────────────────────────────────────────
// Default: on.  Compile with -DVERBOSE=0 to suppress explanatory banners.
#ifndef VERBOSE
# define VERBOSE 1
#endif

/**
 * @brief Prints all Harl complaints at or above the requested minimum level,
 *        using a `switch` with intentional fallthrough.
 *
 * @details
 * ### The core concept — switch fallthrough
 * A `switch` statement in C++ executes from the matching `case` label
 * **all the way to the end** unless a `break` is encountered.  This
 * "fallthrough" behaviour is normally a bug, but here it is the *feature*:
 * by placing only one `break` at the very end (`case 3`), every case that
 * matches will execute itself **and** every subsequent case automatically.
 *
 * Example — passing `"WARNING"` maps to `level_index = 2`:
 * @code
 *   switch (2) {
 *     case 0: // skipped — switch jumps directly to case 2
 *     case 1: // skipped
 *     case 2: harl.complain("WARNING");  // ← entry point
 *     case 3: harl.complain("ERROR");    // ← falls through, no break needed
 *       break;
 *   }
 * @endcode
 *
 * ### String-to-index conversion
 * `switch` only accepts integral expressions, not strings.  A small
 * linear scan converts the command-line argument to an integer index
 * (`0`=DEBUG, `1`=INFO, `2`=WARNING, `3`=ERROR) before the switch.
 * An unrecognised string leaves `level_index = -1`, which hits `default`.
 *
 * ### Dispatch table (level → case → output)
 * | argv[1]    | level_index | Cases executed    |
 * |------------|-------------|-------------------|
 * | "DEBUG"    | 0           | 0 → 1 → 2 → 3    |
 * | "INFO"     | 1           | 1 → 2 → 3         |
 * | "WARNING"  | 2           | 2 → 3             |
 * | "ERROR"    | 3           | 3                 |
 * | *anything* | -1          | default           |
 *
 * ### Usage
 * @code
 *   ./ex06 <level>
 *   ./ex06 WARNING   # prints WARNING and ERROR messages
 * @endcode
 *
 * @param argc Must be exactly 2 (program name + one level argument).
 *             If not, a default message is printed and the program exits.
 * @param argv argv[1] is the minimum log level to display.
 * @return int Always 0 (success).
 */
int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "[ Probably complaining about insignificant problems ]"
              << std::endl;
    return 0;
  }

  Harl harl;
  std::string input = argv[1];
  std::string levels[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
  int level_index = -1;

  // Convert the string argument to an integer index so switch can use it.
  // level_index stays -1 if no match is found, which routes to default.
  for (int i = 0; i < 4; i++) {
    if (input == levels[i]) {
      level_index = i;
      break;
    }
  }

# if VERBOSE
  std::cout << C_BOLD << C_MAGENTA
            << "\n=== harlFilter — switch fallthrough dispatch ===\n"
            << C_RESET << std::endl;
  std::cout << C_DIM
            << "The switch enters at the matching severity and falls through\n"
               "to ERROR — printing every level from the minimum upward.\n"
               "Unknown levels hit the default case."
            << C_RESET << "\n" << std::endl;
  if (level_index >= 0)
    std::cout << C_DIM << "  Minimum level: " << C_CYAN << input << C_RESET
              << C_DIM << " (index " << level_index
              << ") — will print levels " << level_index << "..3"
              << C_RESET << "\n" << std::endl;
  else
    std::cout << C_DIM << "  Unknown level: " << C_RED << input << C_RESET
              << C_DIM << " — will hit default case"
              << C_RESET << "\n" << std::endl;
# endif

  // Intentional fallthrough: no break between cases 0-2 means every case
  // prints its own level AND all levels that follow it down to ERROR.
  // The /* FALLTHROUGH */ comment is a recognised annotation that suppresses
  // compiler warnings about implicit fallthrough (-Wimplicit-fallthrough).
  switch (level_index) {
    case 0:
      std::cout << C_BOLD << C_GREEN << "[ DEBUG ]" << C_RESET << std::endl;
      harl.complain("DEBUG");
      std::cout << std::endl;
      /* FALLTHROUGH */
    case 1:
      std::cout << C_BOLD << C_CYAN << "[ INFO ]" << C_RESET << std::endl;
      harl.complain("INFO");
      std::cout << std::endl;
      /* FALLTHROUGH */
    case 2:
      std::cout << C_BOLD << C_YELLOW << "[ WARNING ]" << C_RESET << std::endl;
      harl.complain("WARNING");
      std::cout << std::endl;
      /* FALLTHROUGH */
    case 3:
      std::cout << C_BOLD << C_RED << "[ ERROR ]" << C_RESET << std::endl;
      harl.complain("ERROR");
      std::cout << std::endl;
      break; // Only break in the chain — stops execution after ERROR.
    default:
      // Reached when argv[1] does not match any known level.
      std::cout << C_YELLOW
                << "[ Probably complaining about insignificant problems ]"
                << C_RESET << std::endl;
  }

  return 0;
}
