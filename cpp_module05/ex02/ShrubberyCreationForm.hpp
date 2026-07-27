/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShrubberyCreationForm.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:27:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX02_SHRUBBERYCREATIONFORM_HPP_
#define CPP_MODULE05_EX02_SHRUBBERYCREATIONFORM_HPP_

#include <string>

#include "AForm.hpp"

// sign 145, exec 137: writes ASCII trees to <target>_shrubbery.
class ShrubberyCreationForm : public AForm {
 public:
  ShrubberyCreationForm();
  explicit ShrubberyCreationForm(const std::string& target);
  ShrubberyCreationForm(const ShrubberyCreationForm& other);
  ShrubberyCreationForm& operator=(const ShrubberyCreationForm& other);
  ~ShrubberyCreationForm();

 protected:
  void executeAction(const Bureaucrat& executor) const;

 private:
  std::string _target;
};

#endif  // CPP_MODULE05_EX02_SHRUBBERYCREATIONFORM_HPP_
