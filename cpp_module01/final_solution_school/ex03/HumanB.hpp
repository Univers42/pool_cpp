/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HumanB.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:45:20 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:01:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX03_HUMANB_HPP_
#define CPP_MODULE01_EX03_HUMANB_HPP_

#include <string>

#include "Weapon.hpp"

/**
 * @brief A human combatant that **may or may not** be armed.
 *
 * @details
 * HumanB stores its weapon as a **pointer** (`Weapon*`), which means:
 * - The weapon is optional — HumanB can be constructed without one.
 * - The pointer can be set or swapped at any time via @ref setWeapon.
 * - @ref attack must check for `nullptr` before dereferencing to avoid a
 *   segmentation fault.
 * - Like HumanA, any change to the Weapon via `Weapon::setType` is
 *   immediately visible on the next `attack()` call.
 *
 * ### Why a pointer and not a reference?
 * A reference must be bound to a valid object at construction and can never
 * be reseated.  Because HumanB can start unarmed (null) and acquire a weapon
 * later, a raw pointer is the correct choice here.
 *
 * @see HumanA for the always-armed counterpart that uses a reference.
 */
class HumanB {
 private:
  std::string name; ///< The human's name.
  Weapon* weapon;   ///< Pointer to the weapon — may be nullptr (unarmed).

 public:
  /**
   * @brief Constructs an unarmed HumanB.
   *
   * The internal weapon pointer is initialised to @c NULL.  Call
   * @ref setWeapon before attacking to arm this human.
   *
   * @param name The human's name.
   */
  explicit HumanB(const std::string& name);

  /** @brief Destructs the HumanB. */
  ~HumanB();

  /**
   * @brief Arms (or re-arms) this human with the given weapon.
   *
   * Stores the address of @p weapon.  The caller must ensure the Weapon
   * object outlives this HumanB, otherwise the pointer becomes dangling.
   *
   * @param weapon The weapon to equip.
   */
  void setWeapon(Weapon& weapon);

  /**
   * @brief Prints an attack message, or an "unarmed" message if no weapon
   *        has been equipped.
   *
   * Performs a null-check on the internal pointer before dereferencing it,
   * so calling this method on an unarmed HumanB is safe.
   */
  void attack() const;
};

#endif  // CPP_MODULE01_EX03_HUMANB_HPP_
