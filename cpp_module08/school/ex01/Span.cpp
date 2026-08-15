/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Span.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:30:14 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Span.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <numeric>
#include <ostream>
#include <vector>

namespace {

// The distance between two ints, as an unsigned int. Casting before
// subtracting is the whole trick: the span between INT_MIN and INT_MAX is
// 4294967295, which fits in no signed 32-bit type, and C++98 has no int64_t.
// Unsigned arithmetic is defined to wrap mod 2^32, and the wrapped result is
// the exact distance for every pair of ints. Signed overflow, by contrast, is
// undefined behaviour — the compiler is allowed to assume it never happens.
struct UnsignedGap {
  unsigned int operator()(int high, int low) const {
    return static_cast<unsigned int>(high) - static_cast<unsigned int>(low);
  }
};

// std::min is a function template; std::inner_product wants something it can
// call. A functor is the C++98 way to hand an algorithm a operation.
struct Smaller {
  unsigned int operator()(unsigned int a, unsigned int b) const {
    return a < b ? a : b;
  }
};

}  // namespace

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

unsigned int Span::size() const {
  return static_cast<unsigned int>(_vec.size());
}

unsigned int Span::maxSize() const { return _maxSize; }

bool Span::empty() const { return _vec.empty(); }

bool Span::full() const { return _vec.size() >= _maxSize; }

Span::const_iterator Span::begin() const { return _vec.begin(); }

Span::const_iterator Span::end() const { return _vec.end(); }

// ponytail: no int64_t in C++98 — see UnsignedGap above for why the casts are
// the answer rather than a bigger type. Upgrade path: uint64_t if ints ever
// grow past 32 bits.
unsigned int Span::longestSpan() const {
  if (_vec.size() < 2) throw NotEnoughElementsException();

  const int minVal = *std::min_element(_vec.begin(), _vec.end());
  const int maxVal = *std::max_element(_vec.begin(), _vec.end());

  return UnsignedGap()(maxVal, minVal);
}

// The shortest span is NOT the difference between the two lowest values — the
// subject's own example says so: {6, 3, 17, 9, 11} has 3 and 6 at the bottom,
// a gap of 3, while the real answer is 2, between 9 and 11. The smallest gap
// can sit anywhere, but after sorting it is always between two NEIGHBOURS, so
// one pass over adjacent pairs finds it in O(n log n) instead of comparing all
// n(n-1)/2 pairs.
//
// That pass is std::inner_product, which is misnamed: it is the general
// two-range fold. It computes
//     init = combine(init, pairwise(*first1, *first2))
// for each step, so feeding it the sorted range offset by one against itself
// reduces to "the smallest gap between neighbours" — one pass, no temporary
// vector of differences, and not a hand-written loop in sight.
unsigned int Span::shortestSpan() const {
  if (_vec.size() < 2) throw NotEnoughElementsException();

  std::vector<int> sorted(_vec);
  std::sort(sorted.begin(), sorted.end());

  return std::inner_product(sorted.begin() + 1, sorted.end(), sorted.begin(),
                            std::numeric_limits<unsigned int>::max(),
                            Smaller(), UnsignedGap());
}

const char* Span::SpanFullException::what() const throw() {
  return "Error: Span is already full, cannot add more numbers.";
}

const char* Span::NotEnoughElementsException::what() const throw() {
  return "Error: Not enough elements to calculate a span (requires at least "
         "2).";
}

std::ostream& operator<<(std::ostream& os, const Span& span) {
  os << "Span(" << span.size() << "/" << span.maxSize() << ") [";
  for (Span::const_iterator it = span.begin(); it != span.end(); ++it) {
    if (it != span.begin()) os << ", ";
    os << *it;
  }
  return os << "]";
}
