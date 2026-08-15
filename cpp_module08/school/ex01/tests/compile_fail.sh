#!/bin/sh
# Half of what Span guarantees is the type system's job: explicit on the sized
# constructor, read-only iteration, private storage, and no friend anywhere.
# tests/test.cpp cannot check any of it — a test that fails to compile is not
# a failing test, it is a broken build — so it lives here. Everything must
# fail to build except the control cases.
#
#   ./tests/compile_fail.sh   (from ex01/ or from tests/)

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

printf "${BOLD}ex01 compile-failure tests${OFF}  18 cases, each with a required outcome\n"
printf "${GREY}  every line should say ok. 'rejected' is the compiler refusing bad\n  code on purpose, which is the result being tested for.${OFF}\n\n"

expect_ok "control" "ordinary correct usage, warning-free" <<'EOF'
#include <iostream>
#include <list>
#include <vector>
#include "Span.hpp"
void f() {
  Span sp(5);
  sp.addNumber(1);
  std::vector<int> v(3, 2);
  sp.addNumber(v.begin(), v.end());
  const Span& c = sp;
  (void)c.shortestSpan();
  (void)c.longestSpan();
  (void)c.size();
  (void)c.maxSize();
  (void)c.empty();
  (void)c.full();
  Span copy(sp);
  Span assigned;
  assigned = sp;
  std::cout << sp;
  for (Span::const_iterator it = c.begin(); it != c.end(); ++it) (void)*it;
}
EOF

expect_ok "easyfind-accepts-a-span" "ex00's template, unchanged, takes a Span" <<'EOF'
#include <algorithm>
#include <stdexcept>
#include "Span.hpp"
// Copied verbatim from ex00/easyfind.hpp. It compiles against Span because
// Span exposes begin(), end() and a const_iterator typedef - nothing else was
// ever required of a container.
template <typename T>
typename T::const_iterator easyfind(const T& container, int value) {
  typename T::const_iterator it =
      std::find(container.begin(), container.end(), value);
  if (it == container.end())
    throw std::out_of_range("easyfind: value not found");
  return (it);
}
void f() {
  Span sp(3);
  sp.addNumber(7);
  (void)easyfind(sp, 7);
}
EOF

expect_ok "catch-by-nested-type" "the exception classes are public" <<'EOF'
#include "Span.hpp"
void f() {
  Span sp(1);
  try {
    sp.addNumber(1);
    sp.addNumber(2);
  } catch (const Span::SpanFullException&) {
  } catch (const Span::NotEnoughElementsException&) {
  }
}
EOF

expect_fail "implicit-size-ctor" "explicit Span(unsigned int) blocks int -> Span" <<'EOF'
#include "Span.hpp"
void f() {
  Span sp = 5;  // would silently mean "a Span of capacity 5"
}
EOF

expect_fail "implicit-arg-conversion" "and blocks it at a call site too" <<'EOF'
#include "Span.hpp"
void takesSpan(const Span&) {}
void f() {
  takesSpan(5);
}
EOF

expect_fail "write-through-iterator" "iteration is read-only by design" <<'EOF'
#include "Span.hpp"
void f() {
  Span sp(3);
  sp.addNumber(1);
  *sp.begin() = 42;  // const_iterator
}
EOF

expect_fail "no-mutable-iterator" "Span deliberately has no ::iterator" <<'EOF'
#include "Span.hpp"
void f() {
  Span sp(3);
  Span::iterator it = sp.begin();
  (void)it;
}
EOF

expect_fail "sort-a-span" "algorithms that write cannot touch it" <<'EOF'
#include <algorithm>
#include "Span.hpp"
void f() {
  Span sp(3);
  sp.addNumber(2);
  sp.addNumber(1);
  std::sort(sp.begin(), sp.end());  // would resize N's meaning out from under it
}
EOF

expect_fail "private-storage" "the vector and the capacity are private" <<'EOF'
#include "Span.hpp"
void f() {
  Span sp(3);
  sp._vec.push_back(1);
}
EOF

expect_fail "private-capacity" "and so is _maxSize, so N cannot be raised" <<'EOF'
#include "Span.hpp"
void f() {
  Span sp(3);
  sp._maxSize = 1000;
}
EOF

expect_fail "add-to-const-span" "addNumber is not const" <<'EOF'
#include "Span.hpp"
void f() {
  const Span sp(3);
  sp.addNumber(1);
}
EOF

expect_fail "add-range-to-const-span" "nor is the range overload" <<'EOF'
#include <vector>
#include "Span.hpp"
void f() {
  const Span sp(3);
  std::vector<int> v(2, 1);
  sp.addNumber(v.begin(), v.end());
}
EOF

expect_fail "query-returns-a-value" "shortestSpan() is not a handle" <<'EOF'
#include "Span.hpp"
void f() {
  Span sp(3);
  sp.shortestSpan() = 5;
}
EOF

expect_fail "no-equality-operator" "Span does not claim to be comparable" <<'EOF'
#include "Span.hpp"
void f() {
  Span a(3);
  Span b(3);
  if (a == b) {
  }
}
EOF

expect_fail "not-a-vector" "a Span is not convertible to its storage" <<'EOF'
#include <vector>
#include "Span.hpp"
void f() {
  Span sp(3);
  std::vector<int> stolen = sp;
}
EOF

expect_fail "range-missing-argument" "the range overload needs both ends" <<'EOF'
#include <vector>
#include "Span.hpp"
void f() {
  Span sp(3);
  std::vector<int> v(2, 1);
  sp.addNumber(v.begin());
}
EOF

expect_fail "generic-needs-iterator" "generic code asking for T::iterator" <<'EOF'
#include "Span.hpp"
// A template written against mutable containers cannot accept a Span, which
// is the intended consequence of exposing only const_iterator.
template <typename T>
void mutateFirst(T& container) {
  typename T::iterator it = container.begin();
  *it = 0;
}
void f() {
  Span sp(3);
  mutateFirst(sp);
}
EOF

expect_fail "greedy-angle-brackets" "C++98 needs the space in > >" <<'EOF'
#include <vector>
#include "Span.hpp"
void f() {
  std::vector<std::vector<int>> rows;  // >> lexes as right-shift in C++98
  Span sp(1);
  (void)rows;
  (void)sp;
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
