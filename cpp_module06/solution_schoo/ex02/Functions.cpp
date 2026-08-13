/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Functions.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 22:01:11 by marvin            #+#    #+#             */
/*   Updated: 2025/12/25 22:01:11 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Functions.hpp"

#include <cstdlib>
#include <iostream>

#include "A.hpp"
#include "B.hpp"
#include "C.hpp"

Base* generate(void) {
  switch (std::rand() % 3) {
    case 0:
      std::cout << "[generated A]" << std::endl;
      return new A();
    case 1:
      std::cout << "[generated B]" << std::endl;
      return new B();
    default:
      std::cout << "[generated C]" << std::endl;
      return new C();
  }
}

// the <dynamic_cast<A*>(p) produces a pointer
// if p actually points to an A:-> a valid pointer in an `if  is treated as true
// if `p` does NOT point to an `A`:->nullptr is treated as false.
// essentially it's like saying A* result = dynamic_cast<A*>(p)
// if (result != nullptr)
// std::cout << "A" << std::endl;
void identify(Base* p) {
  if (dynamic_cast<A*>(p))
    std::cout << "A" << std::endl;
  else if (dynamic_cast<B*>(p))
    std::cout << "B" << std::endl;
  else if (dynamic_cast<C*>(p))
    std::cout << "C" << std::endl;
  else
    std::cout << "Unknown" << std::endl;
}

// One candidate test for the reference overload: announces `name` and returns
// true when p really is a T, swallows the throw and returns false otherwise so
// the caller can try the next type. Same algorithm for every T, which is what
// makes this worth a template — unlike ex00's four printers, where each target
// type has its own rules and a template would need four specializations.
//
// ponytail: catch (...) instead of catch (std::bad_cast) — naming bad_cast
// requires <typeinfo>, which the subject forbids. dynamic_cast<T&> throwing
// on mismatch is the only signal we need; no pointer is used in here.

/// this trick use because `dynamic_cast<T&>` performs the type check at runtime,
// and fr a reference the only ailure signal is an exception.
template <typename T>
static bool tryRef(Base& p, const char* name) {
  try {
    // try to determine if p is actually a type T if false we catch the error.
    (void)dynamic_cast<T&>(p);
    std::cout << name << std::endl;
    return true;
  } catch (...) {   // ... is a sugar yntax in C++ for a catch-all handler, a generic throw that says we ddon't care what exceptin was thrown. if anything throws here, consider the cast unsuccesful
    return false;
  }
}

// compared to its homologue above, Base& p cannot be null so c++ cannnot use nullptr to indicate failure
// instead we need a try catch 
void identify(Base& p) {
  if (tryRef<A>(p, "A")) return;
  if (tryRef<B>(p, "B")) return;
  if (tryRef<C>(p, "C")) return;
  std::cout << "Unknown" << std::endl;
}
