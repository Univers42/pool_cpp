#ifndef CPP_MODULE05_EX03_ROBOTOMYREQUESTFORM_HPP_
#define CPP_MODULE05_EX03_ROBOTOMYREQUESTFORM_HPP_

#include <string>

#include "AForm.hpp"

// Sign 72 / exec 45: robotomizes <target>, 50% success.
class RobotomyRequestForm : public AForm {
 public:
  RobotomyRequestForm();
  explicit RobotomyRequestForm(const std::string& target);
  RobotomyRequestForm(const RobotomyRequestForm& other);
  RobotomyRequestForm& operator=(const RobotomyRequestForm& other);
  ~RobotomyRequestForm();

 protected:
  void executeAction(const Bureaucrat& executor) const;

 private:
  std::string _target;
};

#endif  // CPP_MODULE05_EX03_ROBOTOMYREQUESTFORM_HPP_
