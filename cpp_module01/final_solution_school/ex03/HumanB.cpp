/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HumanB.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:47:16 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:32:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HumanB.hpp"

#include <iostream>
#include <string>

/// @brief Constructs an unarmed HumanB; weapon pointer initialised to NULL.
HumanB::HumanB(const std::string& name) : name(name), weapon(NULL) {}

/// @brief Destructs the HumanB.
HumanB::~HumanB() {}

/// @brief Stores the address of @p weapon, arming (or re-arming) this human.
void HumanB::setWeapon(Weapon& weapon) { this->weapon = &weapon; }

/// @brief Prints an attack message, or "unarmed" if weapon pointer is NULL.
///        The null-check here is what makes HumanB safe to call unarmed.
void HumanB::attack() const {
  if (this->weapon) {
    std::cout << C_BOLD << C_YELLOW << this->name << C_RESET
              << " attacks with their "
              << C_BOLD << C_RED << this->weapon->getType() << C_RESET
              << std::endl;
  } else {
    std::cout << C_BOLD << C_YELLOW << this->name << C_RESET
              << C_DIM << " is unarmed and cannot attack!" << C_RESET
              << std::endl;
  }
}
