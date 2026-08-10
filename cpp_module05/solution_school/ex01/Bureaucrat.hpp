/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bureaucrat.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 19:37:22 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX01_BUREAUCRAT_HPP_
#define CPP_MODULE05_EX01_BUREAUCRAT_HPP_

#include <exception>
#include <iostream>
#include <string>

// signForm() only takes a Form by reference, so a declaration is all the
// compiler needs here. Including Form.hpp instead would be circular: Form.hpp
// needs Bureaucrat for beSigned(), and each header must stand on its own.
class Form;

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


  // Asks the form to be signed and reports the outcome on stdout. This is the
  // one member that swallows the exception instead of propagating it: the
  // subject asks for a printed reason, not a throw, so callers never need a
  // try/catch around it.
  void signForm(Form& form);

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

#endif  // CPP_MODULE05_EX01_BUREAUCRAT_HPP_
