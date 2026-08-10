/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShrubberyCreationForm.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:27:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX02_SHRUBBERYCREATIONFORM_HPP_
#define CPP_MODULE05_EX02_SHRUBBERYCREATIONFORM_HPP_

#include <string>

#include "AForm.hpp"

// sign 145, exec 137 — the most permissive of the three: almost any bureaucrat
// can sign one, and most can execute it. Writes ASCII trees to
// <target>_shrubbery in the working directory.
class ShrubberyCreationForm : public AForm {
 public:
  // their target ("home" if you want to plant shrubbery at home), so a form
  // without one has no meaning — it would only ever be a valid object in a
  // nonsense state. Declaring the target ctor suppresses the implicit default
  // one, so `ShrubberyCreationForm f;` is a compile error, which is the right
  // answer to that line. OCF's default ctor is a requirement on classes that
  // have a sensible empty state; this one does not.
  // Default constructor. Required: module 05 exempts only the exception
  // classes from Orthodox Canonical Form — "every other class must follow it".
  // It targets "default"; the target-taking constructor below is the one the
  // subject actually asks for, and the only one any code here calls.
  ShrubberyCreationForm();
  explicit ShrubberyCreationForm(const std::string& target);
  ShrubberyCreationForm(const ShrubberyCreationForm& other);
  ShrubberyCreationForm& operator=(const ShrubberyCreationForm& other);
  // Virtual via AForm; spelled out because the base declares it virtual and
  // this one really does get called through an AForm*.
  virtual ~ShrubberyCreationForm();

 protected:
  // C++98 has no `override`, so `virtual` is the only marker that this is the
  // hook AForm::execute() calls. A typo in the signature would silently leave
  // the class abstract instead — which at least fails at compile time.
  virtual void executeAction(const Bureaucrat& executor) const;

 private:
  std::string _target;
};

#endif  // CPP_MODULE05_EX02_SHRUBBERYCREATIONFORM_HPP_
