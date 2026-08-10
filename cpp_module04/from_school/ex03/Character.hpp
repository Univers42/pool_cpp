/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:16:44 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/06 19:42:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Character.hpp
#ifndef CPP_MODULE04_EX03_CHARACTER_HPP_
#define CPP_MODULE04_EX03_CHARACTER_HPP_

#include <string>
#include "AMateria.hpp"
#include "ICharacter.hpp"

class Character : public ICharacter {
 private:
  std::string _name;
  AMateria* _inventory[4];

 public:
  Character();
  explicit Character(std::string const& name);
  Character(const Character& src);
  Character& operator=(const Character& rhs);
  virtual ~Character();

  virtual std::string const& getName() const;
  virtual void equip(AMateria* m);
  virtual void unequip(int idx);
  virtual void use(int idx, ICharacter& target);
};
#endif  // CPP_MODULE04_EX03_CHARACTER_HPP_
