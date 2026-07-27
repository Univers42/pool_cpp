/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AForm.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:11:15 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX02_AFORM_HPP_
#define CPP_MODULE05_EX02_AFORM_HPP_

#include <exception>
#include <iostream>
#include <string>

class Bureaucrat;

// Abstract base form. execute() performs the shared checks (signed, executor
// grade) then delegates to the concrete form's executeAction().
class AForm {
 public:
  class GradeTooHighException : public std::exception {
   public:
    const char* what() const throw();
  };
  class GradeTooLowException : public std::exception {
   public:
    const char* what() const throw();
  };
  class NotSignedException : public std::exception {
   public:
    const char* what() const throw();
  };

  AForm();
  AForm(const std::string& name, int gradeToSign, int gradeToExecute);
  AForm(const AForm& other);
  AForm& operator=(const AForm& other);
  virtual ~AForm();

  const std::string& getName() const;
  bool isSigned() const;
  int getGradeToSign() const;
  int getGradeToExecute() const;

  void beSigned(const Bureaucrat& bureaucrat);
  void execute(const Bureaucrat& executor) const;

 protected:
  // Concrete action; execute() has already validated signature and grade.
  virtual void executeAction(const Bureaucrat& executor) const = 0;

 private:
  const std::string _name;
  bool _isSigned;
  const int _gradeToSign;
  const int _gradeToExecute;
};

std::ostream& operator<<(std::ostream& os, const AForm& form);

#endif  // CPP_MODULE05_EX02_AFORM_HPP_
