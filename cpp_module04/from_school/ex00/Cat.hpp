/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cat.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:40 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 18:13:36 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX00_CAT_HPP_
#define CPP_MODULE04_EX00_CAT_HPP_

#include "Animal.hpp"

/**
 * @class Cat
 * @brief A specific type of Animal that demonstraes polymorphic behavior
 */
class Cat : public Animal {
 public:
  /**
   * @class Default constructor
   * Initialized the type to "cat"
   */
  Cat();

  /**
   * @brief Copy ctor
   * @param src The source Cat object to copy
   */
  Cat(const Cat& src);

  /**
   * @brief Copy assignment operator
   * When cat assigns rhs to itself, it first calls the base class
   * assignment operator Animal::operator(rhs) before assigning its own 
   * specific members (this->type=(rhs)) before assigning its own specific
   * members (this->type = rhs.type). This is the correct way to handle
   * assignment in derived classes to ensure the base part of the object is
   * also properly copied
   * 
   * @param rhs the Right-hand side cat object to assign form
   * @return A reference to the newly assigned object
   */
  Cat& operator=(const Cat& rhs);

  /**
   * @brief virtual dtor
   * Ensures proper cleanup of cat resources when deleted via an Animal pointer
   */
  virtual ~Cat();

  /**
   * @brief Overriden virtual function to make a cat sound
   * This function overrides the base Animal::makeSound() to output a specific
   * mewo sound. due to dynamic binding
   */
  virtual void makeSound() const;
};

#endif  // CPP_MODULE04_EX00_CAT_HPP_
