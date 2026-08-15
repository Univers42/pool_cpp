#!/bin/sh
# Half of what easyfind guarantees is the type system's job, not the runtime's:
# the const overload refusing writes, the template refusing containers whose
# elements do not compare with an int, and C arrays not being containers at
# all. tests/test.cpp cannot check any of it — a test that fails to compile is
# not a failing test, it is a broken build — so it lives here. Everything must
# fail to build except the control cases.
#
#   ./tests/compile_fail.sh   (from ex00/ or from tests/)

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
# Compiled WITHOUT the warning flags, so a rejection here is a real type
# error and never a warning that -Werror happened to promote.
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

# expect_ok <name> <what it shows> - the control
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

# expect_warn <name> <what it shows> - legal C++98 that the required flags
# turn into an error. Proves -Wall -Wextra -Werror is doing work the standard
# alone would not do.
expect_warn() {
  total=$((total + 1))
  cat > "$OUT/case.cpp"
  if ! $CXX $STD -I. -c "$OUT/case.cpp" -o "$OUT/case.o" 2>"$OUT/case.log"; then
    printf "  ${RED}FAIL${OFF} %-9s %-26s %s\n" "ILLEGAL" "$1" "$2"
    wrong=$((wrong + 1))
    return
  fi
  if $CXX $STD $WARN -I. -c "$OUT/case.cpp" -o "$OUT/case.o" \
      2>"$OUT/case.log"; then
    printf "  ${RED}FAIL${OFF} %-9s %-26s %s\n" "NOWARN" "$1" "$2"
    wrong=$((wrong + 1))
  else
    printf "  ${GREEN}ok${OFF}   %-9s %-26s ${GREY}%s${OFF}\n" "-Werror" "$1" "$2"
  fi
}

printf "${BOLD}ex00 compile-failure tests${OFF}  17 cases, each with a required outcome\n"
printf "${GREY}  every line should say ok. 'rejected' is the compiler refusing bad\n  code on purpose, which is the result being tested for.${OFF}\n\n"

expect_ok "control" "ordinary correct usage, warning-free" <<'EOF'
#include <deque>
#include <list>
#include <set>
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<int> v(3, 1);
  std::list<int> l(3, 1);
  std::deque<int> d(3, 1);
  std::set<int> s;
  s.insert(1);
  const std::vector<int>& c = v;
  *easyfind(v, 1) = 2;
  (void)easyfind(l, 1);
  (void)easyfind(d, 1);
  (void)easyfind(s, 1);
  std::vector<int>::const_iterator ci = easyfind(c, 2);
  (void)ci;
}
EOF

expect_ok "container-of-long" "the element only has to compare with an int" <<'EOF'
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<long> v(3, 1L);
  (void)easyfind(v, 1);  // long == int is a valid comparison
}
EOF

expect_ok "element-with-operator-eq" "a class element works if it compares" <<'EOF'
#include <vector>
#include "easyfind.hpp"
struct Badge {
  int id;
  Badge() : id(0) {}
  bool operator==(int other) const { return id == other; }
};
void f() {
  std::vector<Badge> v(3);
  (void)easyfind(v, 0);
}
EOF

expect_fail "write-through-const" "the const overload returns const_iterator" <<'EOF'
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<int> v(3, 1);
  const std::vector<int>& c = v;
  *easyfind(c, 1) = 2;  // read-only
}
EOF

expect_fail "const-result-to-mutable" "and it will not decay into iterator" <<'EOF'
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<int> v(3, 1);
  const std::vector<int>& c = v;
  std::vector<int>::iterator it = easyfind(c, 1);
  (void)it;
}
EOF

expect_fail "temporary-is-const" "a temporary binds to the const overload" <<'EOF'
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<int>::iterator it = easyfind(std::vector<int>(3, 1), 1);
  (void)it;
}
EOF

expect_fail "c-array" "a raw array has no T::iterator" <<'EOF'
#include "easyfind.hpp"
void f() {
  int raw[3] = {1, 2, 3};
  (void)easyfind(raw, 2);
}
EOF

expect_fail "std-map" "map's value_type is a pair, not an int" <<'EOF'
#include <map>
#include "easyfind.hpp"
void f() {
  std::map<int, int> m;
  (void)easyfind(m, 1);  // *it is a pair<const int, int>
}
EOF

expect_fail "vector-of-strings" "no operator==(std::string, int)" <<'EOF'
#include <string>
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<std::string> v(3, "x");
  (void)easyfind(v, 42);
}
EOF

expect_fail "element-without-eq" "an element that compares with nothing" <<'EOF'
#include <vector>
#include "easyfind.hpp"
struct Opaque {
  int id;
  Opaque() : id(0) {}
};
void f() {
  std::vector<Opaque> v(3);
  (void)easyfind(v, 0);
}
EOF

expect_fail "not-a-container" "an int is not a T with begin()/end()" <<'EOF'
#include "easyfind.hpp"
void f() {
  int x = 5;
  (void)easyfind(x, 5);
}
EOF

expect_fail "missing-argument" "both parameters are required" <<'EOF'
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<int> v(3, 1);
  (void)easyfind(v);
}
EOF

expect_fail "string-needle" "the second parameter is an int, not anything" <<'EOF'
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<int> v(3, 1);
  (void)easyfind(v, "1");  // const char* does not convert to int
}
EOF

expect_fail "wrong-explicit-arg" "an explicit T must match the argument" <<'EOF'
#include <list>
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<int> v(3, 1);
  (void)easyfind<std::list<int> >(v, 1);
}
EOF

expect_fail "missing-typename" "T::iterator is a dependent name in C++98" <<'EOF'
#include <vector>
#include "easyfind.hpp"
template <typename T>
void caller(T& c) {
  T::iterator it = easyfind(c, 1);  // needs 'typename T::iterator'
  (void)it;
}
void f() {
  std::vector<int> v(3, 1);
  caller(v);
}
EOF

expect_fail "greedy-angle-brackets" "C++98 needs the space in > >" <<'EOF'
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<std::vector<int>> rows;  // >> lexes as right-shift in C++98
  (void)easyfind(rows[0], 1);
}
EOF

expect_warn "double-needle" "3.9 silently becomes 3 without -Werror" <<'EOF'
#include <vector>
#include "easyfind.hpp"
void f() {
  std::vector<int> v(3, 1);
  (void)easyfind(v, 3.9);  // legal C++98, and almost certainly a bug
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
