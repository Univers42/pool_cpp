/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subject.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// The subject's requirements for ex02, each turned into an assertion —
// including both versions of its example main, run verbatim with std::cout
// captured, so "the output is the same" is compared as text rather than
// eyeballed across two terminals.
//
// The subject asks for:
//   1. a MutantStack that behaves like std::stack but is iterable;
//   2. its example main, whose output is fixed;
//   3. the same main with std::list in place of MutantStack, producing
//      identical output — the subject states this as the acceptance test.
//
// The evaluation sheet adds:
//   4. it inherits from std::stack and offers all of its member functions;
//   5. it has an iterator, and at least the subject's example operations are
//      possible using iterators.

#include <iostream>
#include <list>
#include <sstream>
#include <stack>
#include <streambuf>
#include <string>

#include "../MutantStack.hpp"
#include "check.hpp"

// The subject's main, character for character, with its output captured.
static std::string runMutantStackVersion() {
  std::ostringstream captured;
  std::streambuf* saved = std::cout.rdbuf(captured.rdbuf());

  {
    MutantStack<int> mstack;

    mstack.push(5);
    mstack.push(17);

    std::cout << mstack.top() << std::endl;

    mstack.pop();

    std::cout << mstack.size() << std::endl;

    mstack.push(3);
    mstack.push(5);
    mstack.push(737);
    //[...]
    mstack.push(0);

    MutantStack<int>::iterator it = mstack.begin();
    MutantStack<int>::iterator ite = mstack.end();

    ++it;
    --it;
    while (it != ite) {
      std::cout << *it << std::endl;
      ++it;
    }
    std::stack<int> s(mstack);
  }

  std::cout.rdbuf(saved);
  return captured.str();
}

// The same main with std::list substituted, exactly as the subject spells the
// substitution out: push becomes push_back, top becomes back, and the stack
// copy at the end becomes a list copy.
static std::string runListVersion() {
  std::ostringstream captured;
  std::streambuf* saved = std::cout.rdbuf(captured.rdbuf());

  {
    std::list<int> mstack;

    mstack.push_back(5);
    mstack.push_back(17);

    std::cout << mstack.back() << std::endl;

    mstack.pop_back();

    std::cout << mstack.size() << std::endl;

    mstack.push_back(3);
    mstack.push_back(5);
    mstack.push_back(737);
    //[...]
    mstack.push_back(0);

    std::list<int>::iterator it = mstack.begin();
    std::list<int>::iterator ite = mstack.end();

    ++it;
    --it;
    while (it != ite) {
      std::cout << *it << std::endl;
      ++it;
    }
    std::list<int> s(mstack);
  }

  std::cout.rdbuf(saved);
  return captured.str();
}

static void requirement_1_2_example_output() {
  Check::section("1+2. the example main's output is fixed");

  const std::string output = runMutantStackVersion();
  Check::eq(output, std::string("17\n1\n5\n3\n5\n737\n0\n"),
            "captured stdout, byte for byte");
}

static void requirement_3_same_as_a_list() {
  Check::section("3. the std::list version prints exactly the same");

  const std::string fromStack = runMutantStackVersion();
  const std::string fromList = runListVersion();
  Check::eq(fromStack, fromList,
            "the subject's acceptance test, compared as text");
  Check::ok(!fromStack.empty(), "and neither of them printed nothing");
}

static void requirement_4_inherits_std_stack() {
  Check::section("4. inherits std::stack and keeps all its members");

  MutantStack<int> ms;
  ms.push(1);
  ms.push(2);

  Check::eq(ms.top(), 2, "top()");
  Check::eq(ms.size(), static_cast<std::size_t>(2), "size()");
  Check::ok(!ms.empty(), "empty()");
  ms.pop();
  Check::eq(ms.size(), static_cast<std::size_t>(1), "pop()");

  // The proof of inheritance rather than imitation: a MutantStack binds to a
  // std::stack reference, and the base copy constructor accepts one.
  const std::stack<int>& asBase = ms;
  Check::eq(asBase.size(), static_cast<std::size_t>(1),
            "binds to std::stack& with no conversion");
  std::stack<int> copied(ms);
  Check::eq(copied.top(), 1, "std::stack<int> s(mstack); compiles and copies");
}

static void requirement_5_has_an_iterator() {
  Check::section("5. it has an iterator, and the example's ops work with it");

  MutantStack<int> ms;
  ms.push(5);
  ms.push(3);
  ms.push(737);

  // Exactly the operations the subject's main performs on the iterator.
  MutantStack<int>::iterator it = ms.begin();
  MutantStack<int>::iterator ite = ms.end();
  ++it;
  --it;
  Check::ok(it != ite, "begin() != end() on a non-empty stack");
  Check::eq(*it, 5, "++ then -- lands back on the first element");

  int seen = 0;
  while (it != ite) {
    ++seen;
    ++it;
  }
  Check::eq(seen, 3, "the while loop visits every element once");

  // And the other three flavours the class advertises.
  Check::eq(*ms.rbegin(), 737, "reverse_iterator");
  const MutantStack<int>& frozen = ms;
  Check::eq(*frozen.begin(), 5, "const_iterator");
  Check::eq(*frozen.rbegin(), 737, "const_reverse_iterator");
}

int main() {
  requirement_1_2_example_output();
  requirement_3_same_as_a_list();
  requirement_4_inherits_std_stack();
  requirement_5_has_an_iterator();

  return Check::report("ex02 MutantStack - the subject's own requirements");
}
