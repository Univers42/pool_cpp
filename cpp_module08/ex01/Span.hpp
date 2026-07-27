/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Span.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:25:16 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 12:41:36 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE08_EX01_SPAN_HPP_
#define CPP_MODULE08_EX01_SPAN_HPP_

#include <exception>
#include <iterator>
#include <vector>

class Span {
 private:
  unsigned int _maxSize;
  std::vector<int> _vec;

 public:
  Span();
  explicit Span(unsigned int N);
  Span(const Span& src);
  Span& operator=(const Span& rhs);
  ~Span();

  void addNumber(int n);
  unsigned int shortestSpan() const;
  unsigned int longestSpan() const;

  // Capacity is checked BEFORE inserting, so an oversized range throws and
  // leaves the Span untouched (atomic). ponytail: distance + insert is two
  // passes, so this needs forward iterators; single-pass input iterators
  // would be consumed by the distance() walk.
  template <typename InputIterator>
  void addNumber(InputIterator first, InputIterator last) {
    if (std::distance(first, last) + _vec.size() > _maxSize) {
      throw SpanFullException();
    }
    _vec.insert(_vec.end(), first, last);
  }

  class SpanFullException : public std::exception {
   public:
    virtual const char* what() const throw();
  };

  class NotEnoughElementsException : public std::exception {
   public:
    virtual const char* what() const throw();
  };
};

#endif  // CPP_MODULE08_EX01_SPAN_HPP_
