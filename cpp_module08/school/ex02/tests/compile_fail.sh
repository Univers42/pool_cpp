#!/bin/sh
# The most important claim this exercise makes is a negative one: a plain
# std::stack cannot be iterated. No test binary can assert that, because the
# code that would prove it does not compile — so it lives here, next to the
# C++98 two-phase-lookup rule that forces the `this->` in MutantStack.hpp and
# the iterator-category limits of the different backing containers.
# Everything must fail to build except the control cases.
#
#   ./tests/compile_fail.sh   (from ex02/ or from tests/)

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
WARN="-Wall -Wextra -Werror"
OUT="build/nocompile"

# Colours only when a human is watching: piping this into a file or a chat
# should not paste a wall of escape codes.
if [ -t 1 ] && [ -z "$NO_COLOR" ]; then
  BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"
  GREY="\033[90m"; YELLOW="\033[33m"; CYAN="\033[36m"
else
  BOLD=""; OFF=""; GREEN=""; RED=""; GREY=""; YELLOW=""; CYAN=""
fi

mkdir -p "$OUT"
total=0; wrong=0

# expect_fail <name> <what rule this proves>   - snippet arrives on stdin.
# Compiled WITHOUT the warning flags, so a rejection is a real type error and
# never a warning that -Werror happened to promote.
expect_fail() {
  total=$((total + 1))
  cat > "$OUT/case.cpp"
  if $CXX $STD -I. -c "$OUT/case.cpp" -o "$OUT/case.o" 2>"$OUT/case.log"; then
    printf "  ${RED}FAIL${OFF} %-9s %-26s %s\n" "COMPILED" "$1" "$2"
    wrong=$((wrong + 1))
  else
    printf "  ${GREEN}ok${OFF}   %-9s %-26s ${GREY}%s${OFF}\n" "rejected" "$1" "$2"
  fi
}

# expect_ok <name> <what it shows> - the controls
expect_ok() {
  total=$((total + 1))
  cat > "$OUT/case.cpp"
  if $CXX $STD $WARN -I. -c "$OUT/case.cpp" -o "$OUT/case.o" \
      2>"$OUT/case.log"; then
    printf "  ${GREEN}ok${OFF}   %-9s %-26s ${GREY}%s${OFF}\n" "accepted" "$1" "$2"
  else
    printf "  ${RED}FAIL${OFF} %-9s %-26s %s\n" "REJECTED" "$1" "$2"
    wrong=$((wrong + 1))
    sed 's/^/      /' "$OUT/case.log"
  fi
}

printf "${BOLD}ex02 compile-failure tests${OFF}  17 cases, each with a required outcome\n"
printf "${GREY}  every line should say ok. 'rejected' is the compiler refusing bad\n  code on purpose, which is the result being tested for.${OFF}\n\n"

expect_ok "control" "ordinary correct usage, warning-free" <<'EOF'
#include <algorithm>
#include <list>
#include <stack>
#include <vector>
#include "MutantStack.hpp"
void f() {
  MutantStack<int> ms;
  ms.push(1);
  ms.top() = 2;
  ms.pop();
  ms.push(3);
  (void)ms.size();
  (void)ms.empty();
  for (MutantStack<int>::iterator it = ms.begin(); it != ms.end(); ++it)
    *it = 0;
  for (MutantStack<int>::reverse_iterator it = ms.rbegin(); it != ms.rend();
       ++it)
    (void)*it;
  const MutantStack<int>& c = ms;
  for (MutantStack<int>::const_iterator it = c.begin(); it != c.end(); ++it)
    (void)*it;
  for (MutantStack<int>::const_reverse_iterator it = c.rbegin();
       it != c.rend(); ++it)
    (void)*it;
  MutantStack<int> copy(ms);
  MutantStack<int> assigned;
  assigned = ms;
  std::stack<int> sliced(ms);
  (void)sliced;
  (void)copy;
  MutantStack<int, std::vector<int> > onVector;
  MutantStack<int, std::list<int> > onList;
  (void)onVector;
  (void)onList;
}
EOF

expect_ok "easyfind-accepts-it" "ex00's template, unchanged, takes a stack" <<'EOF'
#include <algorithm>
#include <stdexcept>
#include "MutantStack.hpp"
// Copied verbatim from ex00/easyfind.hpp.
template <typename T>
typename T::iterator easyfind(T& container, int value) {
  typename T::iterator it =
      std::find(container.begin(), container.end(), value);
  if (it == container.end())
    throw std::out_of_range("easyfind: value not found");
  return (it);
}
void f() {
  MutantStack<int> ms;
  ms.push(7);
  (void)easyfind(ms, 7);
}
EOF

expect_fail "easyfind-rejects-std-stack" "the same call on a plain std::stack" <<'EOF'
#include <algorithm>
#include <stack>
#include <stdexcept>
// The identical template. std::stack has no begin(), no end() and no
// iterator typedef, so this is the exact line ex02 exists to make work.
template <typename T>
typename T::iterator easyfind(T& container, int value) {
  typename T::iterator it =
      std::find(container.begin(), container.end(), value);
  if (it == container.end())
    throw std::out_of_range("easyfind: value not found");
  return (it);
}
void f() {
  std::stack<int> s;
  s.push(7);
  (void)easyfind(s, 7);
}
EOF

expect_fail "std-stack-has-no-begin" "std::stack::begin() does not exist" <<'EOF'
#include <stack>
void f() {
  std::stack<int> s;
  s.push(1);
  (void)s.begin();
}
EOF

expect_fail "std-stack-has-no-iterator" "nor the typedef generic code needs" <<'EOF'
#include <stack>
void f() {
  std::stack<int>::iterator it;
  (void)it;
}
EOF

expect_fail "container-is-protected" "c is protected: a caller cannot reach it" <<'EOF'
#include "MutantStack.hpp"
void f() {
  MutantStack<int> ms;
  ms.push(1);
  ms.c.clear();  // protected in std::stack, and it stays that way
}
EOF

expect_fail "unqualified-c" "C++98 two-phase lookup needs the this->" <<'EOF'
#include <deque>
#include <stack>
// The same class with `this->` removed from one accessor. `c` lives in a base
// that depends on the template parameters, so unqualified lookup will not
// search it at template-definition time. This is why MutantStack.hpp says
// this->c.begin() and not c.begin().
template <typename T, typename Container = std::deque<T> >
class Broken : public std::stack<T, Container> {
 public:
  typedef typename Container::iterator iterator;
  iterator begin() { return c.begin(); }
};
void f() {
  Broken<int> b;
  (void)b.begin();
}
EOF

expect_fail "write-through-const-iterator" "const iteration is read-only" <<'EOF'
#include "MutantStack.hpp"
void f() {
  MutantStack<int> ms;
  ms.push(1);
  const MutantStack<int>& c = ms;
  *c.begin() = 2;
}
EOF

expect_fail "const-iterator-to-iterator" "const_iterator does not decay" <<'EOF'
#include "MutantStack.hpp"
void f() {
  MutantStack<int> ms;
  const MutantStack<int>& c = ms;
  MutantStack<int>::iterator it = c.begin();
  (void)it;
}
EOF

expect_fail "top-on-const-is-const" "the const top() returns const T&" <<'EOF'
#include "MutantStack.hpp"
void f() {
  MutantStack<int> ms;
  ms.push(1);
  const MutantStack<int>& c = ms;
  c.top() = 2;
}
EOF

expect_fail "push-on-const" "push() is not const" <<'EOF'
#include "MutantStack.hpp"
void f() {
  const MutantStack<int> ms;
  ms.push(1);
}
EOF

expect_fail "list-has-no-random-access" "a list-backed stack is bidirectional" <<'EOF'
#include <list>
#include "MutantStack.hpp"
void f() {
  MutantStack<int, std::list<int> > ms;
  ms.push(1);
  ms.push(2);
  (void)(ms.begin() + 1);  // random access, which std::list does not offer
}
EOF

expect_fail "sort-a-list-backed-stack" "and std::sort needs random access" <<'EOF'
#include <algorithm>
#include <list>
#include "MutantStack.hpp"
void f() {
  MutantStack<int, std::list<int> > ms;
  ms.push(2);
  ms.push(1);
  std::sort(ms.begin(), ms.end());
}
EOF

expect_fail "stack-is-not-a-mutantstack" "the conversion only runs one way" <<'EOF'
#include <stack>
#include "MutantStack.hpp"
void f() {
  std::stack<int> s;
  MutantStack<int> ms = s;  // slicing works; the reverse does not
}
EOF

expect_fail "mismatched-containers" "MutantStack<int> and its vector twin" <<'EOF'
#include <vector>
#include "MutantStack.hpp"
void f() {
  MutantStack<int> deqBacked;
  MutantStack<int, std::vector<int> > vecBacked;
  deqBacked = vecBacked;  // different types, no conversion
}
EOF

expect_fail "container-ctor-is-explicit" "no implicit deque -> MutantStack" <<'EOF'
#include <deque>
#include "MutantStack.hpp"
void takesStack(const MutantStack<int>&) {}
void f() {
  std::deque<int> d(3, 1);
  takesStack(d);
}
EOF

expect_fail "greedy-angle-brackets" "C++98 needs the space in > >" <<'EOF'
#include <vector>
#include "MutantStack.hpp"
void f() {
  MutantStack<int, std::vector<int>> ms;  // >> lexes as right-shift in C++98
  (void)ms;
}
EOF

printf "\n"
if [ "$wrong" -eq 0 ]; then
  printf "${GREEN}${BOLD}All %d cases behaved as documented.${OFF}\n" "$total"
  exit 0
fi
printf "${RED}${BOLD}%d of %d cases did not${OFF} - a note in main.cpp is now a lie\n" \
  "$wrong" "$total"
exit 1
