/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cat.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:40 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 00:09:25 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX02_CAT_HPP_
#define CPP_MODULE04_EX02_CAT_HPP_

#include "AAnimal.hpp"
#include "Brain.hpp"

class Cat : public AAnimal {
 private:
  Brain* _brain;

 public:
  Cat();
  Cat(const Cat& src);
  Cat& operator=(const Cat& rhs);
  virtual ~Cat();

  virtual void makeSound() const;
  Brain* getBrain() const;
};

#endif  // CPP_MODULE04_EX02_CAT_HPP_
