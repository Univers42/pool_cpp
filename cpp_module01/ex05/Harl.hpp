/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Harl.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:50:34 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:51:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX05_HARL_HPP_
#define CPP_MODULE01_EX05_HARL_HPP_

#include <string>

class Harl {
 private:
  void debug(void);
  void info(void);
  void warning(void);
  void error(void);

 public:
  Harl();
  ~Harl();
  void complain(std::string level);
};

#endif  // CPP_MODULE01_EX05_HARL_HPP_
