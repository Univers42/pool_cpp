/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 16:17:46 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 16:35:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

#include <cctype>
#include <sstream>
#include <string>

RPN::RPN() {}

RPN::RPN(const RPN& src) : _stack(src._stack) {}

RPN& RPN::operator=(const RPN& rhs) {
  if (this != &rhs) {
    this->_stack = rhs._stack;
  }
  return *this;
}

RPN::~RPN() {}

bool RPN::isOperator(const std::string& token) const {
  return (token == "+" || token == "-" || token == "*" || token == "/");
}

// ponytail: plain int arithmetic — a long chain like eleven 9s multiplied
// overflows and wraps. The subject caps operands below 10 and says nothing
// about intermediate overflow; add checked ops only if an evaluator asks.
void RPN::applyOperation(const std::string& op) {
  if (_stack.size() < 2) {
    throw std::runtime_error("Error");
  }

  // The first popped element is the RIGHT operand!
  int right = _stack.top();
  _stack.pop();
  int left = _stack.top();
  _stack.pop();

  if (op == "+") {
    _stack.push(left + right);
  } else if (op == "-") {
    _stack.push(left - right);
  } else if (op == "*") {
    _stack.push(left * right);
  } else if (op == "/") {
    if (right == 0) {
      throw std::runtime_error("Error");
    }
    _stack.push(left / right);
  }
}

int RPN::evaluate(const std::string& expr) {
  while (!_stack.empty()) {  // make the object reusable across calls
    _stack.pop();
  }

  std::istringstream iss(expr);
  std::string token;

  while (iss >> token) {
    if (token.length() == 1 &&
        std::isdigit(static_cast<unsigned char>(token[0]))) {
      // The subject guarantees operands are single digits (< 10).
      _stack.push(token[0] - '0');
    } else if (token.length() == 1 && isOperator(token)) {
      applyOperation(token);
    } else {
      throw std::runtime_error("Error");
    }
  }

  // A well-formed expression leaves exactly one value on the stack.
  if (_stack.size() != 1) {
    throw std::runtime_error("Error");
  }

  return _stack.top();
}
