/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AForm.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:11:06 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AForm.hpp"

#include "Bureaucrat.hpp"

const char* AForm::GradeTooHighException::what() const throw() {
  return "form grade is too high";
}

const char* AForm::GradeTooLowException::what() const throw() {
  return "form grade is too low";
}

const char* AForm::NotSignedException::what() const throw() {
  return "form is not signed";
}

AForm::AForm()
    : _name("default"), _isSigned(false), _gradeToSign(150),
      _gradeToExecute(150) {}

AForm::AForm(const std::string& name, int gradeToSign, int gradeToExecute)
    : _name(name), _isSigned(false), _gradeToSign(gradeToSign),
      _gradeToExecute(gradeToExecute) {
  if (gradeToSign < 1 || gradeToExecute < 1) throw GradeTooHighException();
  if (gradeToSign > 150 || gradeToExecute > 150) throw GradeTooLowException();
}

AForm::AForm(const AForm& other)
    : _name(other._name), _isSigned(other._isSigned),
      _gradeToSign(other._gradeToSign), _gradeToExecute(other._gradeToExecute) {
}

AForm& AForm::operator=(const AForm& other) {
  // _name and grades are const; only the signed flag is assignable
  if (this != &other) _isSigned = other._isSigned;
  return *this;
}

AForm::~AForm() {}

const std::string& AForm::getName() const { return _name; }

bool AForm::isSigned() const { return _isSigned; }

int AForm::getGradeToSign() const { return _gradeToSign; }

int AForm::getGradeToExecute() const { return _gradeToExecute; }

void AForm::beSigned(const Bureaucrat& bureaucrat) {
  // lower number = higher rank
  if (bureaucrat.getGrade() > _gradeToSign) throw GradeTooLowException();
  _isSigned = true;
}

void AForm::execute(const Bureaucrat& executor) const {
  if (!_isSigned) throw NotSignedException();
  if (executor.getGrade() > _gradeToExecute) throw GradeTooLowException();
  executeAction(executor);
}

std::ostream& operator<<(std::ostream& os, const AForm& form) {
  os << "form " << form.getName() << ", signed: "
     << (form.isSigned() ? "yes" : "no")
     << ", grade to sign: " << form.getGradeToSign()
     << ", grade to execute: " << form.getGradeToExecute();
  return os;
}
