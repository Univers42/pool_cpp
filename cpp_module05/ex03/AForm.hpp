#ifndef CPP_MODULE05_EX03_AFORM_HPP_
#define CPP_MODULE05_EX03_AFORM_HPP_

#include <exception>
#include <iostream>
#include <string>

class Bureaucrat;

// Abstract base: sign/execute grade checks live here, concrete forms only
// provide executeAction().
class AForm {
 public:
  class GradeTooHighException : public std::exception {
   public:
    const char* what() const throw();
  };
  class GradeTooLowException : public std::exception {
   public:
    const char* what() const throw();
  };
  class NotSignedException : public std::exception {
   public:
    const char* what() const throw();
  };

  AForm();
  AForm(const std::string& name, int gradeToSign, int gradeToExecute);
  AForm(const AForm& other);
  AForm& operator=(const AForm& other);
  virtual ~AForm();

  std::string getName() const;
  bool isSigned() const;
  int getGradeToSign() const;
  int getGradeToExecute() const;

  void beSigned(const Bureaucrat& bureaucrat);
  void execute(const Bureaucrat& executor) const;

 protected:
  // Called by execute() once the signed/grade checks passed.
  virtual void executeAction(const Bureaucrat& executor) const = 0;

 private:
  const std::string _name;
  bool _isSigned;
  const int _gradeToSign;
  const int _gradeToExecute;
};

std::ostream& operator<<(std::ostream& os, const AForm& form);

#endif  // CPP_MODULE05_EX03_AFORM_HPP_
