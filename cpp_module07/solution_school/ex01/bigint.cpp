/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bigint.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 16:26:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/14 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bigint.hpp"

#include <ostream>

size_t bigint::_copies = 0;

bigint::bigint() : _s("0") {}

bigint::bigint(int n) : _s(fromInt(n)) {}

bigint::bigint(const std::string& v) : _s(normalize(v)) {}

bigint::bigint(const bigint& o) : _s(o._s) { ++_copies; }

bigint& bigint::operator=(const bigint& o) {
  if (this != &o) _s = o._s;
  return (*this);
}

bigint::~bigint() {}

// Schoolbook addition, right to left. The left operand is padded with leading
// zeros first so both numbers end at the same column, then the carry walks off
// the end of the shorter one on its own.
bigint& bigint::operator+=(const bigint& o) {
  const std::string b = o._s;
  if (_s.size() < b.size())
    _s.insert(static_cast<size_t>(0), b.size() - _s.size(), '0');

  size_t i = _s.size();
  size_t j = b.size();
  int carry = 0;
  while (j > 0) {
    --i;
    --j;
    const int d = (_s[i] - '0') + (b[j] - '0') + carry;
    carry = (d >= 10);
    _s[i] = static_cast<char>('0' + d - 10 * carry);
  }
  while (carry && i > 0) {
    --i;
    if (_s[i] == '9') {
      _s[i] = '0';
    } else {
      ++_s[i];
      carry = 0;
    }
  }
  if (carry) _s.insert(_s.begin(), '1');
  return (*this);
}

bigint& bigint::operator<<=(const bigint& k) {
  const size_t n = count(k._s);
  if (_s != "0") _s.append(n, '0');
  return (*this);
}

bigint& bigint::operator>>=(const bigint& k) {
  const size_t n = count(k._s);
  const size_t len = _s.size();
  if (n >= len)
    _s = "0";
  else
    _s.erase(len - n);
  return (*this);
}

bigint& bigint::operator++(void) { return (*this += bigint(1)); }

bigint bigint::operator++(int) {
  bigint tmp(*this);
  ++(*this);
  return (tmp);
}

const std::string& bigint::str(void) const { return (_s); }

size_t bigint::digits(void) const { return (_s.size()); }

size_t bigint::count(const std::string& v) {
  if (v.size() > 9) return (~static_cast<size_t>(0));
  size_t k = 0;
  for (size_t i = 0; i < v.size(); ++i)
    k = k * 10 + static_cast<size_t>(v[i] - '0');
  return (k);
}

size_t bigint::copies(void) { return (_copies); }

void bigint::resetCopies(void) { _copies = 0; }

std::string bigint::fromInt(int n) {
  if (n <= 0) return ("0");
  std::string out;
  while (n) {
    out.insert(out.begin(), static_cast<char>('0' + n % 10));
    n /= 10;
  }
  return (out);
}

// Trusts its input to be digits - the callers here are literals. An empty
// string and any run of zeros both come out as "0".
std::string bigint::normalize(const std::string& v) {
  const size_t first = v.find_first_not_of('0');
  if (first == std::string::npos) return ("0");
  return (v.substr(first));
}

bigint operator+(bigint a, const bigint& b) { return (a += b); }

bigint operator<<(bigint a, const bigint& b) { return (a <<= b); }

bigint operator>>(bigint a, const bigint& b) { return (a >>= b); }

// Normalised digits mean a longer number is always the bigger one, so the
// lexicographic compare only ever runs on equal lengths.
bool operator<(const bigint& a, const bigint& b) {
  return (a.str().size() != b.str().size() ? a.str().size() < b.str().size()
                                           : a.str() < b.str());
}

bool operator>(const bigint& a, const bigint& b) { return (b < a); }

bool operator<=(const bigint& a, const bigint& b) { return (!(b < a)); }

bool operator>=(const bigint& a, const bigint& b) { return (!(a < b)); }

bool operator==(const bigint& a, const bigint& b) {
  return (a.str() == b.str());
}

bool operator!=(const bigint& a, const bigint& b) { return (!(a == b)); }

std::ostream& operator<<(std::ostream& os, const bigint& v) {
  return (os << v.str());
}
