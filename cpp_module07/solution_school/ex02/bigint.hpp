/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bigint.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 16:26:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/14 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE07_EX02_BIGINT_HPP_
#define CPP_MODULE07_EX02_BIGINT_HPP_

#include <cstddef>
#include <iosfwd>
#include <string>

// An unbounded non-negative integer kept as its decimal digits, most
// significant first, always normalised: no leading zeros, and zero is "0".
// That normalisation is what lets operator< compare lengths first.
//
// It is here to give iter an element type that primitives cannot imitate:
// copying one costs a heap allocation, so the by-value functor parameter that
// is invisible on int becomes measurable.
//
// Deliberately partial - no subtraction, no division, no negatives. A negative
// int constructs zero rather than throwing, which is the one thing here that
// would need revisiting before this went anywhere real.
class bigint {
 public:
  bigint();
  explicit bigint(int n);
  explicit bigint(const std::string& v);
  bigint(const bigint& o);
  bigint& operator=(const bigint& o);
  ~bigint();

  bigint& operator+=(const bigint& o);

  // Decimal shifts: k zeros on the end multiply by 10^k, and erasing k digits
  // off the end is the floor division that undoes it.
  bigint& operator<<=(const bigint& k);
  bigint& operator>>=(const bigint& k);

  bigint& operator++(void);
  bigint operator++(int);

  const std::string& str(void) const;
  size_t digits(void) const;

  // A shift count has to become a real size_t at some point. Anything past
  // nine digits saturates, and a shift that large cannot be materialised
  // anyway - append would throw length_error long before.
  static size_t count(const std::string& v);

  // main.cpp uses these to price a copy instead of claiming it.
  static size_t copies(void);
  static void resetCopies(void);

 private:
  static std::string fromInt(int n);
  static std::string normalize(const std::string& v);

  std::string _s;
  static size_t _copies;
};

bigint operator+(bigint a, const bigint& b);
bigint operator<<(bigint a, const bigint& b);
bigint operator>>(bigint a, const bigint& b);

bool operator<(const bigint& a, const bigint& b);
bool operator>(const bigint& a, const bigint& b);
bool operator<=(const bigint& a, const bigint& b);
bool operator>=(const bigint& a, const bigint& b);
bool operator==(const bigint& a, const bigint& b);
bool operator!=(const bigint& a, const bigint& b);

// Shares its token with operator<<(bigint, const bigint&) above, and the two
// never collide: one takes a stream on the left, the other a bigint.
std::ostream& operator<<(std::ostream& os, const bigint& v);

#endif  // CPP_MODULE07_EX02_BIGINT_HPP_
