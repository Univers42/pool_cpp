/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Intern.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:40:07 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Intern.hpp"

#include <cstddef>
#include <iostream>

#include "AForm.hpp"
#include "PresidentialPardonForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "ShrubberyCreationForm.hpp"

#ifdef DEBUG
#define TRACE(what) (std::cout << (what) << " " << this << std::endl)
#else
#define TRACE(what) ((void)0)
#endif

// OCF, all four empty: an Intern has no members, so there is nothing to
// initialise, nothing to copy and nothing to release. operator= still returns
// *this so that chained assignment keeps working.
Intern::Intern() { TRACE("default constructor"); }
Intern::Intern(const Intern& other) { (void)other; TRACE("copy constructor"); }
Intern& Intern::operator=(const Intern& other) { (void)other; return (*this); }
Intern::~Intern() { TRACE("destructor"); }

namespace {

// One free function per form, each with the SAME signature. That uniformity is
// what makes the table below possible: three different constructors, three
// different types, one common shape a function pointer can hold.
AForm* makeShrubbery(const std::string& target) { return (new ShrubberyCreationForm(target)); }
AForm* makeRobotomy(const std::string& target) { return (new RobotomyRequestForm(target)); }
AForm* makePardon(const std::string& target) { return (new PresidentialPardonForm(target)); }

typedef AForm* (*Creator)(const std::string&);

struct FormEntry {
  const char* name;
  Creator create;
};

// THE POINT OF THE EXERCISE. The subject rejects "an excessive if/elseif/else
// structure", and the reason is not aesthetics: in a ladder, the name-to-type
// mapping is spread across control flow, so adding a fourth form means writing
// another branch and hoping it matches the shape of the other three. Here the
// mapping is DATA. Adding a form is one line in this table plus one creator
// above; the search below never changes, and there is no branch to get wrong.
//
// C++98 has no std::map initialiser list, so a plain array with a linear scan
// is the honest tool at n = 3 — a map would cost a static constructor and more
// code to save nothing at this size.
const FormEntry kForms[] = {
    {"shrubbery creation", &makeShrubbery},
    {"robotomy request", &makeRobotomy},
    {"presidential pardon", &makePardon},
};

const std::size_t kFormCount = sizeof(kForms) / sizeof(kForms[0]);

}  // namespace

AForm* Intern::makeForm(const std::string& formName, const std::string& target) const {
  for (std::size_t i = 0; i < kFormCount; ++i) {
    if (formName != kForms[i].name) continue;

    // Construct FIRST, announce second: if `new` throws (or a form's
    // constructor rejects something), the caller gets the exception and never
    // sees a "creates" line for a form that does not exist.
    AForm* form = kForms[i].create(target);
    // Printing the built form's own name rather than the string we matched on
    // makes the line self-verifying: it can only say "RobotomyRequestForm" if
    // a RobotomyRequestForm is what actually came back.
    std::cout << "Intern creates " << form->getName() << std::endl;
    return (form);
  }

  // Unknown name. Returning NULL rather than throwing is deliberate: the
  // subject's own example assigns the result straight into an AForm* with no
  // try/catch around it, so a throw here would break the usage it shows. The
  // error message is the "explicit error message" the subject asks for, and it
  // echoes the bad name so the caller can see the typo.
  std::cout << "Intern cannot create \"" << formName << "\": no such form. Known forms are:";
  for (std::size_t i = 0; i < kFormCount; ++i) std::cout << (i ? ", " : " ") << kForms[i].name;
  std::cout << "." << std::endl;
  return (NULL);
}
