/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Zombie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 15:59:41 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 19:57:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX01_ZOMBIE_HPP_
#define CPP_MODULE01_EX01_ZOMBIE_HPP_

#include <iostream>
#include <string>

// ── UI Styling Macros ────────────────────────────────────────────────────────
#define C_RESET "\033[0m"
#define C_RED "\033[31m"
#define C_GREEN "\033[32m"
#define C_YELLOW "\033[33m"
#define C_CYAN "\033[36m"
#define C_MAGENTA "\033[35m"
#define C_BOLD "\033[1m"
#define C_DIM "\033[2m"

/**
 * @brief Represents an undead zombie entity that can be part of a horde.
 *
 * Supports both default construction (required for `new Zombie[N]` array
 * allocation) and named construction.  The name can be assigned after
 * construction via @ref setName, which is how @ref zombieHorde initialises
 * every member of the horde in a single allocation.
 */
class Zombie {
 private:
  std::string name; ///< The zombie's display name.

 public:
  /**
   * @brief Default constructor.
   *
   * Initialises the zombie with the placeholder name @c "Unnamed".
   * Required so that @c new Zombie[N] can allocate an array without
   * supplying a name up-front.
   */
  Zombie();

  /**
   * @brief Constructs a Zombie with the given name.
   *
   * Prints a creation message to standard output.
   *
   * @param name The name to assign to this zombie.
   */
  explicit Zombie(std::string name);

  /**
   * @brief Destructs the Zombie.
   *
   * Prints a destruction message to standard output so that array
   * deallocation order is clearly visible at runtime.
   */
  ~Zombie();

  /**
   * @brief Makes the zombie announce itself on standard output.
   *
   * Prints "<name>: BraiiiiiiinnnzzzZ..." using the zombie's stored name.
   */
  void announce(void);

  /**
   * @brief Sets (or replaces) the zombie's name.
   *
   * Used by @ref zombieHorde to assign the shared horde name to every
   * element of the array after default construction.
   *
   * @param name The new name to assign.
   */
  void setName(std::string name);
};

/**
 * @brief Allocates an array of @p N zombies on the heap, all sharing the
 *        same name, and returns a pointer to the first element.
 *
 * The array must be released with @c delete[] to avoid a memory leak.
 * Returns @c NULL when @p N is less than or equal to zero.
 *
 * @param N    The number of zombies to create.  Must be > 0.
 * @param name The name to assign to every zombie in the horde.
 * @return Zombie* Pointer to the first element of the heap-allocated array,
 *         or @c NULL if @p N <= 0.
 */
Zombie* zombieHorde(int N, std::string name);

#endif  // CPP_MODULE01_EX01_ZOMBIE_HPP_
