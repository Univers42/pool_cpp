/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HumanA.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:35:30 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:33:46 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HumanA.hpp"

#include <iostream>
#include <string>

/// @brief Binds name and weapon reference in the initialiser list (O(1), no copy).
HumanA::HumanA(const std::string& name, Weapon& weapon)
    : name(name), weapon(weapon) {}

/// @brief Destructs the HumanA.
HumanA::~HumanA() {}

/// @brief Prints "<name> attacks with their <weapon.getType()>".
///        Always reflects the live weapon type — no null check needed.
void HumanA::attack() const {
  std::cout << C_BOLD << C_YELLOW << this->name << C_RESET
            << " attacks with their "
            << C_BOLD << C_RED << this->weapon.getType() << C_RESET
            << std::endl;
}
