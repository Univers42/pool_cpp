/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:47:25 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 00:03:59 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX02_ANIMAL_HPP_
#define CPP_MODULE04_EX02_ANIMAL_HPP_

#include <iostream>
#include <string>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"

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

#endif  // CPP_MODULE04_EX02_ANIMAL_HPP_
