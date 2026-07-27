/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 17:01:13 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/time.h>

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <deque>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "PmergeMe.hpp"

static double now_us(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

// ponytail: display truncated at 10 elements like the subject's "[...]";
// print everything if an evaluator ever asks.
template <typename Container>
static void printSequence(const char* label, const Container& c) {
  std::cout << label;
  size_t shown = c.size() < 10 ? c.size() : 10;
  for (size_t i = 0; i < shown; ++i) {
    if (i) std::cout << ' ';
    std::cout << c[i];
  }
  if (c.size() > shown) std::cout << " [...]";
  std::cout << std::endl;
}

// Fills 'out' with the parsed positive ints; false on any invalid token.
static bool parseArgs(int argc, char** argv, std::vector<int>& out) {
  std::string joined;
  for (int i = 1; i < argc; ++i) {
    joined += argv[i];
    joined += ' ';
  }
  std::istringstream iss(joined);
  std::string token;
  while (iss >> token) {
    errno = 0;
    char* end = NULL;
    long val = std::strtol(token.c_str(), &end, 10);
    if (errno != 0 || end == token.c_str() || *end != '\0' || val <= 0 ||
        val > INT_MAX)
      return false;  // subject wants POSITIVE integers: 0 is not positive
    out.push_back(static_cast<int>(val));
  }
  return !out.empty();
}

int main(int argc, char** argv) {
  std::vector<int> input;
  if (!parseArgs(argc, argv, input)) {
    std::cerr << "Error" << std::endl;
    return 1;
  }
  printSequence("Before:  ", input);

  // Subject: timing must cover data management too, so the container fill
  // is inside the timed section for each container.
  PmergeMe sorter;
  double t0 = now_us();
  std::vector<int> vec(input.begin(), input.end());
  sorter.sortVector(vec);
  double vecTime = now_us() - t0;

  t0 = now_us();
  std::deque<int> deq(input.begin(), input.end());
  sorter.sortDeque(deq);
  double deqTime = now_us() - t0;

  printSequence("After:   ", vec);
  std::cout << std::fixed << std::setprecision(5);
  std::cout << "Time to process a range of " << input.size()
            << " elements with std::vector : " << vecTime << " us"
            << std::endl;
  std::cout << "Time to process a range of " << input.size()
            << " elements with std::deque  : " << deqTime << " us"
            << std::endl;
  return 0;
}
