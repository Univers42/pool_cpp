/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RobotomyRequestForm.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:27:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RobotomyRequestForm.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>

#ifdef DEBUG
#define TRACE(what) (std::cout << (what) << " " << this << std::endl)
#else
#define TRACE(what) ((void)0)
#endif

// Name and grades fixed by the subject: sign 72, exec 45. No default ctor —
// the header says why.
RobotomyRequestForm::RobotomyRequestForm() : AForm("RobotomyRequestForm", 72, 45), _target("default") { TRACE("default constructor"); }
RobotomyRequestForm::RobotomyRequestForm(const std::string& target) : AForm("RobotomyRequestForm", 72, 45), _target(target) { TRACE("target constructor"); }
RobotomyRequestForm::RobotomyRequestForm(const RobotomyRequestForm& other) : AForm(other), _target(other._target) { TRACE("copy constructor"); }
RobotomyRequestForm::~RobotomyRequestForm() { TRACE("destructor"); }

RobotomyRequestForm& RobotomyRequestForm::operator=(const RobotomyRequestForm& other) { if (this != &other) { AForm::operator=(other); _target = other._target; } return (*this); }

// execute() has already checked signature and grade, so this only has to make
// noise and flip a coin.
void RobotomyRequestForm::executeAction(const Bureaucrat& executor) const {
  (void)executor;
  // ponytail: std::rand is the only C++98 RNG; seed once, 50% is all we need.
  // Seeding lazily (rather than in a constructor) keeps the first roll random
  // even when the only form built is a copy.
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
