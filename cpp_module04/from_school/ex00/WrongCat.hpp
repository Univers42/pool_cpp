/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongCat.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:49:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:49:13 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX00_WRONGCAT_HPP_
#define CPP_MODULE04_EX00_WRONGCAT_HPP_

#include "WrongAnimal.hpp"

/**
 * @class WrongCat 
 * @breif A derived class inheriting from the flawed WrongAnimal base class.
 */
class WrongCat : public WrongAnimal {
 public:
  WrongCat();
  WrongCat(const WrongCat& src);
  WrongCat& operator=(const WrongCat& rhs);
  virtual ~WrongCat();

  /**
   * @brief Attempts to make a specific cat sound.
   * because the base class Wronganimal::makeSound() is not virtual,
   * this method onnly hides the base method (method hiding), it does not
   * override it. It will not be called if the object is accessed via a 
   * WrongAnimal pointer
   */
  void makeSound() const;
};

#endif  // CPP_MODULE04_EX00_WRONGCAT_HPP_
