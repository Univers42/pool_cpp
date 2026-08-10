/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Form.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:11:15 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX01_FORM_HPP_
#define CPP_MODULE05_EX01_FORM_HPP_

#include <exception>
#include <iostream>
#include <string>

// beSigned() only reads a Bureaucrat through a reference, so a declaration is
// enough and the Bureaucrat <-> Form include cycle never forms.
class Bureaucrat;

// Invariant: both required grades are always in [1, 150], where 1 is the
// highest — the same scale as Bureaucrat. A Form starts unsigned and can only
// ever move to signed, so `_isSigned` is the single mutable piece of state.
class Form {
 public:
  class GradeTooHighException : public std::exception {
   public:
    virtual const char* what() const throw();
  };

  class GradeTooLowException : public std::exception {
   public:
    virtual const char* what() const throw();
  };

  Form();
  Form(const std::string& name, int gradeToSign, int gradeToExecute);
  Form(const Form& other);
  Form& operator=(const Form& other);
  ~Form();


  const std::string& getName() const;
  bool isSigned() const;
  int getGradeToSign() const;
  int getGradeToExecute() const;


  // Signs the form if `bureaucrat` ranks at or above _gradeToSign, throws
  // GradeTooLowException otherwise. Signing twice is a harmless no-op.
  void beSigned(const Bureaucrat& bureaucrat);


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
  bool _isSigned;
  const int _gradeToSign;
  const int _gradeToExecute;
};

std::ostream& operator<<(std::ostream& os, const Form& form);

#endif  // CPP_MODULE05_EX01_FORM_HPP_
