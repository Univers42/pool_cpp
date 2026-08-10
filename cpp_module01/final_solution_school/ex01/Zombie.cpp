/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Zombie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:02:21 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 19:57:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Zombie.hpp"
#include <iostream>
#include <string>

/// @brief Default constructor — initialises name to "Unnamed".
Zombie::Zombie(void) : name("Unnamed") {
  std::cout << C_DIM << "  [System] " << C_GREEN
            << "An unnamed Zombie crawled out of the grave." << C_RESET
            << std::endl;
}

/// @brief Constructs a Zombie with a given name and prints a creation message.
Zombie::Zombie(std::string name) : name(name) {
  std::cout << C_DIM << "  [System] " << C_GREEN << "Zombie '" << this->name
            << "' crawled out of the grave." << C_RESET << std::endl;
}

/// @brief Destructs the Zombie and prints a destruction message.
Zombie::~Zombie() {
  std::cout << C_DIM << "  [System] " << C_RED << "Zombie '" << this->name
            << "' turned to dust." << C_RESET << std::endl;
}

/// @brief Prints "<name>: BraiiiiiiinnnzzzZ..." to standard output.
void Zombie::announce(void) {
  std::cout << C_BOLD << C_CYAN << this->name << C_RESET
            << ": BraiiiiiiinnnzzzZ..." << std::endl;
}

/// @brief Replaces the zombie's name.
/// @param name The new name to assign.
void Zombie::setName(std::string name) { this->name = name; }
