// Assert-style tests for easyfind. Exits non-zero on failure.
#include <cassert>
#include <iostream>
#include <list>
#include <stdexcept>
#include <vector>

#include "easyfind.hpp"

int main() {
  std::vector<int> v;
  for (int i = 0; i < 5; ++i) v.push_back(i * 10);  // 0 10 20 30 40

  // finds an existing value
  assert(*easyfind(v, 30) == 30);
  // first element reachable
  assert(easyfind(v, 0) == v.begin());
  // returns the FIRST occurrence when duplicated
  v.push_back(30);
  assert(easyfind(v, 30) - v.begin() == 3);

  // throws std::out_of_range (like vector::at) when absent
  bool threw = false;
  try {
    easyfind(v, 99);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  assert(threw);

  // empty container throws too
  std::vector<int> empty;
  threw = false;
  try {
    easyfind(empty, 1);
  } catch (const std::exception&) {
    threw = true;
  }
  assert(threw);

  // works on a non-vector container and on const (const_iterator overload)
  std::list<int> l;
  l.push_back(1);
  l.push_back(2);
  const std::list<int>& cl = l;
  assert(*easyfind(cl, 2) == 2);

  std::cout << "ex00: all tests passed" << std::endl;
  return 0;
}
