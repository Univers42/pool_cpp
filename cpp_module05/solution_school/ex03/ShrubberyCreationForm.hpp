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

#ifndef CPP_MODULE05_EX03_SHRUBBERYCREATIONFORM_HPP_
#define CPP_MODULE05_EX03_SHRUBBERYCREATIONFORM_HPP_

#include <string>

#include "AForm.hpp"

// sign 145, exec 137 — the most permissive of the three: almost any bureaucrat
// can sign one, and most can execute it. Writes ASCII trees to
// <target>_shrubbery in the working directory.
class ShrubberyCreationForm : public AForm {
 public:
  // Default constructor. Required: module 05 exempts only the exception
  // classes from Orthodox Canonical Form — "every other class must follow it".
  // It targets "default"; the target-taking constructor below is the one the
  // subject actually asks for, and the only one any code here calls.
  ShrubberyCreationForm();
  explicit ShrubberyCreationForm(const std::string& target);
  ShrubberyCreationForm(const ShrubberyCreationForm& other);
  ShrubberyCreationForm& operator=(const ShrubberyCreationForm& other);
  virtual ~ShrubberyCreationForm();

 protected:
  virtual void executeAction(const Bureaucrat& executor) const;

 private:
  std::string _target;
};

#endif  // CPP_MODULE05_EX03_SHRUBBERYCREATIONFORM_HPP_
