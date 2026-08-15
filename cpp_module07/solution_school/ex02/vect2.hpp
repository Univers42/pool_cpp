/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vect2.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 16:23:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/14 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE07_EX02_VECT2_HPP_
#define CPP_MODULE07_EX02_VECT2_HPP_

#include <iosfwd>

// A pair of ints with a full operator set - the 42 exam's vect2. It is the
// cheapest way to give iter an element type that is a real class: assignable,
// printable, mutable through a reference, and carrying two operator[]
// overloads that the const-ness of the array picks between.
class vect2 {
 public:
  vect2();
  vect2(int x, int y);
  vect2(const vect2& o);
  vect2& operator=(const vect2& o);
  ~vect2();

  // Index 0 is x and anything else is y, which is the exam's contract. The
  // pair matters here: a vect2& callback gets the first and can assign
  // through it, a const vect2& callback gets the second and cannot.
  int& operator[](int idx);
  int operator[](int idx) const;

  vect2& operator++(void);
  vect2& operator--(void);
  vect2 operator++(int);
  vect2 operator--(int);

  vect2& operator+=(const vect2& o);
  vect2& operator-=(const vect2& o);
  vect2& operator*=(int n);

  vect2 operator+(const vect2& o) const;
  vect2 operator-(const vect2& o) const;
  vect2 operator-(void) const;
  vect2 operator*(int n) const;

  bool operator==(const vect2& o) const;
  bool operator!=(const vect2& o) const;

 private:
  int _x;
  int _y;
};

vect2 operator*(int n, const vect2& v);
std::ostream& operator<<(std::ostream& os, const vect2& v);

#endif  // CPP_MODULE07_EX02_VECT2_HPP_
