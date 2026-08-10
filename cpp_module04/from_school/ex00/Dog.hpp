/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:13 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 18:13:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX00_DOG_HPP_
#define CPP_MODULE04_EX00_DOG_HPP_

#include "Animal.hpp"

/**
 * @class Dog
 * @brief A specific type of Animal that demonstrates polymorphic behavior
 */
class Dog : public Animal {
 public:
  /**
   * @brief Default ctor
   * Initialized the type to "Dog"
   */
  Dog();
  /**
   * @brief Copy constructor.
   * @param src The source Dog object to copy.
   */
  Dog(const Dog& src);
  /**
   * @brief Copy assignment operator.
   * Safely assigns the base Animal components and the specific Dog components.
   * @param rhs The right-hand side Dog object to assign from.
   * @return A reference to the newly assigned object.
   */
  Dog& operator=(const Dog& rhs);
  /**
   * @brief Virtual dtor
   * Ensure proper cleanup of Dog resources when deleted via an Animal pointer
   */
  virtual ~Dog();

  /**
   * @brief Overriden virtual function to make a dog sound
   * Outputs a specific barking sound. Called dynamically at runtime
   */
  virtual void makeSound() const;
};

#endif  // CPP_MODULE04_EX00_DOG_HPP_
