/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:00:20 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:55:52 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Zombie.hpp"

#ifdef TESTS
/**
 * @brief Extended test suite — compile with -DTESTS to enable.
 */
int main(void) {
  int N = 5;

  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 1. CREATING THE HORDE ===" << C_RESET << std::endl;
  std::cout << C_YELLOW << "-> Calling zombieHorde(" << N << ", \"Minion\")"
            << C_RESET << std::endl;

  Zombie* horde = zombieHorde(N, "Minion");

  if (!horde) {
    std::cout << C_RED << "Failed to create horde!" << C_RESET << std::endl;
    return 1;
  }

  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 2. THE HORDE ANNOUNCES ITSELF ===" << C_RESET
            << std::endl;

  for (int i = 0; i < N; i++) {
    std::cout << C_DIM << "Zombie " << i << ": " << C_RESET;
    horde[i].announce();
  }

  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 3. DESTROYING THE HORDE ===" << C_RESET << std::endl;
  std::cout << C_YELLOW << "-> Calling delete[] horde" << C_RESET << std::endl;

  delete[] horde;

  std::cout << C_BOLD << C_MAGENTA << "\n=== 4. EDGE CASE: N = 0 ===" << C_RESET
            << std::endl;
  Zombie* zeroHorde = zombieHorde(0, "Nobody");
  if (zeroHorde == NULL) {
    std::cout << C_GREEN << "Successfully handled N = 0 by returning NULL."
              << C_RESET << std::endl;
  }

  std::cout << C_BOLD << C_MAGENTA << "\n=== END OF TESTS ===\n"
            << C_RESET << std::endl;

  return 0;
}

#else
/**
 * @brief Default main — creates a horde, announces, and destroys it.
 */
int main(void) {
  int N = 5;
  Zombie* horde = zombieHorde(N, "Minion");

  if (!horde)
    return 1;

  for (int i = 0; i < N; i++)
    horde[i].announce();

  delete[] horde;
  return 0;
}
#endif
