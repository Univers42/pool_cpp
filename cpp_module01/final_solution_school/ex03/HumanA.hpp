/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HumanA.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:41:20 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:01:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX03_HUMANA_HPP_
#define CPP_MODULE01_EX03_HUMANA_HPP_

#include <string>

#include "Weapon.hpp"

/**
 * @brief A human combatant that is **always** armed.
 *
 * @details
 * HumanA stores its weapon as a **reference** (`Weapon&`), which means:
 * - The weapon **must** be provided at construction — there is no default.
 * - The reference cannot be reseated; HumanA is permanently bound to the
 *   same Weapon object for its entire lifetime.
 * - No null-check is needed inside @ref attack because a reference can never
 *   be null.
 * - Because it stores a reference (not a copy), any change made to the
 *   Weapon via `Weapon::setType` is immediately visible when `attack()` is
 *   called next.
 *
 * ### Reference vs pointer — why reference here?
 * | Aspect          | `Weapon&` (HumanA)         | `Weapon*` (HumanB)          |
 * |-----------------|----------------------------|-----------------------------|
 * | Can be null?    | No — always valid          | Yes — unarmed state         |
 * | Can be reseated?| No                         | Yes — via `setWeapon()`     |
 * | Null check needed? | No                      | Yes                         |
 * | Syntax          | `weapon.getType()`         | `weapon->getType()`         |
 */
class HumanA {
 private:
  std::string name; ///< The human's name.
  Weapon& weapon;   ///< Reference to the weapon — always valid, never null.

 public:
  /**
   * @brief Constructs a HumanA that is immediately armed.
   *
   * The weapon reference is bound in the initialiser list and cannot change
   * afterwards.
   *
   * @param name   The human's name.
   * @param weapon The weapon this human always carries.
   */
  HumanA(const std::string& name, Weapon& weapon);

  /** @brief Destructs the HumanA. */
  ~HumanA();

  /**
   * @brief Prints an attack message using the current weapon type.
   *
   * Because @c weapon is a reference, this always reflects the live state of
   * the Weapon object — even if its type was changed after construction.
   */
  void attack() const;
};

#endif  // CPP_MODULE01_EX03_HUMANA_HPP_
