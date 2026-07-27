#ifndef CPP_MODULE05_EX03_PRESIDENTIALPARDONFORM_HPP_
#define CPP_MODULE05_EX03_PRESIDENTIALPARDONFORM_HPP_

#include <string>

#include "AForm.hpp"

// Sign 25 / exec 5: pardons <target> in Zaphod Beeblebrox's name.
class PresidentialPardonForm : public AForm {
 public:
  PresidentialPardonForm();
  explicit PresidentialPardonForm(const std::string& target);
  PresidentialPardonForm(const PresidentialPardonForm& other);
  PresidentialPardonForm& operator=(const PresidentialPardonForm& other);
  ~PresidentialPardonForm();

 protected:
  void executeAction(const Bureaucrat& executor) const;

 private:
  std::string _target;
};

#endif  // CPP_MODULE05_EX03_PRESIDENTIALPARDONFORM_HPP_
