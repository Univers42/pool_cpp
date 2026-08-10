/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ZombieHorde.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:28:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 19:57:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include "Zombie.hpp"

/// @brief Allocates an array of @p N zombies, assigns @p name to each, and
///        returns a pointer to the first element.
/// @param N    Number of zombies to create.  Must be > 0.
/// @param name The name shared by every zombie in the horde.
/// @return Zombie* Heap-allocated array (use delete[]), or NULL if N <= 0.
Zombie* zombieHorde(int N, std::string name) {
  if (N <= 0) return NULL;
  Zombie* horde = new Zombie[N];
  for (int i = 0; i < N; i++) {
    horde[i].setName(name);
  }
  return horde;
}
