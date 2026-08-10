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

// One expression instead of a 4-line #ifdef block inside every function, so the
// bodies below stay on a single line. With DEBUG off it collapses to nothing.
// Member functions only: it prints `this`.
#ifdef DEBUG
#define TRACE(what) (std::cout << (what) << " " << this << std::endl)
#else
#define TRACE(what) ((void)0)
#endif

/**
  EXCEPTIONS
*/
const char* Bureaucrat::GradeTooHighException::what() const throw() { return ("Grade too high (highest is 1)"); }
const char* Bureaucrat::GradeTooLowException::what() const throw() { return ("Grade too low (lowest is 150)"); }

/**
  CFO — built without an explicit grade, a bureaucrat starts at the bottom (150),
  which is always valid. validateGrade() runs *inside* the initializer list, so
  a bad grade throws before _grade exists: no object is ever born broken.
*/
Bureaucrat::Bureaucrat() : _name("Default"), _grade(150) { TRACE("default constructor"); }
Bureaucrat::Bureaucrat(const std::string& name) : _name(name), _grade(150) { TRACE("name-only constructor"); }
Bureaucrat::Bureaucrat(const std::string& name, int grade) : _name(name), _grade(validateGrade(grade)) { TRACE("name+grade constructor"); }
Bureaucrat::Bureaucrat(const Bureaucrat& other) : _name(other._name), _grade(validateGrade(other._grade)) { TRACE("copy constructor"); }
Bureaucrat::~Bureaucrat() { TRACE("destructor"); }

// _name is const, so only the grade can be assigned.
Bureaucrat& Bureaucrat::operator=(const Bureaucrat& other) { if (this != &other) _grade = validateGrade(other._grade); return (*this); }

// GETTERS
std::string Bureaucrat::getName() const { return (_name); }
int Bureaucrat::getGrade() const { return (_grade); }

// INCREMENT DECREMENT
// Grade 1 is the highest, so a promotion *lowers* the number. validateGrade()
// throws before the assignment, which gives the strong guarantee: a refused
// promotion leaves the bureaucrat exactly as it was.
void Bureaucrat::incrementGrade() { _grade = validateGrade(_grade - 1); }
void Bureaucrat::decrementGrade() { _grade = validateGrade(_grade + 1); }

// Pure sugar over the two functions above: the bounds live in one place only,
// so the two APIs can never drift apart. Prefix hands back the object itself
// (the comma operator discards the void call and yields *this as an lvalue);
// postfix copies first, so a throwing mutation leaves both tmp and *this sane.
Bureaucrat& Bureaucrat::operator++() { return (incrementGrade(), *this); }
Bureaucrat& Bureaucrat::operator--() { return (decrementGrade(), *this); }
Bureaucrat Bureaucrat::operator++(int) { Bureaucrat tmp(*this); incrementGrade(); return (tmp); }
Bureaucrat Bureaucrat::operator--(int) { Bureaucrat tmp(*this); decrementGrade(); return (tmp); }

// The only real branching in the class, so it keeps its own lines.
int Bureaucrat::validateGrade(int grade) {
  if (grade < 1) throw GradeTooHighException();
  if (grade > 150) throw GradeTooLowException();
  return (grade);
}

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b) { return (os << b.getName() << ", bureaucrat grade " << b.getGrade() << "."); }
