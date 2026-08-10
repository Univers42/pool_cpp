/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 15:15:45 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 15:07:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <iostream>
#include <string>

#include "PhoneBook.hpp"

#define PAT_SPACE " \t\n\r\v\f"

#define C_RESET "\033[0m"
#define C_BOLD "\033[1m"
#define C_DIM "\033[2m"
#define C_CYAN "\033[36m"
#define C_YELLOW "\033[33m"

std::string strtolower(const std::string& in) {
  std::string out;
  for (std::size_t i = 0; i < in.size(); ++i) {
    out.push_back(
        static_cast<char>(std::tolower(static_cast<unsigned char>(in[i]))));
  }
  return out;
}

std::string trim(const std::string& s) {
  size_t start = s.find_first_not_of(PAT_SPACE);
  if (start == std::string::npos) return "";
  size_t end = s.find_last_not_of(PAT_SPACE);
  return s.substr(start, end - start + 1);
}

int main() {
  std::string input;
  PhoneBook pb;

  while (true) {
    std::cout << "\n"
              << C_BOLD << C_CYAN << "PhoneBook" << C_RESET << " > "
              << std::flush;
    if (!std::getline(std::cin, input)) {
      std::cout << std::endl;
      break;
    }
    std::string cmd = strtolower(trim(input));
    if (cmd == "add")
      pb.add();
    else if (cmd == "search")
      pb.search();
    else if (cmd == "seed")
      pb.seed();
    else if (cmd == "exit")
      break;
  }
  return 0;
}
