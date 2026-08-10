/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Brain.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:55:01 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 17:50:05 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Brain.hpp"

#include <iostream>

Brain::Brain() { 
  #ifdef DEBUG
    std::cout << "Brain constructor called" << std::endl;
  #endif
}

Brain::Brain(const Brain& src) {
  #ifdef DEBUG
    std::cout << "Brain copy constructor called" << std::endl;
  #endif
  for (int i = 0; i < 100; ++i) {
    _ideas[i] = src._ideas[i];
  }
}

Brain& Brain::operator=(const Brain& rhs) {
  #ifdef DEBUG
    std::cout << "Brain copy assignment operator called" << std::endl;
  #endif
  if (this != &rhs) {
    for (int i = 0; i < 100; ++i) {
      _ideas[i] = rhs._ideas[i];
    }
  }
  return (*this);
}

Brain::~Brain() {
  #ifdef DEBUG
    std::cout << "Brain destructor called" << std::endl;
  #endif
}

void Brain::setIdea(int idx, const std::string& idea){ 
  if (idx >= 0 && idx < 100)
    _ideas[idx] = idea;
}

std::string Brain::getIdea(int idx) const {
  if (idx >= 0 && idx < 100)
    return _ideas[idx];
  /**
    because if idx is not in [0,99]m, the function reached the end without returning anything
    std::cout << brain.getIdea(150);
    The compiler may warn:
    control reaches end of non-void funciton
    this is undefined behavior
    just need to return something by default like empty string to avoid this gap
  */
  return "";
}