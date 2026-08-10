/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 22:35:19 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:31:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

#include "FragTrap.hpp"

// Utility for clean, automated reporting
void assertTest(const std::string& name, bool condition) {
  if (condition) {
    std::cout << GREEN << "[PASS] " << RESET << name << std::endl;
  } else {
    std::cout << RED << "[FAIL] " << RESET << name << std::endl;
  }
}

int main() {
  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- FRAGTRAP AUTOMATED EDGE-CASE FUZZER ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  // 1. Initialization Test
  std::cout << "\n[ Phase 1: Stat Initialization ]" << std::endl;
  FragTrap ft("FuzzerFrag");
  assertTest("Starting HP is 100", ft.getHitPoints() == 100);
  assertTest("Starting EP is 100", ft.getEnergyPoints() == 100);
  assertTest("Starting AD is 30", ft.getAttackDamage() == 30);

  // 2. Special Ability Test
  std::cout << "\n[ Phase 2: Special Ability ]" << std::endl;
  ft.highFivesGuys();

  // 3. Inherited Action Tests
  std::cout << "\n[ Phase 3: Inherited Actions ]" << std::endl;
  ft.attack("Bandit");  // Will output ClapTrap's attack format
  ft.takeDamage(50);
  assertTest("HP dropped to 50", ft.getHitPoints() == 50);
  ft.beRepaired(25);
  assertTest("HP healed to 75", ft.getHitPoints() == 75);
  assertTest("EP dropped to 98 (attack + repair)", ft.getEnergyPoints() == 98);

  // 4. Lethal Damage & Underflow Test
  std::cout << "\n[ Phase 4: Lethal Damage & Death State ]" << std::endl;
  ft.takeDamage(1000);  // Massive overkill damage
  assertTest("HP stopped exactly at 0 (No Underflow)", ft.getHitPoints() == 0);

  ft.highFivesGuys();  // Should be blocked from acting while dead

  // 5. Orthodox Canonical Form Test
  std::cout << "\n[ Phase 5: Deep Copy Validation ]" << std::endl;
  FragTrap original("PrimeFrag");
  original.takeDamage(20);  // HP = 80
  original.attack("X");     // EP = 99

  FragTrap clone(original);  // Copy Constructor
  FragTrap assigned("Temp");
  assigned = original;  // Assignment Operator

  assertTest("Clone inherited exact HP (80)", clone.getHitPoints() == 80);
  assertTest("Assigned inherited exact EP (99)",
             assigned.getEnergyPoints() == 99);
  assertTest("Clone retained FragTrap AD (30)", clone.getAttackDamage() == 30);

  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- END OF TESTS (Destructors will now fire) ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  return 0;
}
