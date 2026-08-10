/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 22:35:19 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 22:35:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "ClapTrap.hpp"

int main() {
  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- 1. Testing Constructors & Orthodox Canonical Form ---"
            << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  ClapTrap a("Alpha");
  ClapTrap b("Bravo");
  ClapTrap c(a);  // Test Copy constructor
  ClapTrap d;     // Test Default constructor
  d = b;          // Test Assignment operator

  std::cout << "\n[Initial Status]" << std::endl;
  std::cout << "A: " << a << std::endl;
  std::cout << "B: " << b << std::endl;
  std::cout << "C: " << c << std::endl;
  std::cout << "D: " << d << std::endl;

  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- 2. Testing Attacks and Energy Depletion ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  // Alpha has 10 energy points. Let's attack 11 times.
  // The 11th attack should fail due to exhaustion.
  for (int i = 0; i < 11; ++i) {
    std::cout << "[" << i + 1 << "] ";
    a.attack("TargetDummy");
  }

  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- 3. Testing Damage, Repair, and Death ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  b.takeDamage(5);  // Takes 5 damage, survives with 5 HP
  std::cout << b << std::endl;

  b.beRepaired(3);  // Recovers 3 HP (now 8 HP), costs 1 EP
  std::cout << b << std::endl;

  b.takeDamage(15);  // Takes massive damage, HP shouldn't go below 0
  std::cout << b << std::endl;

  b.takeDamage(5);  // Should trigger the "already dead" message

  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- 4. Testing actions while Dead ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  b.attack("Alpha");  // b is dead, shouldn't attack
  b.beRepaired(5);    // b is dead, shouldn't repair

  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- 5. End of scope, destructors should trigger ---"
            << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  return 0;
}
