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

#ifndef CPP_MODULE05_EX00_BUREAUCRAT_HPP_
#define CPP_MODULE05_EX00_BUREAUCRAT_HPP_

#include <exception>
#include <iostream>
#include <string>

// Invariant: a Bureaucrat's grade is always in [1, 150], where 1 is the
// highest grade. Every constructor and mutation enforces it by throwing.
class Bureaucrat {
 public:
  // EXCEPTIONS if too low or too high.
  // INFO: `virtual` is decorative here — it only documents that what() overrides
  // std::exception::what(), which would otherwise be inherited as a fallback.
  // The `throw()` spec must be kept: C++98 forbids widening a base's spec.
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

  class GradeTooHighException : public std::exception {
   public:
    virtual const char* what() const throw();
  };

  class GradeTooLowException : public std::exception {
   public:
    virtual const char* what() const throw();
  };


  Bureaucrat();
  // `explicit` so a bare string never silently becomes a Bureaucrat.
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



 private:
  // Returns `grade` when it is in [1, 150], throws otherwise. Returning the
  // value is what makes it usable in a member-initializer list, and `static`
  // is what makes that safe: it touches no member, so it can run before
  // _grade exists.
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

#endif  // CPP_MODULE05_EX00_BUREAUCRAT_HPP_
