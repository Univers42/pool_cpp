/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ICharacter.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:15:20 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:25:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX03_ICHARACTER_HPP_
#define CPP_MODULE04_EX03_ICHARACTER_HPP_

#include <string>

class AMateria;  // Forward declaration

class ICharacter {
 public:
  virtual ~ICharacter() {}
  virtual std::string const& getName() const = 0;
  virtual void equip(AMateria* m) = 0;
  virtual void unequip(int idx) = 0;
  virtual void use(int idx, ICharacter& target) = 0;
};

#endif  // CPP_MODULE04_EX03_ICHARACTER_HPP_
