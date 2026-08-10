/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Contact.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 19:58:06 by marvin            #+#    #+#             */
/*   Updated: 2026/03/01 18:03:43 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE00_EX01_CONTACT_HPP_
#define CPP_MODULE00_EX01_CONTACT_HPP_

#include <string>

class Contact {
 public:
  enum Field {
    FIELD_NAME = 0,
    FIELD_LASTNAME,
    FIELD_NICKNAME,
    FIELD_PHONE,
    FIELD_SECRET,
    FIELD_COUNT
  };

  Contact();
  void setField(Field f, const std::string& v);
  const std::string& getField(Field f) const;

 private:
  std::string fields[FIELD_COUNT];
  static const std::string& emptyString();
};

#endif  // CPP_MODULE00_EX01_CONTACT_HPP_
