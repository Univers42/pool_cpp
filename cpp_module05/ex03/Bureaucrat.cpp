#include "Bureaucrat.hpp"

#include "AForm.hpp"

const char* Bureaucrat::GradeTooHighException::what() const throw() {
  return "Grade is too high. Maximum grade is 1.";
}

const char* Bureaucrat::GradeTooLowException::what() const throw() {
  return "Grade is too low. Minimum grade is 150.";
}

Bureaucrat::Bureaucrat() : _name("Default"), _grade(150) {}

Bureaucrat::Bureaucrat(const std::string& name, int grade)
    : _name(name), _grade(grade) {
  validateGrade(grade);
}

Bureaucrat::Bureaucrat(const Bureaucrat& other)
    : _name(other._name), _grade(other._grade) {}

Bureaucrat& Bureaucrat::operator=(const Bureaucrat& other) {
  if (this != &other) _grade = other._grade;  // _name is const
  return *this;
}

Bureaucrat::~Bureaucrat() {}

std::string Bureaucrat::getName() const { return _name; }

int Bureaucrat::getGrade() const { return _grade; }

void Bureaucrat::incrementGrade() {
  validateGrade(_grade - 1);
  --_grade;
}

void Bureaucrat::decrementGrade() {
  validateGrade(_grade + 1);
  ++_grade;
}

void Bureaucrat::validateGrade(int grade) const {
  if (grade < 1) throw GradeTooHighException();
  if (grade > 150) throw GradeTooLowException();
}

void Bureaucrat::signForm(AForm& form) {
  try {
    form.beSigned(*this);
    std::cout << _name << " signed " << form.getName() << std::endl;
  } catch (std::exception& e) {
    std::cout << _name << " couldn't sign " << form.getName() << " because "
              << e.what() << std::endl;
  }
}

void Bureaucrat::executeForm(const AForm& form) const {
  try {
    form.execute(*this);
    std::cout << _name << " executed " << form.getName() << std::endl;
  } catch (std::exception& e) {
    std::cout << _name << " couldn't execute " << form.getName() << " because "
              << e.what() << std::endl;
  }
}

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b) {
  os << b.getName() << ", bureaucrat grade " << b.getGrade() << ".";
  return os;
}
