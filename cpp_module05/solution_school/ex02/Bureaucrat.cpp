/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bureaucrat.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 19:37:25 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bureaucrat.hpp"

// The forward declaration in the header is enough to *declare* the two form
// members, but calling beSigned()/execute()/getName() needs the complete type.
#include "AForm.hpp"

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

// ── HOW `catch (std::exception& e)` FINDS THE RIGHT ERROR ────────────────────
//
// 1. WHAT IS THROWN. `throw GradeTooLowException();` constructs a temporary
//    exception OBJECT and hands it to the runtime. Nothing is returned and the
//    rest of the throwing function never runs.
//
// 2. STACK UNWINDING. The runtime walks back up the call stack looking for a
//    handler, destroying every local object it passes on the way — which is
//    why a throw cannot leak a stack object. It stops at the FIRST `catch`
//    whose type matches.
//
// 3. WHY A BASE-CLASS HANDLER MATCHES. Each exception class derives PUBLICLY
//    from std::exception, and a handler written for a base class accepts any
//    derived object. So this one handler covers GradeTooHigh, GradeTooLow,
//    NotSigned, and even the std::runtime_error a concrete form throws when a
//    file will not open — including exception types that do not exist yet.
//    That public inheritance is the entire mechanism; drop it and this handler
//    silently stops seeing the exception (tests/mutants.sh proves exactly
//    that: see the "stops deriving from std::exception" mutants).
//
// 4. HOW THE SPECIFIC MESSAGE SURVIVES. `e` is a std::exception REFERENCE, but
//    the object it refers to is still the derived one. `what()` is virtual, so
//    `e.what()` dispatches to the derived override and yields "grade is too
//    low" rather than std::exception's generic text.
//
//    This is why the `&` is not decoration. `catch (std::exception e)` — by
//    value — SLICES: it copies only the std::exception sub-object, throws the
//    derived part away, and what() then returns the useless base message.
//    Catch exceptions by reference, always.
//
// 5. WHAT THIS DOES NOT DO. It does not tell you WHICH exception arrived; it
//    only gets you that exception's own message. To branch on the type you
//    list the specific handlers FIRST, because handlers are tried in order and
//    the first match wins:
//
//        catch (AForm::NotSignedException&) { ... }   // most derived first
//        catch (std::exception&)            { ... }   // catch-all last
//
//    A base handler placed first would swallow everything under it. Here we
//    deliberately do NOT branch: every failure prints the same sentence with a
//    different reason, so one handler is exactly the right shape.

// FORMS
// AForm is the one that decides and throws; these two only report. Catching by
// std::exception& (rather than each concrete exception) keeps the reason
// generic, so a new refusal reason in AForm needs no change here — and it also
// catches whatever a concrete form's action throws, which is why executeForm
// cannot leak a std::runtime_error from ShrubberyCreationForm either.
void Bureaucrat::signForm(AForm& form) {
  try {
    form.beSigned(*this);
    std::cout << _name << " signed " << form.getName() << std::endl;
  } catch (std::exception& e) {
    std::cout << _name << " couldn't sign " << form.getName() << " because " << e.what() << "." << std::endl;
  }
}

void Bureaucrat::executeForm(const AForm& form) const {
  try {
    form.execute(*this);
    std::cout << _name << " executed " << form.getName() << std::endl;
  } catch (std::exception& e) {
    std::cout << _name << " couldn't execute " << form.getName() << " because " << e.what() << "." << std::endl;
  }
}

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b) { return (os << b.getName() << ", bureaucrat grade " << b.getGrade() << "."); }
