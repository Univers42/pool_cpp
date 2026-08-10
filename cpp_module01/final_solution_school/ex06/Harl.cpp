/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Harl.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 05:44:52 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:53:09 by dlesieur         ###   ########.fr       */
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
  std::cout
      << "I love having extra bacon for my "
         "7XL-double-cheese-triple-pickle-special-ketchup burger. I really do!"
      << std::endl;
}

/// @brief Prints the INFO complaint message.
void Harl::info(void) {
  std::cout
      << "I cannot believe adding extra bacon costs more money. You didn't put "
         "enough bacon in my burger! If you did, I wouldn't be asking for more!"
      << std::endl;
}

/// @brief Prints the WARNING complaint message.
void Harl::warning(void) {
  std::cout
      << "I think I deserve to have some extra bacon for free. I've been "
         "coming for years, whereas you started working here just last month."
      << std::endl;
}

/// @brief Prints the ERROR complaint message.
void Harl::error(void) {
  std::cout << "This is unacceptable! I want to speak to the manager now."
            << std::endl;
}

/// @brief Dispatches the complaint matching @p level via a pointer-to-member
///        table — no if/else chain, no map, no extra allocations.
/// @details
/// Two stack-allocated parallel arrays (4 function pointers + 4 strings) are
/// scanned once.  On match the method is invoked through `(this->*fp)()` and
/// the function returns immediately.  N is always 4, so this is effectively
/// O(1) with zero branching.
/// @param level "DEBUG", "INFO", "WARNING", or "ERROR". Unknown values are
///              silently ignored.
void Harl::complain(std::string level) {
  void (Harl::*methods[])(void) = {&Harl::debug, &Harl::info, &Harl::warning,
                                   &Harl::error};
  std::string levels[] = {"DEBUG", "INFO", "WARNING", "ERROR"};

  for (int i = 0; i < 4; i++) {
    if (levels[i] == level) {
      (this->*methods[i])();
      return;
    }
  }
}
