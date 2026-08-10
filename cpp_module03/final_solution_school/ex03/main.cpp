/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 22:35:19 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/05 14:40:45 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

#include "DiamondTrap.hpp"

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
  std::cout << "--- DIAMONDTRAP AUTOMATED EDGE-CASE FUZZER ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  std::cout << "\n[ Phase 1: Construction & Stat Initialization ]" << std::endl;
  DiamondTrap dt("Goliath");
  assertTest("Starting HP is 100 (from FragTrap)", dt.getHitPoints() == 100);
  assertTest("Starting EP is 50 (from ScavTrap)", dt.getEnergyPoints() == 50);
  assertTest("Starting AD is 30 (from FragTrap)", dt.getAttackDamage() == 30);

  std::cout << "\n[ Phase 2: Dual Inheritance Abilities ]" << std::endl;
  dt.guardGate();      
  dt.highFivesGuys();  
  dt.whoAmI();         

  std::cout << "\n[ Phase 3: Action Ambiguity Resolution ]" << std::endl;
  dt.attack("Target");
  assertTest("EP dropped to 49 (ScavTrap attack successful)",
             dt.getEnergyPoints() == 49);

  std::cout << "\n[ Phase 4: Name Shadowing Verification ]" << std::endl;
  
  // FIX IS HERE: We explicitly call ClapTrap's version of getName() to verify the suffix
  assertTest("ClapTrap::_name correctly ends with '_clap_name'",
             dt.ClapTrap::getName() == "Goliath_clap_name");
             
  // Let's also verify that DiamondTrap kept its raw name
  assertTest("DiamondTrap::_name is the raw name",
             dt.getName() == "Goliath");

  std::cout << "\n[ Phase 5: Orthodox Canonical Form ]" << std::endl;
  DiamondTrap clone(dt);
  DiamondTrap assigned("Temp");
  assigned = dt;

  assertTest("Clone has identical HP",
             clone.getHitPoints() == dt.getHitPoints());
  assertTest("Assigned has identical EP",
             assigned.getEnergyPoints() == dt.getEnergyPoints());

  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "--- END OF TESTS (Destructors will now fire) ---" << std::endl;
  std::cout << "========================================================\n"
            << std::endl;

  return 0;
}