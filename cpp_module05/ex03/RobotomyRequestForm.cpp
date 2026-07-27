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

void RobotomyRequestForm::executeAction(const Bureaucrat&) const {
  // ponytail: std::rand is the only C++98 RNG; seed once, 50% is all we need.
  static bool seeded = false;
  if (!seeded) {
    std::srand(static_cast<unsigned>(std::time(NULL)));
    seeded = true;
  }
  std::cout << "Bzzzz... Vrrrrrr... (drilling noises)" << std::endl;
  if (std::rand() % 2)
    std::cout << _target << " has been robotomized successfully." << std::endl;
  else
    std::cout << "Robotomy failed on " << _target << "." << std::endl;
}
