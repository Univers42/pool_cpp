#ifndef CPP_MODULE05_EX03_SHRUBBERYCREATIONFORM_HPP_
#define CPP_MODULE05_EX03_SHRUBBERYCREATIONFORM_HPP_

#include <string>

#include "AForm.hpp"

// Sign 145 / exec 137: writes ASCII trees to <target>_shrubbery.
class ShrubberyCreationForm : public AForm {
 public:
  ShrubberyCreationForm();
  explicit ShrubberyCreationForm(const std::string& target);
  ShrubberyCreationForm(const ShrubberyCreationForm& other);
  ShrubberyCreationForm& operator=(const ShrubberyCreationForm& other);
  ~ShrubberyCreationForm();

 protected:
  void executeAction(const Bureaucrat& executor) const;

 private:
  std::string _target;
};

#endif  // CPP_MODULE05_EX03_SHRUBBERYCREATIONFORM_HPP_
