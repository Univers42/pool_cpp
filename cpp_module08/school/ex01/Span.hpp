/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Span.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:25:16 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE08_EX01_SPAN_HPP_
#define CPP_MODULE08_EX01_SPAN_HPP_

#include <exception>
#include <iterator>
#include <ostream>
#include <vector>

class Span {
 private:
  unsigned int _maxSize;
  std::vector<int> _vec;

 public:
  // Read-only iteration only. A mutable iterator would let a caller resize
  // the vector behind the class's back, and _maxSize is the one thing Span
  // exists to enforce. Exposing the typedef under this name is also what lets
  // ex00's easyfind(container, int) accept a Span unchanged: its whole
  // vocabulary is begin(), end() and T::const_iterator.
  typedef std::vector<int>::const_iterator const_iterator;

  Span();
  explicit Span(unsigned int N);
  Span(const Span& src);
  Span& operator=(const Span& rhs);
  ~Span();

  void addNumber(int n);
  unsigned int shortestSpan() const;
  unsigned int longestSpan() const;

  unsigned int size() const;
  unsigned int maxSize() const;
  bool empty() const;
  bool full() const;

  const_iterator begin() const;
  const_iterator end() const;

  // Capacity is checked BEFORE inserting, so an oversized range throws and
  // leaves the Span untouched (atomic). ponytail: distance + insert is two
  // passes, so this needs forward iterators; single-pass input iterators
  // would be consumed by the distance() walk.
  //
  // This is the "more practical than calling addNumber() repeatedly" the
  // subject asks for: any iterator pair works, including two pointers into a
  // C array and another Span's begin()/end().
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

// A free operator<<, not a member and emphatically not a friend: begin() and
// end() are public, so it needs no privileged access. The 42 evaluation sheet
// flags the friend keyword, and this is the case where people reach for it.
std::ostream& operator<<(std::ostream& os, const Span& span);

#endif  // CPP_MODULE08_EX01_SPAN_HPP_
