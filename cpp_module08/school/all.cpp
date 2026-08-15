// ─────────────────────────────────────────────────────────────────────────────
// all.cpp — the module 08 lab: every container, every exercise, one program.
//
// This is NOT turn-in code. The exercises in ex00/ ex01/ ex02/ are C++98 and
// stay that way; this file is the scratchpad that goes wider than the subject
// is allowed to, so it compiles as C++17 and uses std::array, forward_list and
// the unordered_ containers freely.
//
//   make -C .. -f Makefile run     from anywhere
//   cd cpp_module08 && make run    the usual way
//   make lab                       same thing
//
// It is one program in five parts:
//   1. container typedefs      value_type, size_type, difference_type,
//                              reference, const_reference, iterators
//   2. easyfind, three ways    the C++98 spelling the exercise must use, next
//                              to the C++14 `auto` and C++11 decltype ones,
//                              with an assertion that all three agree
//   3. Span                    ex01's class, working
//   4. MutantStack             ex02's class, working
//   5. container scenes        easyfind driven across ten container types
//
// Everything is asserted rather than eyeballed, so a wrong answer fails the
// run instead of scrolling past.
// ─────────────────────────────────────────────────────────────────────────────

#include <algorithm>
#include <array>
#include <cassert>
#include <deque>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// ── printing helpers ─────────────────────────────────────────────────────────
namespace ui {

const char* const kBold = "\033[1m";
const char* const kDim = "\033[2m";
const char* const kGreen = "\033[92m";
const char* const kCyan = "\033[96m";
const char* const kYellow = "\033[93m";
const char* const kOff = "\033[0m";

void part(const std::string& text) {
  std::cout << "\n"
            << kBold << kCyan << text << kOff << "\n"
            << kCyan << std::string(text.size(), '=') << kOff << std::endl;
}

void scene(const std::string& text) {
  std::cout << "\n  " << kBold << text << kOff << "\n"
            << kDim << "  " << std::string(text.size(), '-') << kOff
            << std::endl;
}

void note(const std::string& text) {
  std::cout << "     " << kYellow << "note" << kOff << "  " << kDim << text
            << kOff << std::endl;
}

template <typename T>
void show(const std::string& label, const T& value) {
  std::string pad = label;
  if (pad.size() < 38) pad += std::string(38 - pad.size(), ' ');
  std::cout << "     " << pad << kGreen << value << kOff << std::endl;
}

// Any container at all, printed the same way: the only thing assumed is that
// it hands out iterators. That is the whole lesson of the module, applied to
// output instead of search.
template <typename T>
std::string join(const T& container) {
  std::string out = "{";
  for (typename T::const_iterator it = container.begin();
       it != container.end(); ++it) {
    if (it != container.begin()) out += ", ";
    out += std::to_string(*it);
  }
  return out + "}";
}

}  // namespace ui

// ─────────────────────────────────────────────────────────────────────────────
// PART 2 — easyfind, three spellings of the same function
//
// The exercise must use the first one: C++98 has no `auto` and no `decltype`,
// and `typename` is mandatory because T::iterator is a dependent name. The
// other two are here to show what the newer standards bought, and to be
// checked against the original rather than merely admired.
// ─────────────────────────────────────────────────────────────────────────────

// C++98 — the one ex00/easyfind.hpp ships.
template <typename T>
typename T::iterator easyfind98(T& container, int value) {
  typename T::iterator it =
      std::find(container.begin(), container.end(), value);
  if (it == container.end())
    throw std::out_of_range("easyfind: value not found");
  return it;
}

template <typename T>
typename T::const_iterator easyfind98(const T& container, int value) {
  typename T::const_iterator it =
      std::find(container.begin(), container.end(), value);
  if (it == container.end())
    throw std::out_of_range("easyfind: value not found");
  return it;
}

// C++14 — return type deduced from the return statement. Shorter, and it also
// works for containers whose iterator is not spelled T::iterator at all.
template <typename T>
auto easyfindAuto(T& container, int value) {
  auto it = std::find(container.begin(), container.end(), value);
  if (it == container.end())
    throw std::out_of_range("easyfind: value not found");
  return it;
}

// C++11 — ask the compiler what std::find would return, without calling it.
// std::declval<T&>() produces a T& in an unevaluated context, so this works
// even for a T that cannot be default-constructed.
template <typename T>
decltype(std::find(std::declval<T&>().begin(), std::declval<T&>().end(), 0))easyfindDecltype(T& container, int value) {
  auto it = std::find(container.begin(), container.end(), value);
  if (it == container.end())
    throw std::out_of_range("easyfind: value not found");
  return it;
}

// ─────────────────────────────────────────────────────────────────────────────
// PART 3 — Span, the ex01 class
// ─────────────────────────────────────────────────────────────────────────────
class Span {
 public:
  class SpanFullException : public std::exception {
   public:
    const char* what() const throw() override {
      return "Error: Span is already full, cannot add more numbers.";
    }
  };

  class NotEnoughElementsException : public std::exception {
   public:
    const char* what() const throw() override {
      return "Error: Not enough elements to calculate a span (requires at "
             "least 2).";
    }
  };

  typedef std::vector<int>::const_iterator const_iterator;

  Span() : _maxSize(0) {}
  explicit Span(unsigned int n) : _maxSize(n) { _numbers.reserve(n); }

  void addNumber(int n) {
    if (full()) throw SpanFullException();
    _numbers.push_back(n);
  }

  // The bulk insert. Capacity is checked BEFORE inserting, so a range that
  // does not fit throws and leaves the Span exactly as it was.
  template <typename InputIterator>
  void addNumber(InputIterator first, InputIterator last) {
    if (std::distance(first, last) + _numbers.size() > _maxSize)
      throw SpanFullException();
    _numbers.insert(_numbers.end(), first, last);
  }

  // The shortest span is NOT the difference between the two lowest values.
  // {6, 3, 17, 9, 11} has 3 and 6 at the bottom, a gap of 3, while the real
  // answer is 2 — between 9 and 11. The smallest gap can sit anywhere, but
  // after sorting it is always between two NEIGHBOURS.
  unsigned int shortestSpan() const {
    if (_numbers.size() < 2) throw NotEnoughElementsException();
    std::vector<int> sorted(_numbers);
    std::sort(sorted.begin(), sorted.end());
    // inner_product is the general two-range fold, not a dot product: the
    // sorted range against itself offset by one is exactly "adjacent gaps".
    return std::inner_product(
        sorted.begin() + 1, sorted.end(), sorted.begin(),
        std::numeric_limits<unsigned int>::max(),
        [](unsigned int a, unsigned int b) { return a < b ? a : b; },
        [](int high, int low) { return gap(high, low); });
  }

  unsigned int longestSpan() const {
    if (_numbers.size() < 2) throw NotEnoughElementsException();
    auto bounds = std::minmax_element(_numbers.begin(), _numbers.end());
    return gap(*bounds.second, *bounds.first);
  }

  unsigned int size() const {
    return static_cast<unsigned int>(_numbers.size());
  }
  unsigned int maxSize() const { return _maxSize; }
  bool empty() const { return _numbers.empty(); }
  bool full() const { return _numbers.size() >= _maxSize; }
  const_iterator begin() const { return _numbers.begin(); }
  const_iterator end() const { return _numbers.end(); }

  // The distance between two ints, as an unsigned. Casting before subtracting
  // is the trick: INT_MIN to INT_MAX is 4294967295, which fits in no signed
  // 32-bit type. Unsigned arithmetic wraps mod 2^32 and lands on the exact
  // distance; signed overflow would be undefined behaviour.
  static unsigned int gap(int high, int low) {
    return static_cast<unsigned int>(high) - static_cast<unsigned int>(low);
  }

 private:
  unsigned int _maxSize;
  std::vector<int> _numbers;
};

std::ostream& operator<<(std::ostream& os, const Span& span) {
  return os << "Span(" << span.size() << "/" << span.maxSize() << ") "
            << ui::join(span);
}

// ─────────────────────────────────────────────────────────────────────────────
// PART 4 — MutantStack, the ex02 class
//
// std::stack is an adapter: it wraps a real container in a protected member
// named `c` and publishes only push/pop/top/size/empty. `c` is protected
// precisely so a derived class can widen the interface again.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T, typename Container = std::deque<T> >
class MutantStack : public std::stack<T, Container> {
 public:
  typedef typename Container::iterator iterator;
  typedef typename Container::const_iterator const_iterator;
  typedef typename Container::reverse_iterator reverse_iterator;
  typedef typename Container::const_reverse_iterator const_reverse_iterator;

  // `this->` is mandatory even in C++17: `c` lives in a base class that
  // depends on the template parameters, so unqualified lookup will not enter
  // it. Bare c.begin() does not compile.
  iterator begin() { return this->c.begin(); }
  iterator end() { return this->c.end(); }
  const_iterator begin() const { return this->c.begin(); }
  const_iterator end() const { return this->c.end(); }
  reverse_iterator rbegin() { return this->c.rbegin(); }
  reverse_iterator rend() { return this->c.rend(); }
  const_reverse_iterator rbegin() const { return this->c.rbegin(); }
  const_reverse_iterator rend() const { return this->c.rend(); }
};

// ─────────────────────────────────────────────────────────────────────────────
// PART 1 — the container typedefs
// ─────────────────────────────────────────────────────────────────────────────
namespace typedefs {

void run() {
  ui::part("1. Container typedefs: the vocabulary every container speaks");

  std::vector<int> v = {10, 20, 30};

  ui::scene("value_type — what does this container hold?");
  static_assert(std::is_same<std::vector<int>::value_type, int>::value, "");
  static_assert(std::is_same<std::list<int>::value_type, int>::value, "");
  static_assert(std::is_same<std::set<int>::value_type, int>::value, "");
  // A map's value_type is the PAIR, not the mapped type. This is the single
  // reason easyfind refuses a std::map: *it == int does not compile.
  static_assert(
      std::is_same<std::map<int, char>::value_type,
                   std::pair<const int, char> >::value,
      "");
  ui::show("vector<int>::value_type", "int");
  ui::show("map<int,char>::value_type", "std::pair<const int, char>");
  ui::note("That pair is why easyfind takes a set but refuses a map.");

  ui::scene("size_type — the type for sizes and indexes");
  std::vector<int>::size_type size = v.size();
  ui::show("v.size()", size);
  ui::note("Not int: size_type is unsigned and wide enough for the container.");
  ui::note("Comparing it against a signed int is where -Wsign-compare bites.");
  for (std::vector<int>::size_type i = 0; i < v.size(); ++i)
    ui::show("  v[" + std::to_string(i) + "]", v[i]);

  ui::scene("difference_type — a SIGNED distance between two iterators");
  std::vector<int>::iterator first = v.begin();
  std::vector<int>::iterator last = v.end();
  std::vector<int>::difference_type distance = last - first;
  ui::show("last - first", distance);
  ui::show("first - last", first - last);
  ui::note("Signed, because subtracting iterators the other way round has to");
  ui::note("be able to answer -3. size_type could not.");

  ui::scene("reference and const_reference");
  std::vector<int>::reference alias = v[0];
  alias = 999;
  ui::show("after writing through v[0]'s reference", ui::join(v));
  ui::note("reference is int&, so the write lands in the vector itself.");

  const std::vector<int> frozen = {10, 20, 30};
  std::vector<int>::const_reference readOnly = frozen[0];
  ui::show("const_reference to frozen[0]", readOnly);
  // const_reference is `const int&`, so it also binds to a temporary and
  // extends its lifetime. Legal, and worth knowing before it surprises you.
  std::vector<int>::const_reference bound = 50;
  ui::show("const_reference bound to the literal 50", bound);
  ui::note("A const reference binds to a temporary and extends its life.");
  ui::note("A non-const one cannot: v[0] = 50 needs a real object.");

  ui::scene("iterator and const_iterator");
  std::vector<int>::const_iterator it = frozen.begin() + 1;
  ui::show("*(frozen.begin() + 1)", *it);
  // A mutable iterator converts to a const one; the reverse never happens.
  std::vector<int>::const_iterator fromMutable = v.begin();
  ui::show("iterator converts to const_iterator", *fromMutable);
  static_assert(
      std::is_convertible<std::vector<int>::iterator,
                          std::vector<int>::const_iterator>::value,
      "");
  static_assert(
      !std::is_convertible<std::vector<int>::const_iterator,
                           std::vector<int>::iterator>::value,
      "");
  ui::note("Checked at compile time above: the conversion is one-way.");

  ui::scene("iterator categories — what the container will let you do");
  ui::show("vector       ", "random access: it + 2, it - it, std::sort");
  ui::show("deque        ", "random access");
  ui::show("list / set   ", "bidirectional: ++ and --, no + 2");
  ui::show("forward_list ", "forward only: ++ and nothing else");
  static_assert(
      std::is_same<
          std::iterator_traits<std::vector<int>::iterator>::iterator_category,
          std::random_access_iterator_tag>::value,
      "");
  static_assert(
      std::is_same<
          std::iterator_traits<std::list<int>::iterator>::iterator_category,
          std::bidirectional_iterator_tag>::value,
      "");
  static_assert(
      std::is_same<std::iterator_traits<
                       std::forward_list<int>::iterator>::iterator_category,
                   std::forward_iterator_tag>::value,
      "");
  ui::note("std::sort needs random access, which is why it works on a vector");
  ui::note("and not on a list — std::list has its own sort() member instead.");
}

}  // namespace typedefs

// ─────────────────────────────────────────────────────────────────────────────
// PART 2 driver — the three spellings, checked against each other
// ─────────────────────────────────────────────────────────────────────────────
namespace three_ways {

void run() {
  ui::part("2. easyfind: three spellings of one function");

  std::vector<int> v = {10, 20, 30, 40};

  ui::scene("all three land on the same element");
  auto a = easyfind98(v, 30);
  auto b = easyfindAuto(v, 30);
  auto c = easyfindDecltype(v, 30);
  assert(a == b && b == c);
  ui::show("easyfind98(v, 30)", *a);
  ui::show("easyfindAuto(v, 30)", *b);
  ui::show("easyfindDecltype(v, 30)", *c);
  ui::show("same iterator", (a == b && b == c) ? "yes" : "no");

  // And the return types are literally the same type, not merely comparable.
  static_assert(std::is_same<decltype(a), decltype(b)>::value, "");
  static_assert(std::is_same<decltype(b), decltype(c)>::value, "");
  ui::note("static_assert above: the three return types are identical.");

  ui::scene("all three throw on a miss");
  int thrown = 0;
  try {
    easyfind98(v, 99);
  } catch (const std::out_of_range&) {
    ++thrown;
  }
  try {
    easyfindAuto(v, 99);
  } catch (const std::out_of_range&) {
    ++thrown;
  }
  try {
    easyfindDecltype(v, 99);
  } catch (const std::out_of_range&) {
    ++thrown;
  }
  assert(thrown == 3);
  ui::show("exceptions thrown", thrown);

  ui::scene("so why does the exercise use the longest one?");
  ui::note("C++98 has neither auto nor decltype, and the evaluation sheet");
  ui::note("treats a newer feature as a forbidden one. typename is not");
  ui::note("optional either: T::iterator is a dependent name, so without it");
  ui::note("the compiler assumes a static member rather than a type.");
  ui::note("The upside of the C++98 spelling is that the signature states the");
  ui::note("return type instead of making the reader deduce it.");
}

}  // namespace three_ways

// ─────────────────────────────────────────────────────────────────────────────
// PART 3 driver
// ─────────────────────────────────────────────────────────────────────────────
namespace span_lab {

void run() {
  ui::part("3. Span — a bounded set of ints answering two questions");

  ui::scene("the subject's example");
  Span sp(5);
  sp.addNumber(6);
  sp.addNumber(3);
  sp.addNumber(17);
  sp.addNumber(9);
  sp.addNumber(11);
  ui::show("sp", sp);
  ui::show("shortestSpan()", sp.shortestSpan());
  ui::show("longestSpan()", sp.longestSpan());
  assert(sp.shortestSpan() == 2);
  assert(sp.longestSpan() == 14);

  ui::scene("shortest is NOT the two lowest, subtracted");
  std::vector<int> sorted = {3, 6, 9, 11, 17};
  ui::show("sorted", ui::join(sorted));
  ui::show("naive: sorted[1] - sorted[0]", sorted[1] - sorted[0]);
  ui::show("real answer (11 - 9)", sp.shortestSpan());
  ui::note("The smallest gap can sit anywhere; sorting only guarantees the");
  ui::note("two values forming it end up next to each other.");

  ui::scene("the capacity is a hard ceiling");
  bool threw = false;
  try {
    sp.addNumber(42);
  } catch (const Span::SpanFullException& e) {
    threw = true;
    ui::show("addNumber past N", e.what());
  }
  assert(threw);
  assert(sp.size() == 5);

  ui::scene("a range that does not fit inserts nothing");
  Span tight(4);
  tight.addNumber(1);
  std::vector<int> tooMany = {2, 3, 4, 5, 6};
  threw = false;
  try {
    tight.addNumber(tooMany.begin(), tooMany.end());
  } catch (const Span::SpanFullException&) {
    threw = true;
  }
  assert(threw);
  assert(tight.size() == 1);
  ui::show("tight after the rejected range", tight);
  ui::note("std::distance is called before the insert, so the whole thing is");
  ui::note("rejected rather than half-applied.");

  ui::scene("fewer than two values is a refusal, not an answer");
  Span thin(5);
  threw = false;
  try {
    thin.shortestSpan();
  } catch (const Span::NotEnoughElementsException& e) {
    threw = true;
    ui::show("shortestSpan() on an empty Span", e.what());
  }
  assert(threw);

  ui::scene("the overflow trick");
  Span extremes(2);
  extremes.addNumber(std::numeric_limits<int>::min());
  extremes.addNumber(std::numeric_limits<int>::max());
  ui::show("INT_MIN .. INT_MAX", extremes.longestSpan());
  ui::show("UINT_MAX", std::numeric_limits<unsigned int>::max());
  assert(extremes.longestSpan() == std::numeric_limits<unsigned int>::max());
  ui::note("4294967295 fits in no signed 32-bit type. Casting to unsigned");
  ui::note("first makes the subtraction wrap mod 2^32 onto the exact answer.");

  ui::scene("scale: 10,000 values, one call");
  std::vector<int> many;
  many.reserve(10000);
  for (int i = 0; i < 10000; ++i) many.push_back(i * 5);
  Span big(10000);
  big.addNumber(many.begin(), many.end());
  ui::show("size", big.size());
  ui::show("shortestSpan()", big.shortestSpan());
  ui::show("longestSpan()", big.longestSpan());
  assert(big.shortestSpan() == 5);
  assert(big.longestSpan() == 49995);

  ui::scene("ex00 reaches into ex01");
  auto found = easyfind98(static_cast<const Span&>(sp), 17);
  ui::show("easyfind98(sp, 17)", *found);
  ui::note("Span exposes begin(), end() and const_iterator, which is the");
  ui::note("entire interface easyfind ever asked for.");
}

}  // namespace span_lab

// ─────────────────────────────────────────────────────────────────────────────
// PART 4 driver
// ─────────────────────────────────────────────────────────────────────────────
namespace stack_lab {

void run() {
  ui::part("4. MutantStack — giving an adapter its iterators back");

  ui::scene("it is still a stack");
  MutantStack<int> ms;
  ms.push(5);
  ms.push(17);
  ui::show("top()", ms.top());
  ms.pop();
  ui::show("size() after pop()", ms.size());
  ms.push(3);
  ms.push(5);
  ms.push(737);
  ms.push(0);
  assert(ms.size() == 5);

  ui::scene("and now it iterates");
  ui::show("bottom to top", ui::join(ms));
  std::vector<int> forward(ms.begin(), ms.end());
  assert((forward == std::vector<int>{5, 3, 5, 737, 0}));
  std::vector<int> backward(ms.rbegin(), ms.rend());
  assert((backward == std::vector<int>{0, 737, 5, 3, 5}));
  ui::show("top to bottom (rbegin/rend)",
           ui::join(std::list<int>(backward.begin(), backward.end())));
  ui::note("Reverse is the useful order for a stack: what pop() would hand");
  ui::note("back, without consuming anything.");

  ui::scene("the subject's acceptance test, as an assertion");
  std::list<int> lst = {5, 3, 5, 737, 0};
  assert(std::equal(ms.begin(), ms.end(), lst.begin()));
  ui::show("identical to the same script through std::list", "yes");

  ui::scene("which puts <algorithm> back within reach");
  ui::show("std::count(ms, 5)", std::count(ms.begin(), ms.end(), 5));
  ui::show("*std::max_element(ms)", *std::max_element(ms.begin(), ms.end()));
  ui::show("std::accumulate(ms, 0)",
           std::accumulate(ms.begin(), ms.end(), 0));
  auto hit = easyfind98(ms, 737);
  ui::show("easyfind98(ms, 737)", *hit);
  ui::note("None of that compiles against a plain std::stack: no begin(), no");
  ui::note("end(), no iterator typedef.");

  ui::scene("ex02 feeds ex01");
  Span fromStack(64);
  fromStack.addNumber(ms.begin(), ms.end());
  ui::show("Span filled from the stack", fromStack);
  ui::show("shortestSpan()", fromStack.shortestSpan());
  assert(fromStack.size() == 5);
  ui::note("One call. A std::stack could not have handed over a range.");

  ui::scene("a MutantStack IS a std::stack");
  std::stack<int> sliced(ms);
  ui::show("std::stack<int> s(ms), size", sliced.size());
  assert(sliced.size() == ms.size());
  ui::note("Public inheritance, so the base copy constructor accepts it. The");
  ui::note("copy keeps every value and loses every iterator.");
}

}  // namespace stack_lab

// ─────────────────────────────────────────────────────────────────────────────
// PART 5 — easyfind across every container the standard library offers
// ─────────────────────────────────────────────────────────────────────────────
namespace cont_lab {

// One scene, written once, for any container that can be built from a range.
template <typename Container>
void probe(const std::string& name, Container& container, int present,
           int absent) {
  ui::scene(name);
  ui::show("contents", ui::join(container));

  auto it = easyfind98(container, present);
  ui::show("easyfind(container, " + std::to_string(present) + ")", *it);
  assert(*it == present);

  bool threw = false;
  try {
    easyfind98(container, absent);
  } catch (const std::out_of_range& e) {
    threw = true;
    ui::show("searching for " + std::to_string(absent), e.what());
  }
  assert(threw);

  // The const overload has to agree with the mutable one.
  const Container& frozen = container;
  auto constIt = easyfind98(frozen, present);
  assert(*constIt == present);
}

void run() {
  ui::part("5. One template, ten containers");

  std::vector<int> asVector = {10, 20, 30, 40};
  probe("std::vector — contiguous, random access", asVector, 30, 99);

  std::array<int, 5> asArray = {10, 20, 30, 40, 50};
  probe("std::array — contiguous, fixed size", asArray, 40, 99);

  std::deque<int> asDeque = {5, 10, 20, 30};
  probe("std::deque — chunked, still random access", asDeque, 5, 99);

  std::list<int> asList = {10, 20, 30};
  probe("std::list — doubly linked, bidirectional", asList, 20, 99);

  std::forward_list<int> asForward = {10, 20, 30};
  ui::scene("std::forward_list — singly linked, forward only");
  ui::show("contents", ui::join(asForward));
  ui::show("easyfind(container, 30)", *easyfind98(asForward, 30));
  ui::note("No size(), no rbegin(): a forward iterator only goes forward.");
  ui::note("easyfind never needed either, which is why it works here too.");

  std::set<int> asSet = {30, 10, 20};
  probe("std::set — sorted, unique, bidirectional", asSet, 20, 99);
  ui::note("std::find walks the tree in order: O(n). set::find descends it:");
  ui::note("O(log n). Same answer, different price.");
  assert(easyfind98(asSet, 20) == asSet.find(20));

  std::multiset<int> asMultiset = {10, 20, 20, 30};
  probe("std::multiset — sorted, duplicates kept", asMultiset, 20, 99);
  assert(easyfind98(asMultiset, 20) == asMultiset.lower_bound(20));
  ui::note("The first of the equal range, as std::find promises.");

  std::unordered_set<int> asUnordered = {10, 20, 30};
  ui::scene("std::unordered_set — hashed, no order at all");
  ui::show("easyfind(container, 30)", *easyfind98(asUnordered, 30));
  ui::note("Printing it is pointless: the iteration order is unspecified and");
  ui::note("may differ between runs. The lookup still works.");

  std::unordered_multiset<int> asUnorderedMulti = {10, 20, 20, 30};
  ui::show("unordered_multiset, found", *easyfind98(asUnorderedMulti, 20));

  ui::scene("std::map — the one that cannot work");
  ui::note("map's value_type is std::pair<const Key, T>, so *it == int does");
  ui::note("not compile. The subject excuses associative containers; a set");
  ui::note("works anyway because its value_type really is int.");
  std::map<int, std::string> asMap = {{1, "one"}, {2, "two"}};
  auto byKey = asMap.find(2);
  ui::show("map::find(2)->second", byKey->second);

  ui::scene("std::stack and std::queue — the adapters");
  std::stack<int> plainStack;
  plainStack.push(1);
  std::queue<int> plainQueue;
  plainQueue.push(1);
  ui::show("stack.top() / queue.front()",
           std::to_string(plainStack.top()) + " / " +
               std::to_string(plainQueue.front()));
  ui::note("Neither has begin(). That is not an oversight, it is the point of");
  ui::note("an adapter — and part 4 is how you get it back.");
}

}  // namespace cont_lab

// ─────────────────────────────────────────────────────────────────────────────
int main() {
  std::cout << ui::kBold << ui::kCyan
            << "\nmodule 08 lab — containers, iterators, algorithms"
            << ui::kOff << "\n"
            << ui::kDim
            << "  not turn-in code: C++17, so it can go wider than the subject"
            << ui::kOff << std::endl;

  typedefs::run();
  three_ways::run();
  span_lab::run();
  stack_lab::run();
  cont_lab::run();

  std::cout << "\n"
            << ui::kGreen << ui::kBold
            << "  every assertion in the lab passed" << ui::kOff << "\n"
            << std::endl;
  return 0;
}
