/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 22:35:19 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:26:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "ClapTrap.hpp"
#include "ScavTrap.hpp"

__attribute__((weak)) int main() {
  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- 1. Testing ScavTrap Construction Chaining ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  ScavTrap scav("Guardian");

  std::cout << "\n[Initial Status]" << std::endl;
  std::cout << scav << std::endl;

  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- 2. Testing Overridden Attack ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  scav.attack("Bandit");  // Should print the ScavTrap specific message
  std::cout << scav << std::endl;

  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- 3. Testing Inherited Damage and Repair ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  scav.takeDamage(30);  // Uses ClapTrap's takeDamage
  scav.beRepaired(10);  // Uses ClapTrap's beRepaired
  std::cout << scav << std::endl;

  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- 4. Testing Special Ability ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  scav.guardGate();

  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- 5. Destruction Chaining ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  return 0;
}
