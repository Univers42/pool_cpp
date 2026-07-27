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

#ifndef CPP_MODULE05_EX00_BUREAUCRAT_HPP_
#define CPP_MODULE05_EX00_BUREAUCRAT_HPP_

#include <exception>
#include <iostream>
#include <string>

// Invariant: a Bureaucrat's grade is always in [1, 150], where 1 is the
// highest grade. Every constructor and mutation enforces it by throwing.
class Bureaucrat {
 public:
  // ponytail: exception classes skip OCF — the subject explicitly allows it.
  class GradeTooHighException : public std::exception {
   public:
    virtual const char* what() const throw();
  };

  class GradeTooLowException : public std::exception {
   public:
    virtual const char* what() const throw();
  };

  Bureaucrat();
  Bureaucrat(const std::string& name, int grade);
  Bureaucrat(const Bureaucrat& other);
  Bureaucrat& operator=(const Bureaucrat& other);
  ~Bureaucrat();

  std::string getName() const;
  int getGrade() const;

  void incrementGrade();  // toward 1
  void decrementGrade();  // toward 150

 private:
  void validateGrade(int grade) const;

  const std::string _name;
  int _grade;
};

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b);

#endif  // CPP_MODULE05_EX00_BUREAUCRAT_HPP_
