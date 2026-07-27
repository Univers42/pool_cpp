#ifndef CPP_MODULE05_EX03_BUREAUCRAT_HPP_
#define CPP_MODULE05_EX03_BUREAUCRAT_HPP_

#include <exception>
#include <iostream>
#include <string>

class AForm;

// Grade invariant: always between 1 (highest) and 150 (lowest).
class Bureaucrat {
 public:
  class GradeTooHighException : public std::exception {
   public:
    const char* what() const throw();
  };
  class GradeTooLowException : public std::exception {
   public:
    const char* what() const throw();
  };

  Bureaucrat();
  Bureaucrat(const std::string& name, int grade);
  Bureaucrat(const Bureaucrat& other);
  Bureaucrat& operator=(const Bureaucrat& other);
  ~Bureaucrat();

  std::string getName() const;
  int getGrade() const;

  void incrementGrade();
  void decrementGrade();

  void signForm(AForm& form);
  void executeForm(const AForm& form) const;

 private:
  const std::string _name;
  int _grade;

  void validateGrade(int grade) const;
};

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b);

#endif  // CPP_MODULE05_EX03_BUREAUCRAT_HPP_
