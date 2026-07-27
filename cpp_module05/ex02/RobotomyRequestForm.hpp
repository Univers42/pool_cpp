/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RobotomyRequestForm.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:27:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX02_ROBOTOMYREQUESTFORM_HPP_
#define CPP_MODULE05_EX02_ROBOTOMYREQUESTFORM_HPP_

#include <string>

#include "AForm.hpp"

// sign 72, exec 45: drills, then robotomizes <target> 50% of the time.
class RobotomyRequestForm : public AForm {
 public:
  RobotomyRequestForm();
  explicit RobotomyRequestForm(const std::string& target);
  RobotomyRequestForm(const RobotomyRequestForm& other);
  RobotomyRequestForm& operator=(const RobotomyRequestForm& other);
  ~RobotomyRequestForm();

 protected:
  void executeAction(const Bureaucrat& executor) const;

 private:
  std::string _target;
};

#endif  // CPP_MODULE05_EX02_ROBOTOMYREQUESTFORM_HPP_
