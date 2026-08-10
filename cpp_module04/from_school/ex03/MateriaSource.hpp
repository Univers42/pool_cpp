/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MateriaSource.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:17:18 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:27:46 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// MateriaSource.hpp
#ifndef CPP_MODULE04_EX03_MATERIASOURCE_HPP_
#define CPP_MODULE04_EX03_MATERIASOURCE_HPP_

#include <string>
#include "AMateria.hpp"
#include "IMateriaSource.hpp"

class MateriaSource : public IMateriaSource {
 private:
  AMateria* _templates[4];

 public:
  MateriaSource();
  MateriaSource(const MateriaSource& src);
  MateriaSource& operator=(const MateriaSource& rhs);
  virtual ~MateriaSource();

  virtual void learnMateria(AMateria*);
  virtual AMateria* createMateria(std::string const& type);
};
#endif  // CPP_MODULE04_EX03_MATERIASOURCE_HPP_
