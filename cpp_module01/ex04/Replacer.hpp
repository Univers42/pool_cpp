/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replacer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:30:18 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:50:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX04_REPLACER_HPP_
#define CPP_MODULE01_EX04_REPLACER_HPP_

#include <string>

class Replacer {
 private:
  std::string inFile;
  std::string outFile;
  std::string s1;
  std::string s2;

 public:
  Replacer(const std::string& filename, const std::string& s1,
           const std::string& s2);
  ~Replacer();

  bool process();
};

#endif  // CPP_MODULE01_EX04_REPLACER_HPP_
