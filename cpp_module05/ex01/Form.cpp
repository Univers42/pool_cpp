/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Form.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:11:06 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Form.hpp"

#include "Bureaucrat.hpp"

const char* Form::GradeTooHighException::what() const throw() {
  return "form grade is too high";
}

const char* Form::GradeTooLowException::what() const throw() {
  return "form grade is too low";
}

Form::Form()
    : _name("default"), _isSigned(false), _gradeToSign(150),
      _gradeToExecute(150) {}

Form::Form(const std::string& name, int gradeToSign, int gradeToExecute)
    : _name(name), _isSigned(false), _gradeToSign(gradeToSign),
      _gradeToExecute(gradeToExecute) {
  if (gradeToSign < 1 || gradeToExecute < 1) throw GradeTooHighException();
  if (gradeToSign > 150 || gradeToExecute > 150) throw GradeTooLowException();
}

Form::Form(const Form& other)
    : _name(other._name), _isSigned(other._isSigned),
      _gradeToSign(other._gradeToSign), _gradeToExecute(other._gradeToExecute) {
}

Form& Form::operator=(const Form& other) {
  // name and grades are const; only the signed flag is assignable
  if (this != &other) _isSigned = other._isSigned;
  return *this;
}

Form::~Form() {}

const std::string& Form::getName() const { return _name; }

bool Form::isSigned() const { return _isSigned; }

int Form::getGradeToSign() const { return _gradeToSign; }

int Form::getGradeToExecute() const { return _gradeToExecute; }

void Form::beSigned(const Bureaucrat& bureaucrat) {
  // lower number = higher rank; equal grade is enough
  if (bureaucrat.getGrade() > _gradeToSign) throw GradeTooLowException();
  _isSigned = true;
}

std::ostream& operator<<(std::ostream& os, const Form& form) {
  os << "form " << form.getName() << ", signed: "
     << (form.isSigned() ? "yes" : "no")
     << ", grade to sign: " << form.getGradeToSign()
     << ", grade to execute: " << form.getGradeToExecute();
  return os;
}
