/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 01:07:41 by marvin            #+#    #+#             */
/*   Updated: 2026/03/01 18:19:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PhoneBook.hpp"

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <iostream>

#include <string>
#include <utility>
#define C_RESET "\033[0m"
#define C_BOLD "\033[1m"
#define C_DIM "\033[2m"
#define C_CYAN "\033[36m"
#define C_YELLOW "\033[33m"
#define C_GREEN "\033[32m"
#define C_RED "\033[31m"

#define BOX_TOP "┌──────────┬──────────┬──────────┬──────────┐"
#define BOX_MID "├──────────┼──────────┼──────────┼──────────┤"
#define BOX_BOT "└──────────┴──────────┴──────────┴──────────┘"
#define V_BAR "│"

PhoneBook::PhoneBook() : cap(MAX_CONTACT), size(0), count(0) {}

static std::string formatField(const std::string& s) {
  return (s.size() > 10 ? s.substr(0, 9) + "." : s);
}

static std::string centerField(const std::string& s, int width = 10) {
  if (s.length() >= static_cast<size_t>(width)) return s.substr(0, width);
  int pad_left = (width - s.length()) / 2;
  int pad_right = width - s.length() - pad_left;
  return std::string(pad_left, ' ') + s + std::string(pad_right, ' ');
}

static bool is_blank(const std::string& s) {
  for (std::size_t i = 0; i < s.size(); ++i)
    if (!std::isspace(static_cast<unsigned char>(s[i]))) return false;
  return true;
}

static bool isValidPhoneNumber(const std::string& phone) {
  std::string cleaned;
  for (size_t i = 0; i < phone.length(); ++i) {
    if (std::isdigit(phone[i]) || (i == 0 && phone[i] == '+')) {
      cleaned += phone[i];
    } else if (phone[i] != ' ' && phone[i] != '-' && phone[i] != '.') {
      return false;
    }
  }

  if (cleaned.empty() || cleaned == "+") return false;
  bool isFR = (cleaned.length() == 10 && cleaned[0] == '0') ||
              (cleaned.length() == 12 && cleaned.substr(0, 3) == "+33");
  bool isES =
      (cleaned.length() == 9 && (cleaned[0] == '6' || cleaned[0] == '7' ||
                                 cleaned[0] == '8' || cleaned[0] == '9')) ||
      (cleaned.length() == 12 && cleaned.substr(0, 3) == "+34");
  bool isE164 = true;
  size_t digit_count = 0;
  for (size_t i = 0; i < cleaned.length(); i++) {
    if (std::isdigit(cleaned[i])) digit_count++;
  }
  if (digit_count < 9 || digit_count > 15) isE164 = false;
  return isFR || isES || isE164;
}

void PhoneBook::seed() {
  int i;
  const char* db[][5] = {
      {"Alice", "Smith", "Al", "06 12 34 56 78", "French local"},
      {"Maximilian", "Von-Trapp", "Maxi", "+33-6-11-22-33-44", "French Intl"},
      {"O'Connor", "John-Paul", "JP", "612 345 678", "Spanish local"},
      {"..........", "----------", "________", "+34 699 888 777",
       "Spanish Intl"},
      {"Short", "Cut", "S", "+1 (800) 123-4567", "Universal standard"},
      {"1234567890", "0987654321", "Numbers", "01.23.45.67.89",
       "Dotted format"}};
  const int n = 6;

  for (i = 0; i < n && i < this->cap; ++i) {
    this->contact[i].setField(Contact::FIELD_NAME, db[i][0]);
    this->contact[i].setField(Contact::FIELD_LASTNAME, db[i][1]);
    this->contact[i].setField(Contact::FIELD_NICKNAME, db[i][2]);
    this->contact[i].setField(Contact::FIELD_PHONE, db[i][3]);
    this->contact[i].setField(Contact::FIELD_SECRET, db[i][4]);
  }
  this->count = i;
  this->size = i % this->cap;
  std::cout << C_GREEN << "Phonebook seeded with " << i
            << " telecom-verified contacts." << C_RESET << "\n";
}

void PhoneBook::search() const {
  if (!this->cap || this->count == 0) {
    std::cout << C_YELLOW << "No contacts to search." << C_RESET << std::endl;
    return;
  }
  std::cout << "\n" << C_DIM << BOX_TOP << C_RESET << std::endl;
  std::cout << C_DIM << V_BAR << C_RESET << C_BOLD << C_CYAN
            << centerField("index") << C_RESET << C_DIM << V_BAR << C_RESET
            << C_BOLD << C_CYAN << centerField("first name") << C_RESET << C_DIM
            << V_BAR << C_RESET << C_BOLD << C_CYAN << centerField("last name")
            << C_RESET << C_DIM << V_BAR << C_RESET << C_BOLD << C_CYAN
            << centerField("nickname") << C_RESET << C_DIM << V_BAR << C_RESET
            << std::endl;
  std::cout << C_DIM << BOX_MID << C_RESET << std::endl;

  for (int i = 0; i < this->count; ++i) {
    std::cout << C_DIM << V_BAR << C_RESET << C_YELLOW << std::right
              << std::setw(10) << i << C_RESET << C_DIM << V_BAR << C_RESET
              << std::left << std::setw(10)
              << formatField(this->contact[i].getField(Contact::FIELD_NAME))
              << C_DIM << V_BAR << C_RESET << std::left << std::setw(10)
              << formatField(this->contact[i].getField(Contact::FIELD_LASTNAME))
              << C_DIM << V_BAR << C_RESET << std::left << std::setw(10)
              << formatField(this->contact[i].getField(Contact::FIELD_NICKNAME))
              << C_DIM << V_BAR << C_RESET << std::endl;
  }
  std::cout << C_DIM << BOX_BOT << C_RESET << std::endl;

  std::string input;
  while (true) {
    std::cout << C_BOLD << "\nEnter index to display " << C_DIM
              << "(empty to cancel): " << C_RESET;
    if (!std::getline(std::cin, input)) {
      std::cout << "\nAborted." << std::endl;
      return;
    }

    size_t start = input.find_first_not_of(" \t\n\r\v\f");
    if (start == std::string::npos) return;
    size_t end = input.find_last_not_of(" \t\n\r\v\f");
    input = input.substr(start, end - start + 1);

    bool ok = !input.empty();
    for (std::size_t k = 0; k < input.size() && ok; ++k)
      if (!std::isdigit(static_cast<unsigned char>(input[k]))) ok = false;

    if (!ok) {
      std::cout << C_RED << "Invalid index. Must be a number." << C_RESET
                << std::endl;
      continue;
    }

    int idx = std::atoi(input.c_str());
    if (idx < 0 || idx >= this->count) {
      std::cout << C_RED << "Index out of range." << C_RESET << std::endl;
      continue;
    }

    std::cout << "\n"
              << C_BOLD << C_CYAN << "── Contact #" << idx << " ──" << C_RESET
              << std::endl;
    std::cout << C_DIM << "First Name : " << C_RESET
              << this->contact[idx].getField(Contact::FIELD_NAME) << std::endl;
    std::cout << C_DIM << "Last Name  : " << C_RESET
              << this->contact[idx].getField(Contact::FIELD_LASTNAME)
              << std::endl;
    std::cout << C_DIM << "Nickname   : " << C_RESET
              << this->contact[idx].getField(Contact::FIELD_NICKNAME)
              << std::endl;
    std::cout << C_DIM << "Phone      : " << C_RESET
              << this->contact[idx].getField(Contact::FIELD_PHONE) << std::endl;
    std::cout << C_DIM << "Secret     : " << C_RESET
              << this->contact[idx].getField(Contact::FIELD_SECRET)
              << std::endl;
    return;
  }
}

void PhoneBook::add() {
  std::string name, lastName, nickName, phoneNumber, darkestSecret;

  std::cout << C_CYAN << "── New Contact ──" << C_RESET << std::endl;

  std::cout << "Enter the name: ";
  if (!std::getline(std::cin, name) || is_blank(name)) return;
  std::cout << "Enter the last name: ";
  if (!std::getline(std::cin, lastName) || is_blank(lastName)) return;
  std::cout << "Enter the nick name: ";
  if (!std::getline(std::cin, nickName) || is_blank(nickName)) return;
  while (true) {
    std::cout << "Enter the phone number: ";
    if (!std::getline(std::cin, phoneNumber)) return;
    if (is_blank(phoneNumber)) return;

    if (isValidPhoneNumber(phoneNumber)) {
      break;
    } else {
      std::cout << C_RED
                << "Invalid format! Try local (e.g. 06... or 6...) or intl "
                   "(+33..., +34...)\n"
                << C_RESET;
    }
  }

  std::cout << "Enter the darkest secret: ";
  if (!std::getline(std::cin, darkestSecret) || is_blank(darkestSecret)) return;

  this->contact[this->size].setField(Contact::FIELD_NAME, name);
  this->contact[this->size].setField(Contact::FIELD_LASTNAME, lastName);
  this->contact[this->size].setField(Contact::FIELD_NICKNAME, nickName);
  this->contact[this->size].setField(Contact::FIELD_PHONE, phoneNumber);
  this->contact[this->size].setField(Contact::FIELD_SECRET, darkestSecret);

  this->size = (this->size + 1) % this->cap;
  if (this->count < this->cap) this->count++;

  std::cout << C_GREEN << "Contact saved successfully!" << C_RESET << std::endl;
}

int PhoneBook::getCount() const { return this->count; }

const Contact& PhoneBook::getContact(int idx) const {
  if (this->count == 0 || idx < 0) {
    static Contact dummy;
    return dummy;
  }
  if (idx >= this->count) return this->contact[this->count - 1];
  return this->contact[idx];
}

bool PhoneBook::pushContact(const Contact& c) {
  if (is_blank(c.getField(Contact::FIELD_NAME)) ||
      is_blank(c.getField(Contact::FIELD_LASTNAME)) ||
      is_blank(c.getField(Contact::FIELD_NICKNAME)) ||
      !isValidPhoneNumber(
          c.getField(Contact::FIELD_PHONE)) ||
      is_blank(c.getField(Contact::FIELD_SECRET)))
    return false;
  this->contact[this->size] = c;
  this->size = (this->size + 1) % this->cap;
  if (this->count < this->cap) this->count++;
  return true;
}
