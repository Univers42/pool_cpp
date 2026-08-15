/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eval.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/14 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Stand-in for the evaluation sheet's `ex00.cpp` attachment.
//
// The sheet links every attachment to https://github.com/rphlr/42-Subjects/,
// which is a 404 — the repo was deleted, so the original file is no longer
// retrievable. This is rebuilt from the two things the sheet actually asks
// for at the ex00 station:
//
//   "Simple types  — Refer to the subject for the expected output with
//                    simple types, such as int."
//   "Complex types — Do the functions also work with complex types?
//                    (check with the ex00.cpp file in attachment)"
//
// So: the subject's own main verbatim, then the same three templates driven
// with a user-defined type that only supplies < and >.

#include <iostream>
#include <string>

#include "../whatever.hpp"

// A "complex type" in the sense the sheet means it: not a builtin, not
// trivially copyable-by-memcpy in spirit, and ordered by something other than
// its bit pattern. swap() must move the whole object, min()/max() must order
// by the class's own operator< / operator>.
class Complex {
 public:
  Complex(int re, int im) : re_(re), im_(im) {}

  int re() const { return re_; }
  int im() const { return im_; }

  // Squared modulus keeps this integral, so there is no float comparison to
  // argue about during a defence.
  int mod2() const { return re_ * re_ + im_ * im_; }

  bool operator<(const Complex& other) const { return mod2() < other.mod2(); }
  bool operator>(const Complex& other) const { return mod2() > other.mod2(); }

 private:
  int re_;
  int im_;
};

std::ostream& operator<<(std::ostream& out, const Complex& c) {
  out << c.re() << (c.im() < 0 ? " - " : " + ")
      << (c.im() < 0 ? -c.im() : c.im()) << "i";
  return out;
}

int main(void) {
  // ---- the subject's main, unchanged ------------------------------------
  int a = 2;
  int b = 3;

  ::swap(a, b);
  std::cout << "a = " << a << ", b = " << b << std::endl;
  std::cout << "min( a, b ) = " << ::min(a, b) << std::endl;
  std::cout << "max( a, b ) = " << ::max(a, b) << std::endl;

  std::string c = "chaine1";
  std::string d = "chaine2";

  ::swap(c, d);
  std::cout << "c = " << c << ", d = " << d << std::endl;
  std::cout << "min( c, d ) = " << ::min(c, d) << std::endl;
  std::cout << "max( c, d ) = " << ::max(c, d) << std::endl;

  // ---- "Do the functions also work with complex types?" ------------------
  Complex e(3, 4);   // mod2 == 25
  Complex f(1, 2);   // mod2 ==  5

  ::swap(e, f);
  std::cout << "e = " << e << ", f = " << f << std::endl;
  std::cout << "min( e, f ) = " << ::min(e, f) << std::endl;
  std::cout << "max( e, f ) = " << ::max(e, f) << std::endl;

  // Ties must hand back the second argument, and they must do it without
  // copying — that is only observable if min/max return a reference.
  Complex g(0, 5);   // mod2 == 25
  Complex h(5, 0);   // mod2 == 25, equal to g

  std::cout << "min( g, h ) is h : "
            << ((&::min(g, h) == &h) ? "yes" : "no") << std::endl;
  std::cout << "max( g, h ) is h : "
            << ((&::max(g, h) == &h) ? "yes" : "no") << std::endl;

  return 0;
}
