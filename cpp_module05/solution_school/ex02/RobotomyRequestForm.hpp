/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RobotomyRequestForm.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:27:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX02_ROBOTOMYREQUESTFORM_HPP_
#define CPP_MODULE05_EX02_ROBOTOMYREQUESTFORM_HPP_

#include <string>

#include "AForm.hpp"

// sign 72, exec 45 — makes drilling noises, then succeeds half the time. The
// only one of the three whose action is not deterministic, which is why the
// test suite checks the shape of the output rather than its exact text.
class RobotomyRequestForm : public AForm {
 public:
  // Default constructor. Required: module 05 exempts only the exception
  // classes from Orthodox Canonical Form — "every other class must follow it".
  // It targets "default"; the target-taking constructor below is the one the
  // subject actually asks for, and the only one any code here calls.
  RobotomyRequestForm();
  explicit RobotomyRequestForm(const std::string& target);
  RobotomyRequestForm(const RobotomyRequestForm& other);
  RobotomyRequestForm& operator=(const RobotomyRequestForm& other);
  virtual ~RobotomyRequestForm();

 protected:
  virtual void executeAction(const Bureaucrat& executor) const;

 private:
  std::string _target;
};

#endif  // CPP_MODULE05_EX02_ROBOTOMYREQUESTFORM_HPP_
