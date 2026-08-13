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
  /**
    because convert() is a static member function, we can call it without creating
    a Scalarconverter object
    Normally, a member function requires an object:
    class Foo {
      public:
        void hello();
    }
      Foo foo;
      foo.hello();
    but with static void hello()
    Foo::hello(); no object is needed..
    
    **Static** means this ffunction belongs to the class itself rather than to an individual object.
    we need to access it through the class:
    ScalarConverter::convert("42");
    it isn't a global function like:
    convert("42");

    we also need convert public because our user of the class ned to be able to call it. 
    We want our user of the class need to be able to cal it. 
    Therefore convert() must be accessible from outside the class:
    if we would have put convert in private then
    ScalarConverter::convert("42") -> [ERR] compiltion ❌
    private means that only the class itself can access this member
    so we can do that
    Imagine we have the constructor possible
    ScalarConverter(): _number(convert("42"));
  */
  static void convert(const std::string& literal);

 private:
  // ponytail: C++98 non-instantiable idiom — declared private, never defined.
  ScalarConverter();
  ScalarConverter(const ScalarConverter& other);
  ScalarConverter& operator=(const ScalarConverter& other);
  ~ScalarConverter();
};

#endif  // CPP_MODULE06_EX00_SCALARCONVERTER_HPP_
