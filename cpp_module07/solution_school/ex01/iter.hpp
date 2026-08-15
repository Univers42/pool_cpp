/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   iter.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 11:37:11 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/13 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE07_EX01_ITER_HPP_
#define CPP_MODULE07_EX01_ITER_HPP_

#include <cstddef>  // size_t

// Two template parameters, and both of them earn their place.
//
// T deduces the element type WITH its const, so a const array gives
// T = const X and a callback taking X& refuses to bind. I never wrote a second
// overload for const; the compiler does it.
//
// F deduces the callable's own type instead of pinning a signature like
// void (*)(T&). That is why a functor works here, and an instantiated function
// template, and a callback that returns something.
template <typename T, typename F>
void iter(T* array, const size_t length, F f) {
  for (size_t i = 0; i < length; ++i) f(array[i]);
}

#endif  // CPP_MODULE07_EX01_ITER_HPP_
