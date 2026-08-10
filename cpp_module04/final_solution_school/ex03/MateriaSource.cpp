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
  // a throwing clone() must not leak the clones made before it
  int i = 0;
  try {
    for (; i < 4; i++)
      this->_templates[i] =
          src._templates[i] ? src._templates[i]->clone() : NULL;
  } catch (...) {
    while (i-- > 0) delete this->_templates[i];
    throw;
  }
}

MateriaSource& MateriaSource::operator=(const MateriaSource& rhs) {
  if (this != &rhs) {
    // clone ALL first: a throwing clone must leave our templates untouched
    AMateria* fresh[4];
    int i = 0;
    try {
      for (; i < 4; i++)
        fresh[i] = rhs._templates[i] ? rhs._templates[i]->clone() : NULL;
    } catch (...) {
      while (i-- > 0) delete fresh[i];
      throw;
    }
    for (int j = 0; j < 4; j++) {
      delete this->_templates[j];
      this->_templates[j] = fresh[j];
    }
  }
  return *this;
}

MateriaSource::~MateriaSource() {
  for (int i = 0; i < 4; i++) {
    if (this->_templates[i]) delete this->_templates[i];
  }
}

// The subject says learnMateria "copies the Materia passed as a parameter":
// store a clone, then free the original — callers hand it over
// (src->learnMateria(new Ice())), so deleting it here is what keeps the
// subject's own main leak-free.
void MateriaSource::learnMateria(AMateria* m) {
  if (!m) return;
  for (int i = 0; i < 4; i++) {
    if (this->_templates[i] == NULL) {
      this->_templates[i] = m->clone();
      delete m;
      return;
    }
  }
  delete m;  // Inventory full: nothing learned, but the hand-off is still ours
}

AMateria* MateriaSource::createMateria(std::string const& type) {
  for (int i = 0; i < 4; i++) {
    if (this->_templates[i] && this->_templates[i]->getType() == type) {
      return this->_templates[i]->clone();
    }
  }
  return NULL;  // Return 0 if type is unknown
}
