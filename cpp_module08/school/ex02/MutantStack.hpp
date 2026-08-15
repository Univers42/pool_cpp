/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MutantStack.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:57:40 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE08_EX02_MUTANTSTACK_HPP_
#define CPP_MODULE08_EX02_MUTANTSTACK_HPP_

#include <deque>
#include <stack>

// std::stack is not a container, it is a container ADAPTER: it wraps a real
// sequence container in a member the standard names `c` and narrows the
// interface down to push/pop/top/size/empty. That narrowing is why a stack has
// no iterators — and the standard declares `c` protected precisely so a
// derived class can widen it again. This is the intended extension point, not
// a loophole.
//
// Public inheritance means a MutantStack IS a std::stack: every member
// function is inherited unchanged, which is what the evaluation sheet means by
// "offers all of its member functions", and it is why `std::stack<int> s(m);`
// compiles in the subject's own main.
//
// ponytail: std::stack has no virtual destructor, so deleting a MutantStack
// through a std::stack* is undefined behaviour. Nothing here or in the tests
// ever does, and the standard offers no adapter that would — the trigger to
// revisit is the day something stores these polymorphically, which would mean
// composition (holding a stack) rather than inheritance.
template <typename T, typename Container = std::deque<T> >
class MutantStack : public std::stack<T, Container> {
 public:
  // The four iterator typedefs are the entire contract <algorithm> asks for.
  // They are not new iterators — they are the underlying container's,
  // re-exported under the names generic code expects to find.
  typedef typename Container::iterator iterator;
  typedef typename Container::const_iterator const_iterator;
  typedef typename Container::reverse_iterator reverse_iterator;
  typedef typename Container::const_reverse_iterator const_reverse_iterator;

  MutantStack() : std::stack<T, Container>() {}

  // std::stack's own constructor takes a container to start from. Declaring
  // any constructor here hides every base-class one, so it has to be
  // re-exposed by hand or `MutantStack<int> m(existingDeque)` stops working.
  explicit MutantStack(const Container& container)
      : std::stack<T, Container>(container) {}

  MutantStack(const MutantStack& src) : std::stack<T, Container>(src) {}

  MutantStack& operator=(const MutantStack& rhs) {
    if (this != &rhs) std::stack<T, Container>::operator=(rhs);
    return (*this);
  }

  ~MutantStack() {}

  // `this->` is mandatory in C++98, and this is the one C++ scar of the
  // exercise. `c` lives in a base class that depends on the template
  // parameters, so at template-definition time the compiler does not know
  // whether that base even has a member called `c`. Unqualified lookup
  // therefore refuses to search it; `this->` defers the lookup to
  // instantiation time, when the base is known. Bare `c.begin()` does not
  // compile — tests/compile_fail.sh proves it.
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
