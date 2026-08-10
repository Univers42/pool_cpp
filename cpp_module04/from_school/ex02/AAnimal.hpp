/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAnimal.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:04:31 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 00:04:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX02_AANIMAL_HPP_
#define CPP_MODULE04_EX02_AANIMAL_HPP_

#include <iostream>
#include <string>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"

class AAnimal {
 protected:
  std::string type;

 public:
  AAnimal();
  AAnimal(const AAnimal& src);
  AAnimal& operator=(const AAnimal& rhs);
  virtual ~AAnimal();

  std::string getType() const;

  // The '= 0' makes this a Pure Virtual Function.
  // This completely prevents anyone from instantiating AAnimal directly!
  virtual void makeSound() const = 0;
};

#endif  // CPP_MODULE04_EX02_AANIMAL_HPP_
