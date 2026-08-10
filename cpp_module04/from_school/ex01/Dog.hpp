/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:13 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 00:00:18 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX01_DOG_HPP_
#define CPP_MODULE04_EX01_DOG_HPP_

#include "Animal.hpp"
#include "Brain.hpp"

class Dog : public Animal {
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

#endif  // CPP_MODULE04_EX01_DOG_HPP_
