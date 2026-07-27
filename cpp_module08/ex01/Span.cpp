/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Span.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:30:14 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:18:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Span.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

Span::Span() : _maxSize(0) {}

Span::Span(unsigned int N) : _maxSize(N) { _vec.reserve(N); }

Span::Span(const Span& src) : _maxSize(src._maxSize), _vec(src._vec) {}

Span& Span::operator=(const Span& rhs) {
  if (this != &rhs) {
    this->_maxSize = rhs._maxSize;
    this->_vec = rhs._vec;
  }
  return *this;
}

Span::~Span() {}

void Span::addNumber(int n) {
  if (_vec.size() >= _maxSize) throw SpanFullException();
  _vec.push_back(n);
}

// ponytail: no int64_t in C++98 — unsigned subtraction wraps mod 2^32 and
// yields the exact distance for any int pair (max span INT_MIN..INT_MAX fits
// in unsigned int). Upgrade path: uint64_t if ints ever grow past 32 bits.
unsigned int Span::longestSpan() const {
  if (_vec.size() < 2) throw NotEnoughElementsException();

  int minVal = *std::min_element(_vec.begin(), _vec.end());
  int maxVal = *std::max_element(_vec.begin(), _vec.end());

  return static_cast<unsigned int>(maxVal) - static_cast<unsigned int>(minVal);
}

unsigned int Span::shortestSpan() const {
  if (_vec.size() < 2) throw NotEnoughElementsException();

  std::vector<int> sortedVec = _vec;
  std::sort(sortedVec.begin(), sortedVec.end());

  unsigned int minSpan = std::numeric_limits<unsigned int>::max();
  for (std::size_t i = 1; i < sortedVec.size(); ++i) {
    unsigned int diff = static_cast<unsigned int>(sortedVec[i]) -
                        static_cast<unsigned int>(sortedVec[i - 1]);
    if (diff < minSpan) minSpan = diff;
  }
  return minSpan;
}

const char* Span::SpanFullException::what() const throw() {
  return "Error: Span is already full, cannot add more numbers.";
}

const char* Span::NotEnoughElementsException::what() const throw() {
  return "Error: Not enough elements to calculate a span (requires at least "
         "2).";
}
