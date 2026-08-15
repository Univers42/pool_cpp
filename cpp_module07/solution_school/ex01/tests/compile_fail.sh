#!/bin/sh
# The claim of this exercise is that deduction alone enforces const, with no
# second overload and no const_cast. That only means something if the bad code
# really is rejected, and test.cpp cannot check it: a test that does not
# compile is not a test. Everything here must fail to build, except the one
# control case, otherwise a broken script would "pass" the lot.
#
#   ./tests/compile_fail.sh   (from ex01/ or from tests/)

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

printf "${BOLD}ex01 compile-failure tests${OFF}  - these must NOT compile\n\n"

expect_ok "control" "const array + const-ref callback compiles" <<'EOF'
#include "iter.hpp"
void reader(const int& n) { (void)n; }
void f() {
  const int a[] = {1, 2, 3};
  ::iter(a, 3, reader);
}
EOF

# The headline case: this is the experiment the README describes, kept as an
# executable check instead of a claim.
expect_fail "const-array-mutator" "T deduces const int; int& will not bind" <<'EOF'
#include "iter.hpp"
void increment(int& n) { n++; }
void f() {
  const int a[] = {1, 2, 3};
  ::iter(a, 3, increment);  // binding int& to const int discards qualifiers
}
EOF

expect_fail "const-string-mutator" "same rule for a class type" <<'EOF'
#include <string>
#include "iter.hpp"
void shout(std::string& s) { s += "!"; }
void f() {
  const std::string a[] = {"x", "y"};
  ::iter(a, 2, shout);
}
EOF

expect_fail "const-functor-mutator" "a functor gets no exemption" <<'EOF'
#include "iter.hpp"
struct Bump { void operator()(int& n) const { n++; } };
void f() {
  const int a[] = {1, 2};
  ::iter(a, 2, Bump());
}
EOF

expect_fail "wrong-arity" "the callback must take exactly one argument" <<'EOF'
#include "iter.hpp"
void twoArgs(int& a, int& b) { a += b; }
void f() {
  int a[] = {1, 2};
  ::iter(a, 2, twoArgs);
}
EOF

expect_fail "incompatible-element" "f(array[i]) must be a valid expression" <<'EOF'
#include <string>
#include "iter.hpp"
void takesInt(const int& n) { (void)n; }
void f() {
  std::string a[] = {"x", "y"};
  ::iter(a, 2, takesInt);
}
EOF

expect_fail "not-callable" "F deduces anything, but it must be callable" <<'EOF'
#include "iter.hpp"
void f() {
  int a[] = {1, 2};
  ::iter(a, 2, 42);  // an int is not a callable
}
EOF

expect_fail "overloaded-callback" "an overload set cannot deduce a single F" <<'EOF'
#include "iter.hpp"
void handle(int& n) { n++; }
void handle(double& d) { d += 1.0; }
void f() {
  int a[] = {1, 2};
  ::iter(a, 2, handle);  // which handle? deduction cannot choose
}
EOF

expect_fail "uninstantiated-template" "a template NAME is not a callable" <<'EOF'
#include "iter.hpp"
template <typename T>
void print(const T& elem) { (void)elem; }
void f() {
  int a[] = {1, 2};
  ::iter(a, 2, print);  // needs print<int>, an instantiated one
}
EOF

expect_fail "array-not-pointer" "a length must be supplied; nothing deduces it" <<'EOF'
#include "iter.hpp"
void reader(const int& n) { (void)n; }
void f() {
  int a[] = {1, 2, 3};
  ::iter(a, reader);  // no two-argument iter exists
}
EOF

# The same const rule, now on a user-defined element. vect2 has two operator[]
# overloads, so const-ness does not stop at the element - it reaches every
# member the callback calls. main.cpp section 8 says so; these check it.
expect_ok "vect2-const-reader" "a const vect2 array reads fine" <<'EOF'
#include "iter.hpp"
#include "vect2.hpp"
void reader(const vect2& v) { (void)v[0]; }
void f() {
  const vect2 path[] = {vect2(1, 2), vect2(3, 4)};
  ::iter(path, 2, reader);
}
EOF

expect_fail "vect2-const-mutator" "T deduces const vect2; vect2& will not bind" <<'EOF'
#include "iter.hpp"
#include "vect2.hpp"
void bump(vect2& v) { ++v; }
void f() {
  const vect2 path[] = {vect2(1, 2), vect2(3, 4)};
  ::iter(path, 2, bump);
}
EOF

expect_fail "vect2-const-subscript" "operator[] const returns int, not int&" <<'EOF'
#include "iter.hpp"
#include "vect2.hpp"
void writer(const vect2& v) { const_cast<vect2&>(v)[0] = 1; (void)v; }
void takesRef(const vect2& v) { v[0] = 9; }  // the const overload gives an int
void f() {
  const vect2 path[] = {vect2(1, 2)};
  ::iter(path, 1, takesRef);
  (void)writer;
}
EOF

# An array of arrays: T deduces to an array type, the callback binds a
# reference to a whole row, and only the INNER length is part of that type.
expect_ok "nested-iter" "T can be vect2[4], and iter can call itself" <<'EOF'
#include <cstddef>
#include "iter.hpp"
#include "vect2.hpp"
void bump(vect2& v) { ++v; }
template <size_t N>
void bumpRow(vect2 (&row)[N]) { ::iter(row, N, bump); }
void f() {
  vect2 quads[2][4];
  ::iter(quads, 2, bumpRow<4>);
}
EOF

expect_fail "nested-iter-const" "const rides through both levels" <<'EOF'
#include <cstddef>
#include "iter.hpp"
#include "vect2.hpp"
void bump(vect2& v) { ++v; }
template <size_t N>
void bumpRow(vect2 (&row)[N]) { ::iter(row, N, bump); }
void f() {
  const vect2 quads[2][4] = {{vect2(), vect2(), vect2(), vect2()},
                             {vect2(), vect2(), vect2(), vect2()}};
  ::iter(quads, 2, bumpRow<4>);
}
EOF

expect_fail "nested-wrong-extent" "the row's length is part of its type" <<'EOF'
#include <cstddef>
#include "iter.hpp"
#include "vect2.hpp"
void bump(vect2& v) { ++v; }
template <size_t N>
void bumpRow(vect2 (&row)[N]) { ::iter(row, N, bump); }
void f() {
  vect2 quads[2][4];
  ::iter(quads, 2, bumpRow<3>);  // vect2(&)[3] cannot bind to a vect2[4] row
}
EOF

# bigint's constructors are explicit, so nothing converts behind your back on
# the way into a callback - the digit has to be spelled bigint(digit).
expect_fail "bigint-explicit-ctor" "an explicit ctor blocks the silent int" <<'EOF'
#include "bigint.hpp"
#include "iter.hpp"
void add(const bigint& n) { (void)n; }
void f() {
  int digits[] = {1, 2, 3};
  ::iter(digits, 3, add);  // int does not convert to bigint on its own
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
