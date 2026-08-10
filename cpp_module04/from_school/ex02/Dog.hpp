/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:13 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 00:04:52 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX02_DOG_HPP_
#define CPP_MODULE04_EX02_DOG_HPP_

#include "AAnimal.hpp"
#include "Brain.hpp"

class Dog : public AAnimal {  // Inherits from AAnimal now
 private:
  Brain* _brain;

 public:
  Dog();
  Dog(const Dog& src);
  Dog& operator=(const Dog& rhs);
  virtual ~Dog();

  virtual void makeSound() const;
  Brain* getBrain() const;
};

#endif  // CPP_MODULE04_EX02_DOG_HPP_
