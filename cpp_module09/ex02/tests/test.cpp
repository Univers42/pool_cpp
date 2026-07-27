// Ford-Johnson sort checks for PmergeMe: both containers must produce the
// same result as std::sort, with no element lost, up to 3000+ elements.
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <vector>

#include "PmergeMe.hpp"

static void check(const std::vector<int>& in) {
  std::vector<int> vec(in.begin(), in.end());
  std::deque<int> deq(in.begin(), in.end());
  PmergeMe sorter;
  sorter.sortVector(vec);
  sorter.sortDeque(deq);

  std::vector<int> expected(in.begin(), in.end());
  std::sort(expected.begin(), expected.end());

  assert(vec.size() == expected.size());
  assert(std::equal(vec.begin(), vec.end(), expected.begin()));
  assert(deq.size() == expected.size());
  assert(std::equal(deq.begin(), deq.end(), expected.begin()));
}

int main(void) {
  // Trivial sizes
  check(std::vector<int>());
  check(std::vector<int>(1, 42));

  // Subject example
  int subject[] = {3, 5, 9, 7, 4};
  check(std::vector<int>(subject, subject + 5));

  // Duplicates (subject leaves handling to us: we keep them)
  int dups[] = {2, 2, 1, 3, 1, 2, 3};
  check(std::vector<int>(dups, dups + 7));

  // Already sorted, reverse sorted, all equal
  std::vector<int> ramp;
  for (int i = 0; i < 100; ++i) ramp.push_back(i);
  check(ramp);
  std::reverse(ramp.begin(), ramp.end());
  check(ramp);
  check(std::vector<int>(50, 7));

  // 3000 distinct integers, shuffled (subject minimum), even size
  std::vector<int> big;
  for (int i = 0; i < 3000; ++i) big.push_back(i);
  std::srand(42);
  std::random_shuffle(big.begin(), big.end());
  check(big);

  // 3001: odd size exercises the straggler path at scale
  big.push_back(3000);
  check(big);

  std::cout << "test.cpp: all PmergeMe checks passed" << std::endl;
  return 0;
}
