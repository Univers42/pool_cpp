/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 15:15:39 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 18:19:36 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE00_EX01_PHONEBOOK_HPP_
#define CPP_MODULE00_EX01_PHONEBOOK_HPP_

#include "Contact.hpp"

#define MAX_CONTACT 8

class PhoneBook {
 private:
  Contact contact[MAX_CONTACT];
  int cap;
  int size;
  int count;

 public:
  PhoneBook();
  void add();
  void search() const;
  void seed();

  int getCount() const;
  const Contact& getContact(int idx) const;
  bool pushContact(const Contact& c);
};

#endif  // CPP_MODULE00_EX01_PHONEBOOK_HPP_
