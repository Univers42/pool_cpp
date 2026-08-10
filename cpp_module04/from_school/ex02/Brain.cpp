/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Brain.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:55:01 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/28 23:57:09 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Brain.hpp"

#include <iostream>

Brain::Brain() {
  std::cout << DIM << "    Brain default constructor called" << RESET
            << std::endl;
}

Brain::Brain(const Brain& src) {
  std::cout << DIM << "    Brain copy constructor called" << RESET << std::endl;
  for (int i = 0; i < 100; ++i) {
    this->ideas[i] = src.ideas[i];
  }
}

Brain& Brain::operator=(const Brain& rhs) {
  std::cout << DIM << "    Brain assignment operator called" << RESET
            << std::endl;
  if (this != &rhs) {
    for (int i = 0; i < 100; ++i) {
      this->ideas[i] = rhs.ideas[i];
    }
  }
  return *this;
}

Brain::~Brain() {
  std::cout << DIM << "    Brain destructor called" << RESET << std::endl;
}
