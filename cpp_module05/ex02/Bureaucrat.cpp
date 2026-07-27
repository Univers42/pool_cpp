/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bureaucrat.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 19:37:25 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bureaucrat.hpp"

#include "AForm.hpp"

const char* Bureaucrat::GradeTooHighException::what() const throw() {
  return "grade is too high";
}

const char* Bureaucrat::GradeTooLowException::what() const throw() {
  return "grade is too low";
}

Bureaucrat::Bureaucrat() : _name("default"), _grade(150) {}

Bureaucrat::Bureaucrat(const std::string& name, int grade)
    : _name(name), _grade(grade) {
  validateGrade(grade);
}

Bureaucrat::Bureaucrat(const Bureaucrat& other)
    : _name(other._name), _grade(other._grade) {}

Bureaucrat& Bureaucrat::operator=(const Bureaucrat& other) {
  // _name is const; only the grade is assignable
  if (this != &other) _grade = other._grade;
  return *this;
}

Bureaucrat::~Bureaucrat() {}

const std::string& Bureaucrat::getName() const { return _name; }

int Bureaucrat::getGrade() const { return _grade; }

void Bureaucrat::incrementGrade() {
  validateGrade(_grade - 1);
  --_grade;
}

void Bureaucrat::decrementGrade() {
  validateGrade(_grade + 1);
  ++_grade;
}

void Bureaucrat::validateGrade(int grade) {
  if (grade < 1) throw GradeTooHighException();
  if (grade > 150) throw GradeTooLowException();
}

void Bureaucrat::signForm(AForm& form) {
  try {
    form.beSigned(*this);
    std::cout << _name << " signed " << form.getName() << std::endl;
  } catch (std::exception& e) {
    std::cout << _name << " couldn't sign " << form.getName() << " because "
              << e.what() << "." << std::endl;
  }
}

void Bureaucrat::executeForm(const AForm& form) const {
  try {
    form.execute(*this);
    std::cout << _name << " executed " << form.getName() << std::endl;
  } catch (std::exception& e) {
    std::cout << _name << " couldn't execute " << form.getName() << " because "
              << e.what() << "." << std::endl;
  }
}

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b) {
  os << b.getName() << ", bureaucrat grade " << b.getGrade() << ".";
  return os;
}
