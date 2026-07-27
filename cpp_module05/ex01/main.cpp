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
#include "Form.hpp"

int main() {
  std::cout << "--- sign: too low, then high enough ---" << std::endl;
  Bureaucrat alice("Alice", 3);
  Form top("TopSecret", 2, 1);
  std::cout << alice << std::endl;
  std::cout << top << std::endl;
  alice.signForm(top);      // grade 3 < required 2: refused
  alice.incrementGrade();   // now grade 2
  alice.signForm(top);      // equal grade: signed
  alice.signForm(top);      // already signed: signs again, harmless
  std::cout << top << std::endl;

  std::cout << "--- form with out-of-bounds grade ---" << std::endl;
  try {
    Form bad("BadForm", 0, 10);
  } catch (std::exception& e) {
    std::cout << "caught: " << e.what() << std::endl;
  }
  try {
    Form bad("BadForm", 10, 151);
  } catch (std::exception& e) {
    std::cout << "caught: " << e.what() << std::endl;
  }
  return 0;
}
