/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PresidentialPardonForm.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:27:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PresidentialPardonForm.hpp"

#include <iostream>

#ifdef DEBUG
#define TRACE(what) (std::cout << (what) << " " << this << std::endl)
#else
#define TRACE(what) ((void)0)
#endif

// Name and grades fixed by the subject: sign 25, exec 5 — the strictest of the
// three, which is why only a very senior bureaucrat can execute one. No
// default ctor — the header says why.
PresidentialPardonForm::PresidentialPardonForm() : AForm("PresidentialPardonForm", 25, 5), _target("default") { TRACE("default constructor"); }
PresidentialPardonForm::PresidentialPardonForm(const std::string& target) : AForm("PresidentialPardonForm", 25, 5), _target(target) { TRACE("target constructor"); }
PresidentialPardonForm::PresidentialPardonForm(const PresidentialPardonForm& other) : AForm(other), _target(other._target) { TRACE("copy constructor"); }
PresidentialPardonForm::~PresidentialPardonForm() { TRACE("destructor"); }

PresidentialPardonForm& PresidentialPardonForm::operator=(const PresidentialPardonForm& other) { if (this != &other) { AForm::operator=(other); _target = other._target; } return (*this); }

// execute() has already checked signature and grade.
void PresidentialPardonForm::executeAction(const Bureaucrat& executor) const {
  (void)executor;
  std::cout << _target << " has been pardoned by Zaphod Beeblebrox" << std::endl;
}
