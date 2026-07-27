/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 16:58:54 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:15:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE09_EX02_PMERGEME_HPP_
#define CPP_MODULE09_EX02_PMERGEME_HPP_

#include <deque>
#include <utility>
#include <vector>

class PmergeMe {
 private:
  // Core sorting logic duplicated strictly for std::vector
  void sortPairsVector(std::vector<std::pair<int, int> >& arr, int left,
                       int right);
  void mergePairsVector(std::vector<std::pair<int, int> >& arr, int left,
                        int mid, int right);
  std::vector<int> generateJacobsthalVector(int n);

  // Core sorting logic duplicated strictly for std::deque
  void sortPairsDeque(std::deque<std::pair<int, int> >& arr, int left,
                      int right);
  void mergePairsDeque(std::deque<std::pair<int, int> >& arr, int left, int mid,
                       int right);
  std::deque<int> generateJacobsthalDeque(int n);

 public:
  PmergeMe();
  PmergeMe(const PmergeMe& src);
  PmergeMe& operator=(const PmergeMe& rhs);
  ~PmergeMe();

  void sortVector(std::vector<int>& arr);
  void sortDeque(std::deque<int>& arr);
};

#endif  // CPP_MODULE09_EX02_PMERGEME_HPP_
