/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   newZombie.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:05:03 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 19:57:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include "Zombie.hpp"

/// @brief Allocates a single Zombie on the heap and returns a pointer to it.
/// @param name The name for the new zombie.
/// @return Zombie* Heap-allocated zombie; caller must delete it.
Zombie* newZombie(std::string name) { return new Zombie(name); }
