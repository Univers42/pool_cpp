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

#include "Cat.hpp"
#include "Dog.hpp"

int main() {
  // Subject-mandated test: delete through Animal* must not leak.
  const Animal* j = new Dog();
  const Animal* i = new Cat();

  delete j;  // should not create a leak
  delete i;

  std::cout << "\n--- array of Animal*: half Dog, half Cat ---" << std::endl;
  Animal* animals[10];
  for (int k = 0; k < 10; ++k) {
    if (k < 5)
      animals[k] = new Dog();
    else
      animals[k] = new Cat();
  }
  for (int k = 0; k < 10; ++k) animals[k]->makeSound();
  // Deleted directly as Animal*: virtual dtor runs Dog/Cat dtor + Brain free.
  for (int k = 0; k < 10; ++k) delete animals[k];

  std::cout << "\n--- deep copy: mutating the copy leaves the original ---"
            << std::endl;
  Dog original;
  original.getBrain()->ideas[0] = "chase the mailman";
  {
    Dog copy(original);
    copy.getBrain()->ideas[0] = "sleep all day";
    std::cout << "original idea: " << original.getBrain()->ideas[0]
              << std::endl;
    std::cout << "copy idea:     " << copy.getBrain()->ideas[0] << std::endl;
  }  // copy destroyed here: its own Brain is freed, original's survives
  std::cout << "original idea after copy died: "
            << original.getBrain()->ideas[0] << std::endl;

  return 0;
}
