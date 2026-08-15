/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 13:05:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/15 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Fixed-case tests for MutantStack. The randomised half is tests/fuzz.cpp,
// which replays random push/pop sequences against a std::deque shadow; the
// subject's own main is tests/subject.cpp with its output asserted verbatim;
// and tests/cumulative.cpp drives ex00 and ex01 off this class's iterators.
//
//   make test               build and run everything
//   ./build/bin/test        just this file
//   ./tests/mutants.sh      breaks MutantStack.hpp, these tests must fail
//   ./tests/compile_fail.sh the code that must NOT compile
//   valgrind --leak-check=full ./build/bin/test
//
// The section worth reading is "element lifetime": Tracked counts how many of
// itself exist, so a copy that shares storage or a stack that forgets to
// destroy its contents shows up as a count that does not return to zero.
// Still run valgrind — a counter cannot see a double free.

#include <algorithm>
#include <deque>
#include <list>
#include <numeric>
#include <stack>
#include <string>
#include <vector>

#include "../MutantStack.hpp"
#include "check.hpp"

// Counts live instances, so a leaked or shared container is visible.
struct Tracked {
  static int alive;
  static int copies;
  int value;

  Tracked() : value(0) { ++alive; }
  explicit Tracked(int v) : value(v) { ++alive; }
  Tracked(const Tracked& o) : value(o.value) {
    ++alive;
    ++copies;
  }
  Tracked& operator=(const Tracked& o) {
    value = o.value;
    return *this;
  }
  ~Tracked() { --alive; }
};
int Tracked::alive = 0;
int Tracked::copies = 0;

// ─────────────────────────────────────────────────────────────────────────────
static void test_inherits_the_whole_stack() {
  Check::section("every std::stack member function still works");

  MutantStack<int> ms;
  Check::ok(ms.empty(), "a fresh MutantStack is empty");
  Check::eq(ms.size(), static_cast<std::size_t>(0), "and has size 0");

  ms.push(5);
  ms.push(17);
  Check::eq(ms.top(), 17, "top() is the last pushed value");
  Check::eq(ms.size(), static_cast<std::size_t>(2), "size() counts them");
  Check::ok(!ms.empty(), "empty() is false once something is in");

  ms.pop();
  Check::eq(ms.top(), 5, "pop() removes the top");
  Check::eq(ms.size(), static_cast<std::size_t>(1), "and shortens it");

  // top() returns a reference, so it is assignable — a std::stack guarantee
  // that inheritance must not quietly lose.
  ms.top() = 42;
  Check::eq(ms.top(), 42, "top() is a mutable reference");

  const MutantStack<int>& frozen = ms;
  Check::eq(frozen.top(), 42, "and there is a const top() too");
  Check::eq(frozen.size(), static_cast<std::size_t>(1), "const size()");
  Check::ok(!frozen.empty(), "const empty()");

  ms.pop();
  Check::ok(ms.empty(), "popping the last element empties it");
}

static void test_matches_a_list() {
  Check::section("the subject's criterion: identical to std::list");

  // The subject: swap MutantStack for std::list and the output must not
  // change. Both are driven through the same script and compared.
  MutantStack<int> ms;
  std::list<int> lst;

  ms.push(5);
  ms.push(17);
  lst.push_back(5);
  lst.push_back(17);
  Check::eq(ms.top(), lst.back(), "top() matches list::back()");

  ms.pop();
  lst.pop_back();
  Check::eq(ms.size(), lst.size(), "size() matches after a pop");

  const int more[] = {3, 5, 737, 0};
  for (int i = 0; i < 4; ++i) {
    ms.push(more[i]);
    lst.push_back(more[i]);
  }

  std::vector<int> fromStack(ms.begin(), ms.end());
  std::vector<int> fromList(lst.begin(), lst.end());
  Check::ok(fromStack == fromList,
            "every iterated value matches, in the same order");
  Check::eq(fromStack.size(), static_cast<std::size_t>(5), "five values");

  std::vector<int> reverseStack(ms.rbegin(), ms.rend());
  std::vector<int> reverseList(lst.rbegin(), lst.rend());
  Check::ok(reverseStack == reverseList, "and so does reverse iteration");
}

static void test_iteration_order() {
  Check::section("begin() is the bottom, rbegin() is the top");

  MutantStack<int> ms;
  for (int i = 1; i <= 5; ++i) ms.push(i * 10);

  Check::eq(*ms.begin(), 10, "begin() is the first value pushed");
  Check::eq(*ms.rbegin(), 50, "rbegin() is the last one, which is top()");
  Check::eq(*ms.rbegin(), ms.top(), "rbegin() and top() agree");

  Check::eq(static_cast<std::size_t>(std::distance(ms.begin(), ms.end())),
            ms.size(), "the range length is the size");

  const int expected[] = {10, 20, 30, 40, 50};
  Check::ok(std::equal(ms.begin(), ms.end(), expected), "forward order");

  const int reversed[] = {50, 40, 30, 20, 10};
  Check::ok(std::equal(ms.rbegin(), ms.rend(), reversed), "reverse order");

  // Popping moves end() back by one; the rest of the range is unchanged.
  ms.pop();
  Check::eq(static_cast<std::size_t>(std::distance(ms.begin(), ms.end())),
            static_cast<std::size_t>(4), "one shorter after a pop");
  Check::eq(*ms.rbegin(), 40, "and rbegin() followed the new top");
}

static void test_empty_iteration() {
  Check::section("an empty MutantStack has an empty range");

  MutantStack<int> ms;
  Check::same(ms.begin(), ms.end(), "begin() == end()");
  Check::same(ms.rbegin(), ms.rend(), "rbegin() == rend()");
  Check::eq(std::distance(ms.begin(), ms.end()),
            static_cast<std::ptrdiff_t>(0), "distance is zero");

  const MutantStack<int>& frozen = ms;
  Check::same(frozen.begin(), frozen.end(), "const begin() == const end()");

  // Algorithms on an empty range are well defined and do nothing.
  Check::eq(std::count(ms.begin(), ms.end(), 1),
            static_cast<std::ptrdiff_t>(0), "std::count on an empty range");
  Check::eq(std::accumulate(ms.begin(), ms.end(), 0), 0, "std::accumulate");
}

static void test_const_iteration() {
  Check::section("const iteration");

  MutantStack<int> ms;
  ms.push(1);
  ms.push(2);
  ms.push(3);
  const MutantStack<int>& frozen = ms;

  MutantStack<int>::const_iterator it = frozen.begin();
  Check::eq(*it, 1, "const begin()");
  MutantStack<int>::const_reverse_iterator rit = frozen.rbegin();
  Check::eq(*rit, 3, "const rbegin()");

  const int expected[] = {1, 2, 3};
  Check::ok(std::equal(frozen.begin(), frozen.end(), expected),
            "a const MutantStack iterates the same values");
  Check::eq(static_cast<std::size_t>(std::distance(frozen.begin(),
                                                   frozen.end())),
            static_cast<std::size_t>(3), "and the same length");

  // Both ends of the const reverse range, not just rbegin(). Checking only
  // the start left a mutant alive that broke const rend() alone.
  const int reversed[] = {3, 2, 1};
  Check::ok(std::equal(frozen.rbegin(), frozen.rend(), reversed),
            "const reverse iteration covers every element in order");
  Check::eq(static_cast<std::size_t>(std::distance(frozen.rbegin(),
                                                   frozen.rend())),
            static_cast<std::size_t>(3), "over the full length");
  Check::eq(std::count(frozen.rbegin(), frozen.rend(), 1),
            static_cast<std::ptrdiff_t>(1),
            "and an algorithm over the const reverse range agrees");

  // A mutable iterator converts to a const one, as the standard's do.
  MutantStack<int>::const_iterator fromMutable = ms.begin();
  Check::eq(*fromMutable, 1, "iterator converts to const_iterator");
}

static void test_writing_through_iterators() {
  Check::section("the mutable iterator reaches into the real container");

  MutantStack<int> ms;
  for (int i = 1; i <= 4; ++i) ms.push(i);

  MutantStack<int>::iterator middle = ms.begin();
  ++middle;
  ++middle;
  *middle = 99;

  const int expected[] = {1, 2, 99, 4};
  Check::ok(std::equal(ms.begin(), ms.end(), expected),
            "the write landed in the stack itself, not in a copy");
  Check::eq(ms.top(), 4, "the top is untouched");

  // And the change survives being read back through the stack interface.
  ms.pop();
  Check::eq(ms.top(), 99, "pop() exposes the edited element");

  // std::replace over a stack, which is the point of having iterators.
  MutantStack<int> ms2;
  for (int i = 0; i < 5; ++i) ms2.push(i % 2);
  std::replace(ms2.begin(), ms2.end(), 0, 7);
  const int replaced[] = {7, 1, 7, 1, 7};
  Check::ok(std::equal(ms2.begin(), ms2.end(), replaced),
            "std::replace rewrote it in place");
}

static void test_algorithms() {
  Check::section("<algorithm> and <numeric> work on it");

  MutantStack<int> ms;
  const int values[] = {5, 17, 3, 737, 0};
  for (int i = 0; i < 5; ++i) ms.push(values[i]);

  Check::eq(std::count(ms.begin(), ms.end(), 3),
            static_cast<std::ptrdiff_t>(1), "std::count");
  Check::eq(*std::max_element(ms.begin(), ms.end()), 737, "std::max_element");
  Check::eq(*std::min_element(ms.begin(), ms.end()), 0, "std::min_element");
  Check::eq(std::accumulate(ms.begin(), ms.end(), 0), 762, "std::accumulate");
  Check::same(std::find(ms.begin(), ms.end(), 737), ms.begin() + 3,
              "std::find lands on the right position");

  // Copying a stack's contents out is now a one-liner.
  std::vector<int> copied(ms.begin(), ms.end());
  Check::ok(std::equal(copied.begin(), copied.end(), values),
            "range construction of a vector from a stack");

  // And sorting one in place, which no std::stack could offer.
  std::sort(ms.begin(), ms.end());
  const int sorted[] = {0, 3, 5, 17, 737};
  Check::ok(std::equal(ms.begin(), ms.end(), sorted),
            "std::sort over the deque-backed stack");
  Check::eq(ms.top(), 737, "and top() sees the sorted result");
}

static void test_slicing_to_a_stack() {
  Check::section("a MutantStack IS a std::stack");

  MutantStack<int> ms;
  ms.push(1);
  ms.push(2);
  ms.push(3);

  // The subject's last line. It compiles because public inheritance means the
  // base copy constructor accepts a derived object.
  std::stack<int> sliced(ms);
  Check::eq(sliced.size(), static_cast<std::size_t>(3), "the values came over");
  Check::eq(sliced.top(), 3, "including the top");

  sliced.pop();
  Check::eq(sliced.top(), 2, "and the copy behaves like a stack");
  Check::eq(ms.size(), static_cast<std::size_t>(3),
            "the original was not consumed");

  // Passing one to a function that takes a base reference.
  const std::stack<int>& asBase = ms;
  Check::eq(asBase.size(), static_cast<std::size_t>(3),
            "binds to a std::stack reference with no conversion");
  Check::eq(asBase.top(), 3, "and answers through it");
}

static void test_canonical_form() {
  Check::section("orthodox canonical form");

  MutantStack<int> original;
  original.push(1);
  original.push(2);

  MutantStack<int> copy(original);
  Check::eq(copy.size(), static_cast<std::size_t>(2), "copy ctor copies");
  Check::eq(copy.top(), 2, "including the order");

  MutantStack<int> assigned;
  assigned.push(99);
  assigned = original;
  Check::eq(assigned.size(), static_cast<std::size_t>(2),
            "assignment replaces the target's contents");
  Check::eq(assigned.top(), 2, "with the source's");

  // Independence in every direction.
  copy.push(3);
  assigned.pop();
  Check::eq(original.size(), static_cast<std::size_t>(2),
            "the original is untouched by either");
  Check::eq(copy.size(), static_cast<std::size_t>(3), "copy grew alone");
  Check::eq(assigned.size(), static_cast<std::size_t>(1),
            "assigned shrank alone");

  original.push(4);
  Check::eq(copy.size(), static_cast<std::size_t>(3),
            "and the reverse direction too");

  // Self-assignment through an alias, so the compiler cannot fold it away.
  MutantStack<int>& alias = original;
  original = alias;
  Check::eq(original.size(), static_cast<std::size_t>(3),
            "self-assignment keeps the contents");
  Check::eq(original.top(), 4, "and the order");

  // operator= must return *this. Returning the source instead produces the
  // right values everywhere and only shows up in the address, so that is what
  // gets checked - a mutant survived this section until it did.
  MutantStack<int> target;
  MutantStack<int> source;
  source.push(7);
  Check::ok(&(target = source) == &target,
            "operator= returns the target, not the source");

  MutantStack<int> chained;
  chained = target = source;
  Check::eq(chained.size(), static_cast<std::size_t>(1),
            "which is what makes a = b = c work");
  Check::eq(chained.top(), 7, "with the right value at the end of the chain");

  // Iterators of a copy point into the copy, not into the source.
  MutantStack<int> a;
  a.push(1);
  MutantStack<int> b(a);
  *b.begin() = 2;
  Check::eq(*a.begin(), 1, "writing through the copy's iterator misses a");
  Check::eq(*b.begin(), 2, "and hits b");
}

static void test_other_containers() {
  Check::section("any underlying container");

  const int values[] = {1, 2, 3, 4};

  MutantStack<int, std::vector<int> > onVector;
  MutantStack<int, std::list<int> > onList;
  MutantStack<int, std::deque<int> > onDeque;
  for (int i = 0; i < 4; ++i) {
    onVector.push(values[i]);
    onList.push(values[i]);
    onDeque.push(values[i]);
  }

  Check::ok(std::equal(onVector.begin(), onVector.end(), values), "vector");
  Check::ok(std::equal(onList.begin(), onList.end(), values), "list");
  Check::ok(std::equal(onDeque.begin(), onDeque.end(), values), "deque");

  Check::eq(onVector.top(), 4, "vector-backed top()");
  Check::eq(onList.top(), 4, "list-backed top()");

  // A list-backed stack still reverse-iterates, it just is not random access.
  const int reversed[] = {4, 3, 2, 1};
  Check::ok(std::equal(onList.rbegin(), onList.rend(), reversed),
            "list-backed reverse iteration");

  // Random access is available where the container provides it.
  Check::eq(*(onVector.begin() + 2), 3, "vector iterators are random access");
  Check::eq(*(onDeque.begin() + 2), 3, "so are deque's");
}

static void test_container_constructor() {
  Check::section("constructing from an existing container");

  const int values[] = {1, 2, 3};
  std::deque<int> seed(values, values + 3);

  MutantStack<int> ms(seed);
  Check::eq(ms.size(), static_cast<std::size_t>(3), "the container came over");
  Check::eq(ms.top(), 3, "with its last element on top");
  Check::ok(std::equal(ms.begin(), ms.end(), values), "and in order");

  seed.push_back(4);
  Check::eq(ms.size(), static_cast<std::size_t>(3),
            "the stack took a copy, not a reference");

  std::vector<int> vseed(values, values + 3);
  MutantStack<int, std::vector<int> > onVector(vseed);
  Check::eq(onVector.size(), static_cast<std::size_t>(3),
            "same for a vector-backed one");
}

static void test_non_int_elements() {
  Check::section("T is a template parameter, not an int in disguise");

  MutantStack<std::string> words;
  words.push("iterators");
  words.push("are");
  words.push("the");
  words.push("interface");

  Check::eq(words.top(), std::string("interface"), "top() of a string stack");
  Check::eq(words.size(), static_cast<std::size_t>(4), "size()");
  Check::eq(*words.begin(), std::string("iterators"), "begin()");
  Check::eq(*words.rbegin(), std::string("interface"), "rbegin()");

  std::string joined;
  for (MutantStack<std::string>::iterator it = words.begin();
       it != words.end(); ++it) {
    if (it != words.begin()) joined += " ";
    joined += *it;
  }
  Check::eq(joined, std::string("iterators are the interface"),
            "iterating a stack of strings");
}

static void test_element_lifetime() {
  Check::section("element lifetime: nothing leaks, nothing is shared");

  Check::eq(Tracked::alive, 0, "starting from zero");

  {
    MutantStack<Tracked> ms;
    for (int i = 0; i < 5; ++i) ms.push(Tracked(i));
    Check::eq(Tracked::alive, 5, "five elements alive inside the stack");

    {
      MutantStack<Tracked> copy(ms);
      Check::eq(Tracked::alive, 10, "a copy really copied all five");
      Check::eq(copy.begin()->value, 0, "and copied their values");

      // Mutating the copy must not touch the original: proof they are not
      // sharing a container behind the scenes.
      copy.begin()->value = 99;
      Check::eq(ms.begin()->value, 0, "the original is unaffected");
    }
    Check::eq(Tracked::alive, 5, "the copy destroyed its five on the way out");

    ms.pop();
    Check::eq(Tracked::alive, 4, "pop() destroys the element it removes");
  }
  Check::eq(Tracked::alive, 0, "everything destroyed when the stack went out");
}

static void test_scale() {
  Check::section("scale: 100,000 pushes, 50,000 pops");

  MutantStack<int> ms;
  for (int i = 1; i <= 100000; ++i) ms.push(i);
  Check::eq(ms.size(), static_cast<std::size_t>(100000), "all pushed");
  Check::eq(ms.top(), 100000, "top is the last one");

  for (int i = 0; i < 50000; ++i) ms.pop();
  Check::eq(ms.size(), static_cast<std::size_t>(50000), "half popped");
  Check::eq(ms.top(), 50000, "top is the new last one");

  Check::eq(std::accumulate(ms.begin(), ms.end(), 0L), 50000L * 50001L / 2L,
            "the survivors sum to n(n+1)/2");
  Check::eq(*ms.begin(), 1, "the bottom never moved");
  Check::eq(static_cast<std::size_t>(std::distance(ms.begin(), ms.end())),
            ms.size(), "range length still matches size");
}

int main() {
  test_inherits_the_whole_stack();
  test_matches_a_list();
  test_iteration_order();
  test_empty_iteration();
  test_const_iteration();
  test_writing_through_iterators();
  test_algorithms();
  test_slicing_to_a_stack();
  test_canonical_form();
  test_other_containers();
  test_container_constructor();
  test_non_int_elements();
  test_element_lifetime();
  test_scale();

  return Check::report("ex02 MutantStack - fixed cases");
}
