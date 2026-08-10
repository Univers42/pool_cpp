/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:49:24 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 18:38:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Cat.hpp"
#include "Dog.hpp"
#include "utils.hpp"

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
  original.getBrain()->setIdea(0, "chase the mailman");
  {
    Dog copy(original);
    copy.getBrain()->setIdea(0, "sleep all day");
    std::cout << "original idea: " << original.getBrain()->getIdea(0)
              << std::endl;
    std::cout << "copy idea:     " << copy.getBrain()->getIdea(0) << std::endl;
  }  // copy destroyed here: its own Brain is freed, original's survives
  std::cout << "original idea after copy died: "
            << original.getBrain()->getIdea(0) << std::endl;

  {
    banner("COPY ASSIGNMENT");

    Dog original;
    original.getBrain()->setIdea(0, "Bone");

    Dog copy;
    copy.getBrain()->setIdea(0, "Ball");

    copy = original;
    copy.getBrain()->setIdea(0, "Steak");

    std::cout << "original : " << original.getBrain()->getIdea(0) << std::endl;
    std::cout << "copy     : " << copy.getBrain()->getIdea(0) << std::endl;

    // Expected:
    // original -> Bone
    // copy     -> Steak
  }

  {
    banner("BRAIN ADDRESS");

    Dog original;
    Dog copy(original);

    std::cout << original.getBrain() << std::endl;
    std::cout << copy.getBrain() << std::endl;

    // Expected:
    // Different addresses
  }

  {
    banner("COPY ALL IDEAS");

    Dog dog;

    for (int idx = 0; idx < 100; ++idx)
      dog.getBrain()->setIdea(idx, "Idea");

    Dog copy(dog);

    for (int idx = 0; idx < 100; ++idx) {
      if (dog.getBrain()->getIdea(idx) != copy.getBrain()->getIdea(idx))
        std::cout << "ERROR at idea " << idx << std::endl;
    }
  }

  {
    banner("MODIFY COPY");

    Dog original;

    for (int idx = 0; idx < 100; ++idx)
      original.getBrain()->setIdea(idx, "Original");

    Dog copy(original);

    for (int idx = 0; idx < 100; ++idx)
      copy.getBrain()->setIdea(idx, "Copy");

    std::cout << original.getBrain()->getIdea(42) << std::endl;
    std::cout << copy.getBrain()->getIdea(42) << std::endl;

    // Expected:
    // Original
    // Copy
  }

  {
    banner("SELF ASSIGNMENT");

    Dog dog;
    dog.getBrain()->setIdea(0, "Hello");

    Dog& ref = dog;
    dog = ref;

    std::cout << dog.getBrain()->getIdea(0) << std::endl;

    // Expected:
    // Hello
  }

  {
    banner("CONSTRUCTION / DESTRUCTION");

    for (int idx = 0; idx < 20; ++idx) {
      Dog dog;
    }

    // Expected:
    // No leaks under Valgrind
  }

  {
    banner("ARRAY OF DOGS");

    Dog dogs[5];

    dogs[0].getBrain()->setIdea(0, "A");
    dogs[1].getBrain()->setIdea(0, "B");

    std::cout << dogs[0].getBrain()->getIdea(0) << std::endl;
    std::cout << dogs[1].getBrain()->getIdea(0) << std::endl;

    // Expected:
    // A
    // B
  }

  {
    banner("CHAIN OF COPIES");

    Dog a;
    a.getBrain()->setIdea(0, "A");

    Dog b(a);
    Dog c(b);

    c.getBrain()->setIdea(0, "C");

    std::cout << a.getBrain()->getIdea(0) << std::endl;
    std::cout << b.getBrain()->getIdea(0) << std::endl;
    std::cout << c.getBrain()->getIdea(0) << std::endl;

    // Expected:
    // A
    // A
    // C
  }

  {
    banner("HEAP COPY");

    Dog* original = new Dog();
    original->getBrain()->setIdea(0, "Original");

    Dog* copy = new Dog(*original);
    copy->getBrain()->setIdea(0, "Copy");

    std::cout << original->getBrain()->getIdea(0) << std::endl;
    std::cout << copy->getBrain()->getIdea(0) << std::endl;

    delete original;
    delete copy;

    // Expected:
    // Original
    // Copy
  }

  {
    banner("DEEP COPY CHECK");

    Dog original;
    Dog copy(original);

    std::cout << "Original Brain: " << original.getBrain() << std::endl;
    std::cout << "Copy Brain    : " << copy.getBrain() << std::endl;

    if (original.getBrain() != copy.getBrain())
      std::cout << "Deep copy OK" << std::endl;
    else
      std::cout << "Shallow copy!" << std::endl;
  }
  return 0;
}