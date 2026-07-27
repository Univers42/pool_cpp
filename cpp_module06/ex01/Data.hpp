/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Data.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:55:30 by marvin            #+#    #+#             */
/*   Updated: 2025/12/25 21:55:30 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE06_EX01_DATA_HPP_
#define CPP_MODULE06_EX01_DATA_HPP_

#include <string>

// ponytail: plain aggregate, no ctors -- keeps the header free of function
// bodies (module rule); brace-init at call sites. Add a Data.cpp if ctors are
// ever needed.
struct Data {
  int id;
  double value;
  std::string name;
};

#endif  // CPP_MODULE06_EX01_DATA_HPP_