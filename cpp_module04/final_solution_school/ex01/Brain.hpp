/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Brain.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:54:50 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 17:46:03 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX01_BRAIN_HPP_
#define CPP_MODULE04_EX01_BRAIN_HPP_

#include <string>

/**
 * @class Brain
 * @brief Exists to force the difference between SHALLOW and DEEP copy.
 */
class Brain {
  private:
    std::string _ideas[100];
    
  public:
    Brain();
    Brain(const Brain&);
    Brain& operator=(const Brain&);
    ~Brain();

    // member functions
    void setIdea(int, const std::string&);
    std::string getIdea(int) const;
};

#endif  // CPP_MODULE04_EX01_BRAIN_HPP_
