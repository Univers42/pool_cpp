/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 05:45:41 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:56:14 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "HumanA.hpp"
#include "HumanB.hpp"
#include "Weapon.hpp"

// ── Verbose mode ─────────────────────────────────────────────────────────────
// Default: on.  Compile with -DVERBOSE=0 to suppress explanatory banners.
#ifndef VERBOSE
# define VERBOSE 1
#endif

int main() {
#ifdef TESTS
  // ── Subject mandatory tests ───────────────────────────────────────────────
  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 1. MANDATORY SUBJECT TESTS ===" << C_RESET << std::endl;

  {
    std::cout << C_BOLD << C_CYAN << "\n[HumanA — always armed, weapon is a reference]"
              << C_RESET << std::endl;
    Weapon club("crude spiked club");
    HumanA bob("Bob", club);
    bob.attack();
    club.setType("some other type of club");
    bob.attack();
  }
  {
    std::cout << C_BOLD << C_CYAN << "\n[HumanB — optionally armed, weapon is a pointer]"
              << C_RESET << std::endl;
    Weapon club("crude spiked club");
    HumanB jim("Jim");
    jim.setWeapon(club);
    jim.attack();
    club.setType("some other type of club");
    jim.attack();
  }

  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 2. EDGE CASE: HUMAN_B UNARMED ===" << C_RESET << std::endl;
  {
    HumanB paul("Pacifist Paul");
    std::cout << C_YELLOW << "-> Paul attacks without a weapon:" << C_RESET << std::endl;
    paul.attack();
    std::cout << C_YELLOW << "-> Paul finds a shiny longsword!" << C_RESET << std::endl;
    Weapon sword("shiny longsword");
    paul.setWeapon(sword);
    paul.attack();
  }

  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 3. EDGE CASE: EMPTY STRINGS ===" << C_RESET << std::endl;
  {
    Weapon weirdWeapon("");
    HumanA ghost("", weirdWeapon);
    std::cout << C_YELLOW << "-> Nameless HumanA with a nameless weapon:" << C_RESET << std::endl;
    ghost.attack();
  }

  std::cout << C_BOLD << C_GREEN
            << "\n=== ALL TESTS COMPLETED SUCCESSFULLY ===\n"
            << C_RESET << std::endl;

#else
  // ── Default main — narrated combat scenario ───────────────────────────────
# if VERBOSE
  std::cout << C_BOLD << C_MAGENTA
            << "\n=== HumanA vs HumanB — reference vs pointer ===\n"
            << C_RESET << std::endl;
  std::cout << C_DIM
            << "HumanA holds a Weapon& — always armed, cannot be disarmed.\n"
               "HumanB holds a Weapon* — starts unarmed, can swap weapons."
            << C_RESET << "\n" << std::endl;
# endif

  // ── Round 1: HumanA — reference, always armed ────────────────────────────
# if VERBOSE
  std::cout << C_BOLD << C_CYAN << "[ HumanA — Bob ]" << C_RESET << std::endl;
  std::cout << C_DIM << "  Bob is constructed WITH a weapon (reference bound at birth)."
            << C_RESET << std::endl;
# endif
  Weapon club("crude spiked club");
  HumanA bob("Bob", club);

  bob.attack();

# if VERBOSE
  std::cout << C_DIM << "  Upgrading the shared weapon via setType()..."
            << C_RESET << std::endl;
# endif
  club.setType("enchanted war hammer");
  bob.attack();  // reflects new type instantly — reference, not a copy

# if VERBOSE
  std::cout << C_DIM
            << "  → Bob always has a weapon; the reference cannot be null."
            << C_RESET << "\n" << std::endl;
# endif

  // ── Round 2: HumanB — pointer, starts unarmed ────────────────────────────
# if VERBOSE
  std::cout << C_BOLD << C_CYAN << "[ HumanB — Jim ]" << C_RESET << std::endl;
  std::cout << C_DIM << "  Jim is constructed WITHOUT a weapon (pointer = NULL)."
            << C_RESET << std::endl;
# endif
  HumanB jim("Jim");
  jim.attack();  // unarmed — safe because attack() null-checks the pointer

# if VERBOSE
  std::cout << C_DIM << "  Jim picks up a dagger..." << C_RESET << std::endl;
# endif
  Weapon dagger("rusty dagger");
  jim.setWeapon(dagger);
  jim.attack();

# if VERBOSE
  std::cout << C_DIM << "  Jim swaps to a longsword mid-fight..." << C_RESET << std::endl;
# endif
  Weapon sword("flaming longsword");
  jim.setWeapon(sword);  // pointer reseated — impossible with a reference
  jim.attack();

  dagger.setType("poisoned dagger");  // mutating old weapon doesn't affect Jim
# if VERBOSE
  std::cout << C_DIM
            << "  Mutating the dagger Jim dropped — Jim is unaffected:"
            << C_RESET << std::endl;
# endif
  jim.attack();  // still shows "flaming longsword"

# if VERBOSE
  std::cout << C_DIM
            << "  → Jim can start unarmed and swap weapons freely;\n"
               "    Bob is always armed and always tracks his weapon live."
            << C_RESET << "\n" << std::endl;
# endif

#endif  // TESTS
  return 0;
}
