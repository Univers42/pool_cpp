/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongAnimal.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:58 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 18:13:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX00_WRONGANIMAL_HPP_
#define CPP_MODULE04_EX00_WRONGANIMAL_HPP_

#include <iostream>
#include <string>

/**
 * @class WrongAnimal
 * @brief A flawed base class for testing static binding
 * This class purposefully omits 'virtual' keyword on its makesound() method
 * to demonstrate how C++ falls back to early (static) binding speed based
 * on pointer type
 */
class WrongAnimal {
 protected:
  std::string type;

 public:
  WrongAnimal();
  WrongAnimal(const WrongAnimal& src);
  WrongAnimal& operator=(const WrongAnimal& rhs);
  virtual ~WrongAnimal();

  /**
   * @brief Makes a ageneric wrong animal sound
   * CRITICAL DIFFERENCE: this method is NOT virtual. If a derived calss
   * pointer is upcast ot a wrongAnimal pointer, calling this method will
   * execute this generic versoin, not the derived version
   */
  std::string getType() const;
  void makeSound() const;
};

#endif  // CPP_MODULE04_EX00_WRONGANIMAL_HPP_
