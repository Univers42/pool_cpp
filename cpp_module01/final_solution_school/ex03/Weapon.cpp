/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Weapon.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:39:59 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:01:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Weapon.hpp"
#include <string>

/// @brief Constructs a Weapon with the given type string.
Weapon::Weapon(const std::string& type) : type(type) {}

/// @brief Destructs the Weapon.
Weapon::~Weapon() {}

/// @brief Returns a const reference to the type — no copy, always up-to-date.
const std::string& Weapon::getType() const { return this->type; }

/// @brief Replaces the weapon type; callers holding a reference/pointer see
///        the change immediately on their next call to getType().
void Weapon::setType(const std::string& type) { this->type = type; }
