/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:47:25 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 17:35:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX01_ANIMAL_HPP_
#define CPP_MODULE04_EX01_ANIMAL_HPP_

#include <string>

// Polymorphic base: virtual makeSound() gives dynamic dispatch, virtual
// destructor guarantees the derived destructor runs on delete via Animal*.
class Animal {
 protected:
  std::string type;

 public:
  Animal();
  explicit Animal(const std::string&);
  Animal(const Animal& src);
  Animal& operator=(const Animal& rhs);
  virtual ~Animal();

  std::string getType() const;
  virtual void makeSound() const;
};
#endif  // CPP_MODULE04_EX01_ANIMAL_HPP_
