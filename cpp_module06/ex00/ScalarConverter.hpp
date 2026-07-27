/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScalarConverter.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:43:38 by marvin            #+#    #+#             */
/*   Updated: 2025/12/25 21:43:38 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE06_EX00_SCALARCONVERTER_HPP_
#define CPP_MODULE06_EX00_SCALARCONVERTER_HPP_

#include <string>

// Subject: "ONLY one static method convert"; not instantiable by users.
class ScalarConverter {
 public:
  static void convert(const std::string& literal);

 private:
  // ponytail: C++98 non-instantiable idiom — declared private, never defined.
  ScalarConverter();
  ScalarConverter(const ScalarConverter& other);
  ScalarConverter& operator=(const ScalarConverter& other);
  ~ScalarConverter();
};

#endif  // CPP_MODULE06_EX00_SCALARCONVERTER_HPP_
