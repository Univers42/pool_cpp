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

// Stand-in for the evaluation sheet's `ex01.cpp` attachment.
//
// The sheet's link to the attachment (https://github.com/rphlr/42-Subjects/)
// is a 404, so the original is gone. This is rebuilt from the one thing the
// sheet pins down exactly — the ex01 station reads:
//
//   "Test the code ex01.cpp (in attachments) with the iter of the evaluated
//    student. If everything went well, it should display:
//        0 1 2 3 4 42 42 42 42 42"   (one per line)
//
// Which fixes the shape of the file: a five-element int array 0..4, printed
// through iter; every element overwritten with 42 through iter; printed again
// through iter. Nothing else can produce that transcript.

#include <iostream>

#include "../iter.hpp"

// Takes its argument by const reference: iter must be able to pass elements to
// a read-only callback without copying them.
template <typename T>
void print(const T& x) {
  std::cout << x << std::endl;
}

// Takes its argument by non-const reference: iter must hand out the real
// element, not a copy, or this write is lost.
template <typename T>
void setToFortyTwo(T& x) {
  x = 42;
}

int main(void) {
  int tab[] = {0, 1, 2, 3, 4};
  const size_t len = sizeof(tab) / sizeof(tab[0]);

  ::iter(tab, len, print<int>);
  ::iter(tab, len, setToFortyTwo<int>);
  ::iter(tab, len, print<int>);

  return 0;
}
