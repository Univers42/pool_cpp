/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cure.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:16:33 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:25:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Cure.hpp
#ifndef CPP_MODULE04_EX03_CURE_HPP_
#define CPP_MODULE04_EX03_CURE_HPP_
#include "AMateria.hpp"

class Cure : public AMateria {
 public:
  Cure();
  Cure(const Cure& src);
  Cure& operator=(const Cure& rhs);
  virtual ~Cure();

  virtual AMateria* clone() const;
  virtual void use(ICharacter& target);
};
#endif  // CPP_MODULE04_EX03_CURE_HPP_
