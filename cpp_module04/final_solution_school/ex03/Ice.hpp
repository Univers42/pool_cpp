/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ice.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:15:48 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:25:48 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Ice.hpp
#ifndef CPP_MODULE04_EX03_ICE_HPP_
#define CPP_MODULE04_EX03_ICE_HPP_
#include "AMateria.hpp"

class Ice : public AMateria {
 public:
  Ice();
  Ice(const Ice& src);
  Ice& operator=(const Ice& rhs);
  virtual ~Ice();

  virtual AMateria* clone() const;
  virtual void use(ICharacter& target);
};
#endif  // CPP_MODULE04_EX03_ICE_HPP_
