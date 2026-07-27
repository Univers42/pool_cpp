/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bureaucrat.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 19:37:22 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX02_BUREAUCRAT_HPP_
#define CPP_MODULE05_EX02_BUREAUCRAT_HPP_

#include <exception>
#include <iostream>
#include <string>

class AForm;

// Invariant: grade is always in [1, 150]; 1 is the highest rank.
class Bureaucrat {
 public:
  class GradeTooHighException : public std::exception {
   public:
    const char* what() const throw();
  };
  class GradeTooLowException : public std::exception {
   public:
    const char* what() const throw();
  };

  Bureaucrat();
  Bureaucrat(const std::string& name, int grade);
  Bureaucrat(const Bureaucrat& other);
  Bureaucrat& operator=(const Bureaucrat& other);
  ~Bureaucrat();

  const std::string& getName() const;
  int getGrade() const;

  void incrementGrade();  // towards 1
  void decrementGrade();  // towards 150

  void signForm(AForm& form);
  void executeForm(const AForm& form) const;

 private:
  static void validateGrade(int grade);

  const std::string _name;
  int _grade;
};

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b);

#endif  // CPP_MODULE05_EX02_BUREAUCRAT_HPP_
