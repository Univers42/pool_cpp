/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 22:01:48 by marvin            #+#    #+#             */
/*   Updated: 2025/12/25 22:01:48 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE06_EX02_BASE_HPP_
#define CPP_MODULE06_EX02_BASE_HPP_

// ponytail: subject asks for a public virtual destructor ONLY; body lives in
// Base.cpp because 42 rules forbid function implementations in headers.
class Base {
 public:
  virtual ~Base();
};

#endif  // CPP_MODULE06_EX02_BASE_HPP_
