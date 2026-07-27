/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   iter.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 11:37:11 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 11:47:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE07_EX01_ITER_HPP_
#define CPP_MODULE07_EX01_ITER_HPP_

#include <cstddef>  // required for size_t

/**
 * @brief Applies f to every element of array.
 *
 * T deduces the element type including const-ness (a const array yields
 * T = const X), and F deduces the callable's exact type, so f may take its
 * argument by const or non-const reference, and may be an instantiated
 * function template.
 *
 * @param array address of the array.
 * @param length number of elements, passed as a const value.
 * @param f function called on each element.
 */
template <typename T, typename F>
void iter(T* array, const size_t length, F f) {
  for (size_t i = 0; i < length; ++i) f(array[i]);
}

#endif  // CPP_MODULE07_EX01_ITER_HPP_
