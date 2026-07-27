/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Form.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:11:15 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX01_FORM_HPP_
#define CPP_MODULE05_EX01_FORM_HPP_

#include <exception>
#include <iostream>
#include <string>

class Bureaucrat;

// Grades follow the Bureaucrat rules: [1, 150], 1 is the highest.
class Form {
 public:
  class GradeTooHighException : public std::exception {
   public:
    const char* what() const throw();
  };
  class GradeTooLowException : public std::exception {
   public:
    const char* what() const throw();
  };

  Form();
  Form(const std::string& name, int gradeToSign, int gradeToExecute);
  Form(const Form& other);
  Form& operator=(const Form& other);
  ~Form();

  const std::string& getName() const;
  bool isSigned() const;
  int getGradeToSign() const;
  int getGradeToExecute() const;

  void beSigned(const Bureaucrat& bureaucrat);

 private:
  const std::string _name;
  bool _isSigned;
  const int _gradeToSign;
  const int _gradeToExecute;
};

std::ostream& operator<<(std::ostream& os, const Form& form);

#endif  // CPP_MODULE05_EX01_FORM_HPP_
