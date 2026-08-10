/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AForm.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:11:06 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AForm.hpp"

// beSigned() and execute() read the bureaucrat's grade, so they need the
// complete type.
#include "Bureaucrat.hpp"

// One expression instead of a 4-line #ifdef block inside every function, so the
// bodies below stay on a single line. With DEBUG off it collapses to nothing.
// Member functions only: it prints `this`.
#ifdef DEBUG
#define TRACE(what) (std::cout << (what) << " " << this << std::endl)
#else
#define TRACE(what) ((void)0)
#endif

/**
  EXCEPTIONS — the asymmetry in these messages is deliberate.
  GradeTooHigh is only ever thrown by validateGrade(), so it can safely name
  the bound it checks. GradeTooLow has three throw sites that mean different
  things: a form grade above 150, a signer who does not outrank the form, and
  an executor who does not reach the execution grade. Naming "150" would be a
  lie in the last two, which are the ones that end up inside "<bureaucrat>
  couldn't sign/execute <form> because <reason>." — so it stays generic and
  stays true.
*/
const char* AForm::GradeTooHighException::what() const throw() { return ("grade is too high (1 is the highest)"); }
const char* AForm::GradeTooLowException::what() const throw() { return ("grade is too low"); }
const char* AForm::NotSignedException::what() const throw() { return ("the form is not signed"); }

/**
  OCF — a form is born unsigned, always. validateGrade() runs *inside* the
  initializer list, so a bad grade throws before the const members exist: no
  form is ever born broken, and a half-built one never reaches a destructor.
  Members initialize in declaration order, so _gradeToSign is checked first.
*/
AForm::AForm() : _name("default"), _isSigned(false), _gradeToSign(150), _gradeToExecute(150) { TRACE("default constructor"); }
AForm::AForm(const std::string& name, int gradeToSign, int gradeToExecute) : _name(name), _isSigned(false), _gradeToSign(validateGrade(gradeToSign)), _gradeToExecute(validateGrade(gradeToExecute)) { TRACE("name+grades constructor"); }
AForm::AForm(const AForm& other) : _name(other._name), _isSigned(other._isSigned), _gradeToSign(other._gradeToSign), _gradeToExecute(other._gradeToExecute) { TRACE("copy constructor"); }
AForm::~AForm() { TRACE("destructor"); }

// Name and both grades are const, so the signature is the only assignable part.
AForm& AForm::operator=(const AForm& other) { if (this != &other) _isSigned = other._isSigned; return (*this); }

// GETTERS
const std::string& AForm::getName() const { return (_name); }
bool AForm::isSigned() const { return (_isSigned); }
int AForm::getGradeToSign() const { return (_gradeToSign); }
int AForm::getGradeToExecute() const { return (_gradeToExecute); }

// SIGNING
// Lower number = higher rank, so the test is `>`: grade 50 signs a form that
// requires 50, grade 51 does not. The throw happens before the assignment, so
// a refused signature leaves the form exactly as it was.
void AForm::beSigned(const Bureaucrat& bureaucrat) {
  if (bureaucrat.getGrade() > _gradeToSign) throw GradeTooLowException();
  _isSigned = true;
}

// EXECUTION — the gate. Both preconditions live here and nowhere else, so no
// concrete form can forget one. Order matters for the diagnostic: an unsigned
// form reports that it is unsigned even when the executor also outranks
// nothing, because "not signed" is the more actionable of the two.
void AForm::execute(const Bureaucrat& executor) const {
  if (!_isSigned) throw NotSignedException();
  if (executor.getGrade() > _gradeToExecute) throw GradeTooLowException();
  executeAction(executor);
}

// The only real branching in the class, so it keeps its own lines.
int AForm::validateGrade(int grade) {
  if (grade < 1) throw GradeTooHighException();
  if (grade > 150) throw GradeTooLowException();
  return (grade);
}

std::ostream& operator<<(std::ostream& os, const AForm& form) { return (os << "form " << form.getName() << ", signed: " << (form.isSigned() ? "yes" : "no") << ", grade to sign: " << form.getGradeToSign() << ", grade to execute: " << form.getGradeToExecute() << "."); }
