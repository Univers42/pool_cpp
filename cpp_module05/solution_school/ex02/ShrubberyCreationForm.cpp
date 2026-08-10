/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShrubberyCreationForm.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:27:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ShrubberyCreationForm.hpp"

#include <fstream>
#include <stdexcept>

#ifdef DEBUG
#define TRACE(what) (std::cout << (what) << " " << this << std::endl)
#else
#define TRACE(what) ((void)0)
#endif

// The name and the two grades are fixed by the subject (sign 145, exec 137),
// so they are baked into the base call rather than exposed as parameters —
// the subject is explicit that these take only the target.
ShrubberyCreationForm::ShrubberyCreationForm() : AForm("ShrubberyCreationForm", 145, 137), _target("default") { TRACE("default constructor"); }
ShrubberyCreationForm::ShrubberyCreationForm(const std::string& target) : AForm("ShrubberyCreationForm", 145, 137), _target(target) { TRACE("target constructor"); }
// AForm(other) copies the base sub-object; forgetting it would default-construct
// the base and silently reset the grades and the signature.
ShrubberyCreationForm::ShrubberyCreationForm(const ShrubberyCreationForm& other) : AForm(other), _target(other._target) { TRACE("copy constructor"); }
ShrubberyCreationForm::~ShrubberyCreationForm() { TRACE("destructor"); }

// AForm::operator= carries the signature; _target is ours to copy.
ShrubberyCreationForm& ShrubberyCreationForm::operator=(const ShrubberyCreationForm& other) { if (this != &other) { AForm::operator=(other); _target = other._target; } return (*this); }

// execute() has already checked that the form is signed and that the executor
// outranks it, so this only has to do the work.
void ShrubberyCreationForm::executeAction(const Bureaucrat& executor) const {
  (void)executor;
  const std::string filename = _target + "_shrubbery";
  std::ofstream ofs(filename.c_str());
  // A read-only directory or a bad target is a real failure, not a silent one.
  // Bureaucrat::executeForm catches std::exception, so this surfaces as the
  // "couldn't execute" line rather than escaping to the caller.
  if (!ofs) throw std::runtime_error("could not open " + filename);

  const char* tree =
      "       _-_\n"
      "    /~~   ~~\\\n"
      " /~~         ~~\\\n"
      "{               }\n"
      " \\  _-     -_  /\n"
      "   ~  \\\\ //  ~\n"
      "_- -   | | _- _\n"
      "  _ -  | |   -_\n"
      "      // \\\\\n";

  for (int i = 0; i < 3; ++i) ofs << tree << "\n";
}
