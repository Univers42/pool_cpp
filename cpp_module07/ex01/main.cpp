/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 11:37:09 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 11:48:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

#include "iter.hpp"

// Non-const callback: mutates elements in place.
void increment(int& n) { n++; }

// Const callback: read-only access.
void printInt(const int& n) { std::cout << n << " "; }

// Function template; passed to iter as an instantiated template.
template <typename T>
void print(const T& elem) {
  std::cout << elem << " ";
}

int main(void) {
  int ints[] = {1, 2, 3, 4, 5};
  const size_t intLen = sizeof(ints) / sizeof(ints[0]);

  std::cout << "before increment: ";
  ::iter(ints, intLen, printInt);
  std::cout << std::endl;

  ::iter(ints, intLen, increment);

  std::cout << "after increment:  ";
  ::iter(ints, intLen, printInt);
  std::cout << std::endl;

  // const array: T deduces as const std::string, and the callback is an
  // instantiated function template, as the subject requires.
  const std::string strs[] = {"Hello", "42", "C++98"};
  ::iter(strs, sizeof(strs) / sizeof(strs[0]), print<std::string>);
  std::cout << std::endl;

  return 0;
}
