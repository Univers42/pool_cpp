/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   randomChump.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:06:01 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 19:57:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include "Zombie.hpp"

/// @brief Creates a stack-allocated Zombie, calls announce(), then destroys it.
/// @param name The name for the temporary zombie.
void randomChump(std::string name) {
  Zombie stackZombie(name);
  stackZombie.announce();
}
