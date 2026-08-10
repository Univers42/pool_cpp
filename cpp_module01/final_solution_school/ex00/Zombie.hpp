/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Zombie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 15:59:41 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 19:55:14 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX00_ZOMBIE_HPP_
#define CPP_MODULE01_EX00_ZOMBIE_HPP_

#include <iostream>
#include <string>

#define C_RESET "\033[0m"
#define C_RED "\033[31m"
#define C_GREEN "\033[32m"
#define C_YELLOW "\033[33m"
#define C_CYAN "\033[36m"
#define C_MAGENTA "\033[35m"
#define C_BOLD "\033[1m"
#define C_DIM "\033[2m"

/**
 * @brief Represents an undead zombie entity.
 *
 * A Zombie can be allocated on the stack (short-lived) or on the heap
 * (manually managed lifetime).  On construction it announces its arrival;
 * on destruction it announces its demise.
 */
class Zombie {
 private:
  std::string name; ///< The zombie's display name.

 public:
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
   * Prints a destruction message to standard output so that stack- vs.
   * heap-lifetime differences are clearly visible at runtime.
   */
  ~Zombie();

  /**
   * @brief Makes the zombie announce itself on standard output.
   *
   * Prints "<name>: BraiiiiiiinnnzzzZ..." using the zombie's stored name.
   */
  void announce(void);
};

/**
 * @brief Allocates a new Zombie on the heap and returns a pointer to it.
 *
 * The caller is responsible for deleting the returned pointer when it is no
 * longer needed to avoid a memory leak.
 *
 * @param name The name to assign to the new zombie.
 * @return Zombie* A pointer to the newly created Zombie.
 */
Zombie* newZombie(std::string name);

/**
 * @brief Creates a Zombie on the stack, makes it announce itself, then
 *        destroys it automatically when the function returns.
 *
 * Demonstrates stack allocation: the zombie's lifetime is bound to this
 * function's scope.
 *
 * @param name The name to assign to the temporary zombie.
 */
void randomChump(std::string name);

#endif  // CPP_MODULE01_EX00_ZOMBIE_HPP_
