/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 19:37:29 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Bureaucrat.hpp"
#include "PresidentialPardonForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "ShrubberyCreationForm.hpp"

int main() {
  Bureaucrat alice("Alice", 50);
  Bureaucrat bob("Bob", 1);
  Bureaucrat tom("Tom", 150);

  ShrubberyCreationForm shrub("home");
  RobotomyRequestForm robo("Bender");
  PresidentialPardonForm pardon("Ford");

  std::cout << "=== Initial state ===" << std::endl;
  std::cout << shrub << std::endl << robo << std::endl << pardon << std::endl;

  std::cout << std::endl << "=== Signing ===" << std::endl;
  alice.signForm(shrub);   // ok: 50 <= 145
  alice.signForm(robo);    // ok: 50 <= 72
  alice.signForm(pardon);  // fails: needs 25
  tom.signForm(robo);      // fails: needs 72

  std::cout << std::endl << "=== Executing ===" << std::endl;
  alice.executeForm(shrub);  // ok: 50 <= 137, creates home_shrubbery
  alice.executeForm(robo);   // fails: needs 45
  bob.executeForm(robo);     // drills, 50% success
  bob.executeForm(pardon);   // fails: not signed
  bob.signForm(pardon);
  bob.executeForm(pardon);  // ok: pardoned by Zaphod

  return 0;
}
