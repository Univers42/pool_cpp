#!/bin/sh
# test.cpp can only check things that compile, but half of what a template
# promises is the code it turns down. Everything here must fail to build. The
# one control case must build, otherwise a broken script would "pass" the lot.
#
#   ./tests/compile_fail.sh   (from ex00/ or from tests/)

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
    printf "  ${RED}%-9s${OFF} %-22s %s\n" "COMPILED" "$1" "$2"
    wrong=$((wrong + 1))
  else
    printf "  ${GREEN}%-9s${OFF} %-22s ${GREY}%s${OFF}\n" "rejected" "$1" "$2"
  fi
}

# expect_ok <name> <what it shows> - the control: proves the harness can pass
expect_ok() {
  total=$((total + 1))
  cat > "$OUT/case.cpp"
  if $CXX $STD -Wall -Wextra -Werror -I. -c "$OUT/case.cpp" -o "$OUT/case.o" \
      2>"$OUT/case.log"; then
    printf "  ${GREEN}%-9s${OFF} %-22s ${GREY}%s${OFF}\n" "accepted" "$1" "$2"
  else
    printf "  ${RED}%-9s${OFF} %-22s %s\n" "REJECTED" "$1" "$2"
    wrong=$((wrong + 1))
    sed 's/^/      /' "$OUT/case.log"
  fi
}

printf "${BOLD}ex00 compile-failure tests${OFF}  - these must NOT compile\n\n"

expect_ok "control" "the same code, correct, does compile" <<'EOF'
#include <string>
#include "whatever.hpp"
void f() {
  std::string a = "x", b = "y";
  ::swap(a, b);
  (void)::min(a, b);
  (void)::max(a, b);
}
EOF

# NOTE: the std::string version of this does NOT belong here - swap(str, str)
# compiles happily and quietly calls std::swap's basic_string overload, which
# is more specialised and therefore preferred. That silent substitution is
# demonstrated at run time in main.cpp section 3 and asserted in test.cpp.
# An unqualified call is only a hard ERROR when the other candidate is exactly
# as generic as ours, so neither can be preferred:
expect_fail "unqualified-swap" "no :: - two equally generic swaps, ambiguous" <<'EOF'
#include "whatever.hpp"
namespace lib {
struct Widget { int v; };
template <typename T> void swap(T& a, T& b) { T t = a; a = b; b = t; }
}
void f() {
  lib::Widget a, b;
  swap(a, b);  // ADL finds lib::swap, ordinary lookup finds ::swap
}
EOF

expect_fail "mixed-deduction" "T cannot be deduced as int and double at once" <<'EOF'
#include "whatever.hpp"
void f() { (void)::min(1, 2.5); }
EOF

expect_fail "swap-const-args" "swap takes T&, so const objects are refused" <<'EOF'
#include "whatever.hpp"
void f() {
  const int a = 1, b = 2;
  ::swap(a, b);
}
EOF

expect_fail "swap-temporaries" "T& cannot bind to a literal" <<'EOF'
#include "whatever.hpp"
void f() { ::swap(1, 2); }
EOF

expect_fail "write-through-min" "min returns const T&, so the result is read-only" <<'EOF'
#include "whatever.hpp"
void f() {
  int a = 1, b = 2;
  ::min(a, b) = 99;
}
EOF

expect_fail "no-operator-less" "the type must supply the operation min uses" <<'EOF'
#include "whatever.hpp"
struct Opaque { int v; };
void f() {
  Opaque a = {1}, b = {2};
  (void)::min(a, b);
}
EOF

expect_fail "no-operator-greater" "operator< alone is not enough for max" <<'EOF'
#include "whatever.hpp"
struct OnlyLess {
  int v;
  bool operator<(const OnlyLess& o) const { return v < o.v; }
};
void f() {
  OnlyLess a, b;
  a.v = 1; b.v = 2;
  (void)::max(a, b);
}
EOF

expect_fail "swap-not-assignable" "swap needs copy-assignment, not just a ctor" <<'EOF'
#include "whatever.hpp"
class NoAssign {
 public:
  NoAssign() : v(0) {}
  NoAssign(const NoAssign& o) : v(o.v) {}
  int v;
 private:
  NoAssign& operator=(const NoAssign&);  // declared, never defined
};
void f() {
  NoAssign a, b;
  ::swap(a, b);
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
