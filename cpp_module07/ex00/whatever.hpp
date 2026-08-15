/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   whatever.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 11:18:38 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/13 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE07_EX00_WHATEVER_HPP_
#define CPP_MODULE07_EX00_WHATEVER_HPP_

template <typename T>
void swap(T& a, T& b) {
  T temp = a;
  a = b;
  b = temp;
}

// The subject wants the second argument back when the two are equal. The
// ternary already does that, since a < b is false on a tie and it falls
// through to b, so there is no equality case to write.
//
// const T& matters twice: nothing gets copied just to be compared, and the
// caller can check WHICH argument came back. Returning a copy would make the
// tie rule impossible to observe.
template <typename T>
const T& min(const T& a, const T& b) {
  return ((a < b) ? a : b);
}

template <typename T>
const T& max(const T& a, const T& b) {
  return ((a > b) ? a : b);
}

#endif  // CPP_MODULE07_EX00_WHATEVER_HPP_
