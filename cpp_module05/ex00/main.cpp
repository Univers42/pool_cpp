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

int main() {
  // Incrementing walks toward grade 1, then overflows.
  try {
    Bureaucrat alice("Alice", 3);
    std::cout << alice << std::endl;
    alice.incrementGrade();
    std::cout << alice << std::endl;
    alice.incrementGrade();
    std::cout << alice << std::endl;
    alice.incrementGrade();  // grade is 1: throws GradeTooHighException
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << std::endl;
  }

  // Decrementing past 150 throws GradeTooLowException.
  try {
    Bureaucrat bob("Bob", 150);
    std::cout << bob << std::endl;
    bob.decrementGrade();
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << std::endl;
  }

  // Constructing with an out-of-range grade throws too.
  try {
    Bureaucrat overachiever("Overachiever", 0);
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << std::endl;
  }
  try {
    Bureaucrat slacker("Slacker", 151);
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << std::endl;
  }
  return 0;
}
