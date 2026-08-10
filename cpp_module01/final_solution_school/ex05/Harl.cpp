/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Harl.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:50:43 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:51:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Harl.hpp"

#include <iostream>
#include <string>

/// @brief Default constructor — no state to initialise.
Harl::Harl() {}

/// @brief Destructor — no resources to release.
Harl::~Harl() {}

/// @brief Prints the DEBUG complaint message.
void Harl::debug(void) {
  std::cout << C_BOLD << C_GREEN << "[ DEBUG ]" << C_RESET << std::endl;
  std::cout
      << "I love having extra bacon for my "
         "7XL-double-cheese-triple-pickle-special-ketchup burger. I really do!"
      << std::endl;
}

/// @brief Prints the INFO complaint message.
void Harl::info(void) {
  std::cout << C_BOLD << C_CYAN << "[ INFO ]" << C_RESET << std::endl;
  std::cout
      << "I cannot believe adding extra bacon costs more money. You didn't put "
         "enough bacon in my burger! If you did, I wouldn't be asking for more!"
      << std::endl;
}

/// @brief Prints the WARNING complaint message.
void Harl::warning(void) {
  std::cout << C_BOLD << C_YELLOW << "[ WARNING ]" << C_RESET << std::endl;
  std::cout
      << "I think I deserve to have some extra bacon for free. I've been "
         "coming for years, whereas you started working here just last month."
      << std::endl;
}

/// @brief Prints the ERROR complaint message.
void Harl::error(void) {
  std::cout << C_BOLD << C_RED << "[ ERROR ]" << C_RESET << std::endl;
  std::cout << "This is unacceptable! I want to speak to the manager now."
            << std::endl;
}

/// @brief Dispatches the complaint for @p level via a pointer-to-member table.
/// @details
/// Two parallel fixed-size arrays are declared on the stack:
///   - `methods[]` — four `void (Harl::*)()` pointers, one per log level.
///   - `levels[]`  — the four matching label strings.
///
/// A single loop compares @p level against each entry.  On the first match
/// at index `i`, the corresponding method is called as `(this->*methods[i])()`
/// and the function returns immediately, preventing fall-through.
///
/// The `->*` operator is required because member function pointers carry no
/// implicit object — they must be dereferenced in the context of a specific
/// instance (`this`).  Without it the call would not compile.
///
/// If no match is found the loop ends silently (unknown level is ignored).
/// @param level Case-sensitive log level string: "DEBUG", "INFO", "WARNING",
///              or "ERROR".  Any other value is a no-op.
void Harl::complain(std::string level) {
  // Parallel arrays: methods[i] is the handler for levels[i].
  // Using arrays instead of if/else satisfies the subject constraint.
  void (Harl::*methods[])(void) = {&Harl::debug, &Harl::info, &Harl::warning,
                                   &Harl::error};
  std::string levels[] = {"DEBUG", "INFO", "WARNING", "ERROR"};

  // O(N) linear scan — N is always 4, so this is effectively O(1).
  for (int i = 0; i < 4; i++) {
    if (levels[i] == level) {
      (this->*methods[i])(); // Dereference member pointer on 'this' and call.
      return;
    }
  }
}
