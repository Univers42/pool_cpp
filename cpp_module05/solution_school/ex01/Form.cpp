/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Form.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:11:06 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Form.hpp"

// beSigned() reads the bureaucrat's grade, so it needs the complete type.
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
  EXCEPTIONS — the asymmetry in these two messages is deliberate.
  GradeTooHigh is only ever thrown by validateGrade(), so it can safely name
  the bound it checks. GradeTooLow has two throw sites that mean different
  things: a form grade above 150, and a signer who does not outrank the form.
  Naming "150" would be a lie in the second case, which is the one that ends up
  inside "<bureaucrat> couldn't sign <form> because <reason>." — so it stays
  generic and stays true.
*/
const char* Form::GradeTooHighException::what() const throw() { return ("grade is too high (1 is the highest)"); }
const char* Form::GradeTooLowException::what() const throw() { return ("grade is too low"); }

/**
  OCF — a form is born unsigned, always. validateGrade() runs *inside* the
  initializer list, so a bad grade throws before the const members exist: no
  form is ever born broken, and a half-built one never reaches a destructor.
  Members initialize in declaration order, so _gradeToSign is checked first.
*/
Form::Form() : _name("default"), _isSigned(false), _gradeToSign(150), _gradeToExecute(150) { TRACE("default constructor"); }
Form::Form(const std::string& name, int gradeToSign, int gradeToExecute) : _name(name), _isSigned(false), _gradeToSign(validateGrade(gradeToSign)), _gradeToExecute(validateGrade(gradeToExecute)) { TRACE("name+grades constructor"); }
Form::Form(const Form& other) : _name(other._name), _isSigned(other._isSigned), _gradeToSign(other._gradeToSign), _gradeToExecute(other._gradeToExecute) { TRACE("copy constructor"); }
Form::~Form() { TRACE("destructor"); }

// Name and both grades are const, so the signature is the only assignable part.
Form& Form::operator=(const Form& other) { if (this != &other) _isSigned = other._isSigned; return (*this); }

// GETTERS
const std::string& Form::getName() const { return (_name); }
bool Form::isSigned() const { return (_isSigned); }
int Form::getGradeToSign() const { return (_gradeToSign); }
int Form::getGradeToExecute() const { return (_gradeToExecute); }

// SIGNING
// Lower number = higher rank, so the test is `>`: grade 50 signs a form that
// requires 50, grade 51 does not. The throw happens before the assignment, so
// a refused signature leaves the form exactly as it was.
void Form::beSigned(const Bureaucrat& bureaucrat) {
  if (bureaucrat.getGrade() > _gradeToSign) throw GradeTooLowException();
  _isSigned = true;
}

// The only real branching in the class, so it keeps its own lines.
int Form::validateGrade(int grade) {
  if (grade < 1) throw GradeTooHighException();
  if (grade > 150) throw GradeTooLowException();
  return (grade);
}

std::ostream& operator<<(std::ostream& os, const Form& form) { return (os << "form " << form.getName() << ", signed: " << (form.isSigned() ? "yes" : "no") << ", grade to sign: " << form.getGradeToSign() << ", grade to execute: " << form.getGradeToExecute() << "."); }
