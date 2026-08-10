/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:47:25 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/06 15:10:57 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX00_ANIMAL_HPP_
#define CPP_MODULE04_EX00_ANIMAL_HPP_

#include <iostream>
#include <string>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"

/**
 * @brief The entire purpose of writing `WrongAnimal` alongside `animal`
 * is to force us to see the difference between static and dynamic binding
 * side-by-side. We should learn that any-time we expect a class to be used
 * as a base class and manipulated to be overriden must be virtual
*/


/**
 * @class Animal
 * @brief A class representing a generic animal.
 * this class serves as the foundation for speiffic animal types
 * Dog and Cat
 * POLYMORPHISM: it is designed to demonstratae polymorphism
 * dynamic binding by using virtual functions, allowing derived classes
 * to override base behaviors
 */
class Animal {
 protected:
  /**
   * @brief The specific type or species of the animal
   * @protected so that derived classes (like Dog and Cat) can access
   * and mofifyu this value directly using their construction
   */
  std::string type;

 public:

  /**
   * @brief Default ctor
   * initializes the animal with a default type of "Generic Animal"
   */
  Animal();
  Animal(std::string name);
  
  /**
   * @brief Copy ctor
   * * Creates a new animal object as deep copy of an existing one
   * @param src The source Animal object to copy form
   */
  Animal(const Animal& src);

  /**
   * @brief Copy assignment operator
   * * Assigns the state of one Animal object to another, ensuring self-assignment
   * is handled safely
   * @param rhs The right-hand side Animal object to assign from.
   * @return A  reference to the current updated object (*this)
   */
  Animal& operator=(const Animal& rhs);

  /**
   * @brief virtual dtor
   * * Marked as virtual to guarantee that when ha derived class object (e.g., Cat)
   * is deleted via a base class pointer (animal*),the derived class's dtor ise
   * executed first, preventing memory leaks
   */
  virtual ~Animal();
  
  /**
   *  @brief Retrieves the type of animal.
   *  @return A std::string  containing the animal's type
   */
  std::string getType() const;

  /**
   * @brief Outputs the animal's specific sound
   * * Marked as virtual so the  
   */
  virtual void makeSound() const;
};

#endif  // CPP_MODULE04_EX00_ANIMAL_HPP_
