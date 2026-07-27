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

const char* Bureaucrat::GradeTooHighException::what() const throw() {
  return "Grade too high (highest is 1)";
}

const char* Bureaucrat::GradeTooLowException::what() const throw() {
  return "Grade too low (lowest is 150)";
}

Bureaucrat::Bureaucrat() : _name("Default"), _grade(150) {}

Bureaucrat::Bureaucrat(const std::string& name, int grade)
    : _name(name), _grade(grade) {
  validateGrade(grade);
}

Bureaucrat::Bureaucrat(const Bureaucrat& other)
    : _name(other._name), _grade(other._grade) {}

// _name is const, so only the grade can be assigned.
Bureaucrat& Bureaucrat::operator=(const Bureaucrat& other) {
  if (this != &other) _grade = other._grade;
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

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b) {
  os << b.getName() << ", bureaucrat grade " << b.getGrade() << ".";
  return os;
}
