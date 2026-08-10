/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Weapon.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:37:31 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:32:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX03_WEAPON_HPP_
#define CPP_MODULE01_EX03_WEAPON_HPP_

#include <string>

// ── Colour macros (shared by Weapon, HumanA, HumanB and main) ───────────────
#define C_RESET   "\033[0m"
#define C_BOLD    "\033[1m"
#define C_DIM     "\033[2m"
#define C_RED     "\033[31m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_BLUE    "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_CYAN    "\033[36m"

/**
 * @brief Represents a weapon that can be held and swapped by a human.
 *
 * @details
 * Stores a single string that describes the weapon's type (e.g. "crude spiked
 * club").  The type can be changed at any time via @ref setType, which is the
 * mechanism that proves both HumanA and HumanB always print the **current**
 * weapon type — not a copy taken at construction time.
 *
 * ### Why getType() returns a const reference
 * Returning `const std::string&` instead of `std::string` means:
 * - **No copy is made** — the caller reads directly from the internal string.
 * - The `const` prevents the caller from modifying the member through the
 *   returned reference.
 * - The reference stays valid as long as the Weapon object is alive and
 *   @ref setType is not called.
 */
class Weapon {
 private:
  std::string type; ///< Describes what kind of weapon this is.

 public:
  /**
   * @brief Constructs a Weapon with the given type string.
   * @param type A description of the weapon (e.g. "crude spiked club").
   */
  explicit Weapon(const std::string& type);

  /** @brief Destructs the Weapon. */
  ~Weapon();

  /**
   * @brief Returns a const reference to the weapon's type string.
   *
   * Returning by const reference avoids a copy and lets the caller observe
   * live updates made via @ref setType without re-querying.
   *
   * @return const std::string& The current weapon type.
   */
  const std::string& getType() const;

  /**
   * @brief Replaces the weapon's type string.
   *
   * Both HumanA (which holds a reference to this Weapon) and HumanB (which
   * holds a pointer) will automatically reflect the new type the next time
   * @c attack() is called, because neither stores a copy.
   *
   * @param type The new weapon type description.
   */
  void setType(const std::string& type);
};

#endif  // CPP_MODULE01_EX03_WEAPON_HPP_
