/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:49:24 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "AAnimal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"

int main() {
  // AAnimal a;                        // compile error: AAnimal is abstract
  // const AAnimal* p = new AAnimal(); // compile error: AAnimal is abstract

  const AAnimal* j = new Dog();
  const AAnimal* i = new Cat();

  std::cout << j->getType() << " " << std::endl;
  std::cout << i->getType() << " " << std::endl;
  i->makeSound();
  j->makeSound();

  // Deep copy from ex01 still works.
  Dog basic;
  basic.getBrain()->ideas[0] = "chase tail";
  {
    Dog copy(basic);
    copy.getBrain()->ideas[0] = "sleep";
  }
  std::cout << "original idea: " << basic.getBrain()->ideas[0] << std::endl;

  delete j;  // virtual dtor: Dog then AAnimal
  delete i;
  return 0;
}
