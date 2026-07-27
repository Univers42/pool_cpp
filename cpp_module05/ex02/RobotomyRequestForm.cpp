/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RobotomyRequestForm.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:27:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RobotomyRequestForm.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>

RobotomyRequestForm::RobotomyRequestForm()
    : AForm("RobotomyRequestForm", 72, 45), _target("default") {}

RobotomyRequestForm::RobotomyRequestForm(const std::string& target)
    : AForm("RobotomyRequestForm", 72, 45), _target(target) {}

RobotomyRequestForm::RobotomyRequestForm(const RobotomyRequestForm& other)
    : AForm(other), _target(other._target) {}

RobotomyRequestForm& RobotomyRequestForm::operator=(
    const RobotomyRequestForm& other) {
  if (this != &other) {
    AForm::operator=(other);
    _target = other._target;
  }
  return *this;
}

RobotomyRequestForm::~RobotomyRequestForm() {}

void RobotomyRequestForm::executeAction(const Bureaucrat& /*executor*/) const {
  // ponytail: rand() is plenty for a coin flip; <random> is C++11 anyway
  static bool seeded = false;
  if (!seeded) {
    std::srand(static_cast<unsigned int>(std::time(0)));
    seeded = true;
  }
  std::cout << "* drilling noises *" << std::endl;
  if (std::rand() % 2)
    std::cout << _target << " has been robotomized successfully" << std::endl;
  else
    std::cout << "robotomy of " << _target << " failed" << std::endl;
}
