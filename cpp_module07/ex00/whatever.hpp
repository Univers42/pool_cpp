/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   whatever.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 11:18:38 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:20:07 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE07_EX00_WHATEVER_HPP_
#define CPP_MODULE07_EX00_WHATEVER_HPP_

/**
 * @brief Swaps the values of two variables.
 * @tparam T The type of the variables.
 * @param a Reference to the first variable.
 * @param b Reference to the second variable.
 */
template <typename T>
void swap(T& a, T& b) {
  T temp = a;
  a = b;
  b = temp;
}

/**
 * @brief Compares two values and returns the smallest one.
 * If the values are equal, it returns the second one.
 * @tparam T The type of the values.
 * @param a Constant reference to the first value.
 * @param b Constant reference to the second value.
 * @return const T& Reference to the smallest value.
 */
template <typename T>
const T& min(const T& a, const T& b) {
  return ((a < b) ? a : b);
}

/**
 * @brief Compares two values and returns the greatest one.
 * If the values are equal, it returns the second one.
 * @tparam T The type of the values.
 * @param a Constant reference to the first value.
 * @param b Constant reference to the second value.
 * @return const T& Reference to the greatest value.
 */
template <typename T>
const T& max(const T& a, const T& b) {
  return ((a > b) ? a : b);
}

#endif  // CPP_MODULE07_EX00_WHATEVER_HPP_
