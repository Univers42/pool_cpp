/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IMateriaSource.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:18:31 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 18:06:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX03_IMATERIASOURCE_HPP_
#define CPP_MODULE04_EX03_IMATERIASOURCE_HPP_

#include <string>

class AMateria;  // Forward declaration

class IMateriaSource {
 public:
  // as the implementation is still base case, 
  // ICharacter* c = new Character("Bob");
  // delete c;
  // must work ssaffely.. without a virtual destructor, delting thorugh the interface
  // pointer would be undefined behavior
  // its' the standard way to do it.
  virtual ~IMateriaSource() {}
  virtual void learnMateria(AMateria*) = 0;
  virtual AMateria* createMateria(std::string const& type) = 0;
};

#endif  // CPP_MODULE04_EX03_IMATERIASOURCE_HPP_
