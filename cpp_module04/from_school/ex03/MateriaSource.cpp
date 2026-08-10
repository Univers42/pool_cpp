/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MateriaSource.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:17:24 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:26:52 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// MateriaSource.cpp
#include "MateriaSource.hpp"
#include <string>

MateriaSource::MateriaSource() {
  for (int i = 0; i < 4; i++) _templates[i] = NULL;
}

MateriaSource::MateriaSource(const MateriaSource& src) {
  for (int i = 0; i < 4; i++) {
    if (src._templates[i])
      this->_templates[i] = src._templates[i]->clone();
    else
      this->_templates[i] = NULL;
  }
}

MateriaSource& MateriaSource::operator=(const MateriaSource& rhs) {
  if (this != &rhs) {
    for (int i = 0; i < 4; i++) {
      if (this->_templates[i]) delete this->_templates[i];
      if (rhs._templates[i])
        this->_templates[i] = rhs._templates[i]->clone();
      else
        this->_templates[i] = NULL;
    }
  }
  return *this;
}

MateriaSource::~MateriaSource() {
  for (int i = 0; i < 4; i++) {
    if (this->_templates[i]) delete this->_templates[i];
  }
}

void MateriaSource::learnMateria(AMateria* m) {
  if (!m) return;
  for (int i = 0; i < 4; i++) {
    if (this->_templates[i] == NULL) {
      this->_templates[i] = m;
      return;
    }
  }
  delete m;  // If inventory is full, delete to avoid leak
}

AMateria* MateriaSource::createMateria(std::string const& type) {
  for (int i = 0; i < 4; i++) {
    if (this->_templates[i] && this->_templates[i]->getType() == type) {
      return this->_templates[i]->clone();
    }
  }
  return NULL;  // Return 0 if type is unknown
}
