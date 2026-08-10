/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bureaucrat.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 19:37:22 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX03_BUREAUCRAT_HPP_
#define CPP_MODULE05_EX03_BUREAUCRAT_HPP_

#include <exception>
#include <iostream>
#include <string>

// signForm() and executeForm() only take an AForm by reference, so a
// declaration is all the compiler needs here. Including AForm.hpp instead
// would be circular: AForm.hpp needs Bureaucrat for beSigned() and execute(),
// and each header must stand on its own.
class AForm;

// Invariant: a Bureaucrat's grade is always in [1, 150], where 1 is the
// highest grade. Every constructor and mutation enforces it by throwing.
class Bureaucrat {
 public:
  class GradeTooHighException : public std::exception {
   public:
    virtual const char* what() const throw();
  };

  class GradeTooLowException : public std::exception {
   public:
    virtual const char* what() const throw();
  };
  Bureaucrat();
  explicit Bureaucrat(const std::string& name);
  Bureaucrat(const std::string& name, int grade);
  Bureaucrat(const Bureaucrat& other);
  Bureaucrat& operator=(const Bureaucrat& other);
  ~Bureaucrat();
  std::string getName() const;
  int getGrade() const;

  Bureaucrat& operator++();
  Bureaucrat operator++(int);
  Bureaucrat& operator--();
  Bureaucrat operator--(int);
  void incrementGrade();
  void decrementGrade();
  void signForm(AForm& form);
  void executeForm(const AForm& form) const;

 private:
  // ── WHY THIS IS `static` ───────────────────────────────────────────────────
  // The honest version first: it WOULD work without `static`. A non-static
  // member function may legally be called from a member-initialiser list, and
  // this one only ever reads its own parameter, so dropping `static` still
  // compiles and every test still passes. `static` is not what makes the code
  // run — verified, not assumed.
  //
  // What `static` buys is that the DANGEROUS version cannot be written. This
  // function is called from inside the initialiser list:
  //
  //     Foo(...) : _a(name), _b(validateGrade(grade)) {}
  //
  // At that moment the members do not exist yet. If validateGrade could touch
  // them, a one-line edit like `if (_grade < 1) ...` would read an
  // uninitialised member: undefined behaviour, silently, with a clean build.
  // A static member function has no `this` at all, so that same edit becomes a
  // compile error —
  //     "invalid use of member '_grade' in static member function"
  //
  // So `static` does not make this work; it makes the bug impossible. It also
  // states the truth about the function: it is a pure check on its argument,
  // independent of any particular object, and could be called before a single
  // one exists.
  //
  // Why a static MEMBER and not a free function in the .cpp: it throws this
  // class's own nested exception types, so as a member it names them
  // unqualified and has access to them. And it is private, so it stays an
  // implementation detail instead of leaking into the public interface.
  static int validateGrade(int grade);
  const std::string _name;
  int _grade;
};

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b);

#endif  // CPP_MODULE05_EX03_BUREAUCRAT_HPP_
