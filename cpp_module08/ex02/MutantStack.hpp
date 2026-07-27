/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MutantStack.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:57:40 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 12:58:47 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE08_EX02_MUTANTSTACK_HPP_
#define CPP_MODULE08_EX02_MUTANTSTACK_HPP_

#include <deque>
#include <stack>

template <typename T, typename Container = std::deque<T> >
class MutantStack : public std::stack<T, Container> {
 public:
  MutantStack() : std::stack<T, Container>() {}
  MutantStack(const MutantStack& src) : std::stack<T, Container>(src) {}
  MutantStack& operator=(const MutantStack& rhs) {
    if (this != &rhs) std::stack<T, Container>::operator=(rhs);
    return (*this);
  }
  ~MutantStack() {}

  typedef typename Container::iterator iterator;
  typedef typename Container::const_iterator const_iterator;
  typedef typename Container::reverse_iterator reverse_iterator;
  typedef typename Container::const_reverse_iterator const_reverse_iterator;

  // Accessing the protected member 'c' of std::stack
  iterator begin() { return this->c.begin(); }
  iterator end() { return this->c.end(); }

  const_iterator begin() const { return this->c.begin(); }
  const_iterator end() const { return this->c.end(); }

  reverse_iterator rbegin() { return this->c.rbegin(); }
  reverse_iterator rend() { return this->c.rend(); }

  const_reverse_iterator rbegin() const { return this->c.rbegin(); }
  const_reverse_iterator rend() const { return this->c.rend(); }
};

#endif  // CPP_MODULE08_EX02_MUTANTSTACK_HPP_
