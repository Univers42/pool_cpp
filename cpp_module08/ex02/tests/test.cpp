/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Plain C++98 assert tests. Core check: the subject scenario produces the
// exact same value sequence through MutantStack and through std::list.
#include <cassert>
#include <iostream>
#include <list>
#include <stack>
#include <vector>

#include "../MutantStack.hpp"

// Subject main's scenario through MutantStack; every printed value captured.
static std::vector<int> runMutant() {
  std::vector<int> out;
  MutantStack<int> mstack;
  mstack.push(5);
  mstack.push(17);
  out.push_back(mstack.top());
  mstack.pop();
  out.push_back(static_cast<int>(mstack.size()));
  mstack.push(3);
  mstack.push(5);
  mstack.push(737);
  mstack.push(0);
  MutantStack<int>::iterator it = mstack.begin();
  MutantStack<int>::iterator ite = mstack.end();
  ++it;
  --it;
  while (it != ite) {
    out.push_back(*it);
    ++it;
  }
  std::stack<int> s(mstack);  // must compile and copy the contents
  assert(s.size() == mstack.size());
  return out;
}

// Same scenario through std::list — the subject's reference behavior.
static std::vector<int> runList() {
  std::vector<int> out;
  std::list<int> l;
  l.push_back(5);
  l.push_back(17);
  out.push_back(l.back());
  l.pop_back();
  out.push_back(static_cast<int>(l.size()));
  l.push_back(3);
  l.push_back(5);
  l.push_back(737);
  l.push_back(0);
  std::list<int>::iterator it = l.begin();
  std::list<int>::iterator ite = l.end();
  ++it;
  --it;
  while (it != ite) {
    out.push_back(*it);
    ++it;
  }
  return out;
}

int main() {
  // 1. Subject requirement: identical output vs the std::list translation.
  assert(runMutant() == runList());

  // 2. Const and reverse iterators walk bottom -> top and top -> bottom.
  MutantStack<int> m;
  m.push(10);
  m.push(20);
  m.push(30);
  const MutantStack<int> cm(m);
  int expected[] = {10, 20, 30};
  int i = 0;
  for (MutantStack<int>::const_iterator it = cm.begin(); it != cm.end(); ++it)
    assert(*it == expected[i++]);
  assert(i == 3);
  for (MutantStack<int>::reverse_iterator rit = m.rbegin(); rit != m.rend();
       ++rit)
    assert(*rit == expected[--i]);
  assert(i == 0);

  // 3. Orthodox Canonical Form: assignment makes an independent copy.
  MutantStack<int> copy;
  copy = m;
  copy.pop();
  assert(m.size() == 3 && copy.size() == 2);

  // 4. Alternate underlying container (std::vector) still iterates.
  MutantStack<int, std::vector<int> > vs;
  vs.push(42);
  vs.push(84);
  MutantStack<int, std::vector<int> >::iterator vit = vs.begin();
  assert(*vit == 42);
  ++vit;
  assert(*vit == 84);
  ++vit;
  assert(vit == vs.end());

  // 5. Stress: 100k pushes, 50k pops, iterate the survivors and check the sum.
  MutantStack<long> big;
  long pushedSum = 0;
  for (int n = 0; n < 100000; ++n) {
    big.push(n % 100);
    if (n < 50000) pushedSum += n % 100;
  }
  for (int n = 0; n < 50000; ++n) big.pop();
  assert(big.size() == 50000);
  long sum = 0;
  for (MutantStack<long>::iterator bit = big.begin(); bit != big.end(); ++bit)
    sum += *bit;
  assert(sum == pushedSum);

  std::cout << "ex02: all MutantStack tests passed" << std::endl;
  return 0;
}
