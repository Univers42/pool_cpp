/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:14:46 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 12:20:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <deque>
#include <iostream>
#include <list>
#include <vector>

#include "easyfind.hpp"
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"

int main() {
  std::cout << CYAN << "========================================" << RESET
            << std::endl;
  std::cout << "--- 1. Testing std::vector (Array) ---" << std::endl;
  std::cout << CYAN << "========================================" << RESET
            << std::endl;

  std::vector<int> vec;
  vec.push_back(10);
  vec.push_back(20);
  vec.push_back(30);
  vec.push_back(40);

  try {
    std::cout << "Searching for 30... ";
    std::vector<int>::iterator it = easyfind(vec, 30);
    std::cout << GREEN << "Found! Value: " << *it << RESET << std::endl;

    std::cout << "Searching for 99... ";
    it = easyfind(vec, 99);
    std::cout << GREEN << "Found! Value: " << *it << RESET << std::endl;
  } catch (const std::exception& e) {
    std::cout << RED << e.what() << RESET << std::endl;
  }

  std::cout << "\n"
            << CYAN << "========================================" << RESET
            << std::endl;
  std::cout << "--- 2. Testing std::list (Linked List) ---" << std::endl;
  std::cout << CYAN << "========================================" << RESET
            << std::endl;

  std::list<int> lst;
  lst.push_back(42);
  lst.push_back(21);
  lst.push_back(84);

  try {
    std::cout << "Searching for 42... ";
    std::list<int>::iterator it = easyfind(lst, 42);
    std::cout << GREEN << "Found! Value: " << *it << RESET << std::endl;

    std::cout << "Searching for 0... ";
    it = easyfind(lst, 0);
    std::cout << GREEN << "Found! Value: " << *it << RESET << std::endl;
  } catch (const std::exception& e) {
    std::cout << RED << e.what() << RESET << std::endl;
  }

  std::cout << "\n"
            << CYAN << "========================================" << RESET
            << std::endl;
  std::cout << "--- 3. Testing Const Container (std::deque) ---" << std::endl;
  std::cout << CYAN << "========================================" << RESET
            << std::endl;

  std::deque<int> deq;
  deq.push_back(7);
  deq.push_back(8);
  deq.push_back(9);

  const std::deque<int> constDeq = deq;

  try {
    std::cout << "Searching for 8 in const deque... ";
    std::deque<int>::const_iterator it = easyfind(constDeq, 8);
    std::cout << GREEN << "Found! Value: " << *it << RESET << std::endl;
  } catch (const std::exception& e) {
    std::cout << RED << e.what() << RESET << std::endl;
  }

  return 0;
}
