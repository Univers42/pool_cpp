/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AForm.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:11:15 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX03_AFORM_HPP_
#define CPP_MODULE05_EX03_AFORM_HPP_

#include <exception>
#include <iostream>
#include <string>

// beSigned() and execute() only read a Bureaucrat through a reference, so a
// declaration is enough and the Bureaucrat <-> AForm include cycle never forms.
class Bureaucrat;

// ex01's Form, made abstract. The attributes stay private and stay here in the
// base, as the subject requires — a derived form reaches them through the
// getters like anyone else.
//
// Invariant: both required grades are always in [1, 150], where 1 is the
// highest. A form starts unsigned and can only ever move to signed.
//
// The split between execute() and executeAction() is the point of this class:
// execute() is public and NON-virtual, so the shared preconditions (signed,
// and the executor outranks the form) are checked in exactly one place and no
// derived class can forget them or skip them. executeAction() is protected and
// pure virtual, so each concrete form supplies only its own behaviour and
// cannot be called directly by outside code. That is the "more elegant" of the
// two options the subject offers — the alternative, repeating both checks in
// every concrete form, duplicates the rule three times and drifts on the
// fourth.
class AForm {
 public:
  class GradeTooHighException : public std::exception {
   public:
    virtual const char* what() const throw();
  };

  class GradeTooLowException : public std::exception {
   public:
    virtual const char* what() const throw();
  };

  class NotSignedException : public std::exception {
   public:
    virtual const char* what() const throw();
  };
  // Default constructor: an unnamed form with both grades at 150. No concrete
  // form reaches it — each names its own grades in its base initializer — but
  // OCF is required of every non-exception class in this module, so it is
  // public and defined rather than hidden.
  AForm();
  AForm(const std::string& name, int gradeToSign, int gradeToExecute);
  AForm(const AForm& other);
  AForm& operator=(const AForm& other);
  virtual ~AForm();

  const std::string& getName() const;
  bool isSigned() const;
  int getGradeToSign() const;
  int getGradeToExecute() const;
  void beSigned(const Bureaucrat& bureaucrat);
  void execute(const Bureaucrat& executor) const;

 protected:
  virtual void executeAction(const Bureaucrat& executor) const = 0;

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

std::ostream& operator<<(std::ostream& os, const AForm& form);

#endif  // CPP_MODULE05_EX03_AFORM_HPP_
