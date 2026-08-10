/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Intern.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:40:07 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE05_EX03_INTERN_HPP_
#define CPP_MODULE05_EX03_INTERN_HPP_

#include <string>

// makeForm() hands back an AForm* but never dereferences one here, so a
// declaration is enough — Intern.cpp is the only file that needs the concrete
// form headers, and no user of Intern is forced to include all three.
class AForm;

class Intern {
 public:
  Intern();
  Intern(const Intern& other);
  Intern& operator=(const Intern& other);
  ~Intern();
  AForm* makeForm(const std::string& formName, const std::string& target) const;
};

#endif  // CPP_MODULE05_EX03_INTERN_HPP_
