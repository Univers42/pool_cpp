/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMateria.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:15:35 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:25:33 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX03_AMATERIA_HPP_
#define CPP_MODULE04_EX03_AMATERIA_HPP_

#include <iostream>
#include <string>

#include "ICharacter.hpp"

class AMateria {
 protected:
  std::string type;

 public:
  AMateria();
  explicit AMateria(std::string const& type);
  AMateria(const AMateria& src);
  AMateria& operator=(const AMateria& rhs);
  virtual ~AMateria();

  std::string const& getType() const;  // Returns the materia type
  virtual AMateria* clone() const = 0;
  virtual void use(ICharacter& target);
};

#endif  // CPP_MODULE04_EX03_AMATERIA_HPP_
