/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Brain.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:54:50 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 00:07:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX02_BRAIN_HPP_
#define CPP_MODULE04_EX02_BRAIN_HPP_

#include <iostream>
#include <string>

#define RESET "\033[0m"
#define DIM "\033[2m"

class Brain {
 public:
  std::string ideas[100];

  // Orthodox Canonical Form
  Brain();
  Brain(const Brain& src);
  Brain& operator=(const Brain& rhs);
  ~Brain();
};

#endif  // CPP_MODULE04_EX02_BRAIN_HPP_
