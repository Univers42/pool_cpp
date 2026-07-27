/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 16:17:48 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 16:35:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE09_EX01_RPN_HPP_
#define CPP_MODULE09_EX01_RPN_HPP_

#include <list>
#include <stack>
#include <stdexcept>
#include <string>

class RPN {
 private:
  // ponytail: std::stack over std::list on purpose — module 09 forbids
  // reusing containers across exercises; this keeps vector/deque for ex02.
  std::stack<int, std::list<int> > _stack;

  void applyOperation(const std::string& op);
  bool isOperator(const std::string& token) const;

 public:
  RPN();
  RPN(const RPN& src);
  RPN& operator=(const RPN& rhs);
  ~RPN();

  // Evaluates a postfix expression and returns the result.
  // Throws std::runtime_error("Error") on any invalid input.
  int evaluate(const std::string& expr);
};

#endif  // CPP_MODULE09_EX01_RPN_HPP_
