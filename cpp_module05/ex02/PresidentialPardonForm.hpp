/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PresidentialPardonForm.hpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:27:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX02_PRESIDENTIALPARDONFORM_HPP_
#define CPP_MODULE05_EX02_PRESIDENTIALPARDONFORM_HPP_

#include <string>

#include "AForm.hpp"

// sign 25, exec 5: pardons <target> in the name of Zaphod Beeblebrox.
class PresidentialPardonForm : public AForm {
 public:
  PresidentialPardonForm();
  explicit PresidentialPardonForm(const std::string& target);
  PresidentialPardonForm(const PresidentialPardonForm& other);
  PresidentialPardonForm& operator=(const PresidentialPardonForm& other);
  ~PresidentialPardonForm();

 protected:
  void executeAction(const Bureaucrat& executor) const;

 private:
  std::string _target;
};

#endif  // CPP_MODULE05_EX02_PRESIDENTIALPARDONFORM_HPP_
