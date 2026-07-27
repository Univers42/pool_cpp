/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:47:25 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:59:06 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX01_ANIMAL_HPP_
#define CPP_MODULE04_EX01_ANIMAL_HPP_

#include <string>

class Animal {
 protected:
  std::string type;  // Protected attribute as requested

 public:
  Animal();
  Animal(const Animal& src);
  Animal& operator=(const Animal& rhs);
  virtual ~Animal();  // Virtual destructor is strictly mandatory here

  std::string getType() const;

  // Virtual allows derived classes to override this behavior
  // Must be const because subject tests it with 'const Animal*'
  virtual void makeSound() const;
};

#endif  // CPP_MODULE04_EX01_ANIMAL_HPP_
