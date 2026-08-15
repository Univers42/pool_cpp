#!/bin/sh
# Some of what Array<T> guarantees is the type system's job, not the runtime's:
# explicit on the sized constructor, the const operator[] refusing writes, and
# Array<int> and Array<double> having nothing to do with each other. test.cpp
# cannot check any of it, so it lives here. Everything must fail to build
# except the control case.
#
#   ./tests/compile_fail.sh   (from ex02/ or from tests/)

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
OUT="build/nocompile"

BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"; GREY="\033[90m"

mkdir -p "$OUT"
total=0; wrong=0

# expect_fail <name> <what rule this proves>   - snippet arrives on stdin
expect_fail() {
  total=$((total + 1))
  cat > "$OUT/case.cpp"
  if $CXX $STD -I. -c "$OUT/case.cpp" -o "$OUT/case.o" 2>"$OUT/case.log"; then
    printf "  ${RED}%-9s${OFF} %-24s %s\n" "COMPILED" "$1" "$2"
    wrong=$((wrong + 1))
  else
    printf "  ${GREEN}%-9s${OFF} %-24s ${GREY}%s${OFF}\n" "rejected" "$1" "$2"
  fi
}

# expect_ok <name> <what it shows> - the control
expect_ok() {
  total=$((total + 1))
  cat > "$OUT/case.cpp"
  if $CXX $STD -Wall -Wextra -Werror -I. -c "$OUT/case.cpp" -o "$OUT/case.o" \
      2>"$OUT/case.log"; then
    printf "  ${GREEN}%-9s${OFF} %-24s ${GREY}%s${OFF}\n" "accepted" "$1" "$2"
  else
    printf "  ${RED}%-9s${OFF} %-24s %s\n" "REJECTED" "$1" "$2"
    wrong=$((wrong + 1))
    sed 's/^/      /' "$OUT/case.log"
  fi
}

printf "${BOLD}ex02 compile-failure tests${OFF}  - these must NOT compile\n\n"

expect_ok "control" "ordinary correct usage compiles warning-free" <<'EOF'
#include <string>
#include "Array.hpp"
void f() {
  Array<int> a(3);
  a[0] = 1;
  const Array<int>& c = a;
  (void)c[0];
  Array<int> b(a);
  b = a;
  Array<std::string> s(2);
  s[0] = "x";
  Array<Array<int> > nested(2);
  nested[0] = a;
}
EOF

expect_fail "implicit-size-ctor" "explicit Array(unsigned int) blocks int -> Array" <<'EOF'
#include "Array.hpp"
void f() {
  Array<int> a = 5;  // would silently mean "a 5-element array"
}
EOF

expect_fail "implicit-arg-conversion" "and blocks it at a call site too" <<'EOF'
#include "Array.hpp"
void takesArray(const Array<int>&) {}
void f() {
  takesArray(5);  // explicit stops 5 becoming a 5-element array
}
EOF

expect_fail "write-through-const" "the const overload returns const T&" <<'EOF'
#include "Array.hpp"
void f() {
  Array<int> a(3);
  const Array<int>& c = a;
  c[0] = 42;  // read-only
}
EOF

expect_fail "unrelated-instantiations" "Array<int> and Array<double> do not convert" <<'EOF'
#include "Array.hpp"
void f() {
  Array<double> d(3);
  Array<int> a = d;
}
EOF

expect_fail "assign-across-types" "nor does assignment between them" <<'EOF'
#include <string>
#include "Array.hpp"
void f() {
  Array<int> a(3);
  Array<std::string> s(3);
  a = s;
}
EOF

expect_fail "no-size-conversion" "size() is unsigned int, not an Array" <<'EOF'
#include "Array.hpp"
void f() {
  Array<int> a(3);
  Array<int> b;
  b = a.size();  // an unsigned int is not an Array<int>
}
EOF

expect_fail "size-is-const" "size() is const and returns a value, not a handle" <<'EOF'
#include "Array.hpp"
void f() {
  Array<int> a(3);
  a.size() = 10;
}
EOF

expect_fail "private-buffer" "the buffer is private; ownership is not negotiable" <<'EOF'
#include "Array.hpp"
void f() {
  Array<int> a(3);
  delete[] a._array;
}
EOF

expect_fail "nongreedy-angle-brackets" "C++98 needs the space in > >" <<'EOF'
#include "Array.hpp"
void f() {
  Array<Array<int>> nested(2);  // >> lexes as right-shift in C++98
}
EOF

expect_fail "element-needs-default-ctor" "new T[n]() requires a default ctor" <<'EOF'
#include "Array.hpp"
class NoDefault {
 public:
  explicit NoDefault(int x) : v(x) {}
  int v;
};
void f() {
  Array<NoDefault> a(3);
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
