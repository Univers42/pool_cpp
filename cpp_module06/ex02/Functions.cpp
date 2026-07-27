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

// ponytail: catch (...) instead of catch (std::bad_cast) — naming bad_cast
// requires <typeinfo>, which the subject forbids. dynamic_cast<T&> throwing
// on mismatch is the only signal we need; no pointer is used in here.
void identify(Base& p) {
  try {
    (void)dynamic_cast<A&>(p);
    std::cout << "A" << std::endl;
    return;
  } catch (...) {
  }
  try {
    (void)dynamic_cast<B&>(p);
    std::cout << "B" << std::endl;
    return;
  } catch (...) {
  }
  try {
    (void)dynamic_cast<C&>(p);
    std::cout << "C" << std::endl;
    return;
  } catch (...) {
  }
  std::cout << "Unknown" << std::endl;
}
