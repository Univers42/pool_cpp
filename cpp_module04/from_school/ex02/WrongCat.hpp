/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongCat.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:49:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 00:03:59 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX02_WRONGCAT_HPP_
#define CPP_MODULE04_EX02_WRONGCAT_HPP_

#include <iostream>

#include "WrongAnimal.hpp"

class WrongCat : public WrongAnimal {
 public:
  WrongCat();
  WrongCat(const WrongCat& src);
  WrongCat& operator=(const WrongCat& rhs);
  virtual ~WrongCat();

  void makeSound() const;
};

#endif  // CPP_MODULE04_EX02_WRONGCAT_HPP_
