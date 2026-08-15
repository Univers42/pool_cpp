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

# The eval sheet's "complex types" question, taken literally. std::complex is
# the one type in the standard library that splits the three functions apart:
# it is copyable and assignable, so swap works, but it has no ordering at all,
# so min and max cannot be instantiated. Both halves are checked here, and the
# accepted half is what stops this pair from passing for the wrong reason.
expect_ok "complex-swap" "swap needs only copy ctor + copy assign" <<'EOF'
#include <complex>
#include "whatever.hpp"
void f() {
  std::complex<double> a(3.0, 4.0), b(1.0, -2.0);
  ::swap(a, b);
}
EOF

expect_fail "complex-no-ordering" "std::complex has == but no <, so no min" <<'EOF'
#include <complex>
#include "whatever.hpp"
void f() {
  std::complex<double> a(3.0, 4.0), b(1.0, -2.0);
  (void)::min(a, b);
}
EOF

expect_fail "complex-no-ordering-max" "and no >, so no max either" <<'EOF'
#include <complex>
#include "whatever.hpp"
void f() {
  std::complex<double> a(3.0, 4.0), b(1.0, -2.0);
  (void)::max(a, b);
}
EOF

# The std::string version of the unqualified call compiles and quietly picks
# std::swap; this one cannot, because std::swap<complex> is exactly as generic
# as ours. Same missing ::, two different outcomes - that is the whole point of
# main.cpp section 3.
expect_fail "complex-unqualified-swap" "ADL finds an equally generic std::swap" <<'EOF'
#include <complex>
#include "whatever.hpp"
void f() {
  std::complex<double> a(3.0, 4.0), b(1.0, -2.0);
  swap(a, b);
}
EOF

# An abstract base splits the three functions the same way std::complex did,
# but on the other axis. std::complex was missing operators; Animal is missing
# the ability to exist. min and max only ever take and return const T&, so they
# do not care; swap opens with T temp = a; and cannot proceed. main.cpp
# section 10 says all of this out loud, so all of it is checked here.
expect_fail "abstract-no-object" "a pure virtual member leaves no complete type" <<'EOF'
#include "whatever.hpp"
class Animal {
 public:
  virtual ~Animal() {}
  virtual void makeSound() const = 0;
};
void f() {
  Animal a;
  (void)a;
}
EOF

expect_fail "abstract-no-array" "and therefore no array of them either" <<'EOF'
#include "whatever.hpp"
class Animal {
 public:
  virtual ~Animal() {}
  virtual void makeSound() const = 0;
};
void f() {
  Animal zoo[4];
  (void)zoo;
}
EOF

expect_fail "swap-abstract" "swap's T temp = a; cannot build an abstract T" <<'EOF'
#include "whatever.hpp"
class Animal {
 public:
  virtual ~Animal() {}
  virtual void makeSound() const = 0;
};
class Dog : public Animal {
 public:
  void makeSound() const {}
};
void f() {
  Dog d1, d2;
  Animal& a = d1;
  Animal& b = d2;
  ::swap(a, b);  // swap<Animal>, and Animal cannot be instantiated
}
EOF

# The accepted half, and the reason the pair above proves something: min and
# max take an abstract T without complaint, because no T is ever constructed.
# The array of POINTERS is here too - that is the array the subject asks for.
expect_ok "min-max-abstract" "min/max never build a T, so abstract is fine" <<'EOF'
#include "whatever.hpp"
class Animal {
 public:
  virtual ~Animal() {}
  virtual void makeSound() const = 0;
  bool operator<(const Animal& o) const { return this < &o; }
  bool operator>(const Animal& o) const { return this > &o; }
};
class Dog : public Animal {
 public:
  void makeSound() const {}
};
void f() {
  Animal* zoo[2];
  zoo[0] = new Dog();
  zoo[1] = new Dog();
  (void)::min(*zoo[0], *zoo[1]);
  (void)::max(*zoo[0], *zoo[1]);
  ::swap(zoo[0], zoo[1]);  // swap<Animal*>: the pointers move, not the Dogs
  delete zoo[0];
  delete zoo[1];
}
EOF

# Section 7 again, with a hierarchy instead of int and double. Two siblings do
# not meet in the middle at their base: deduction reports Dog and Cat and gives
# up. Naming T is the fix, exactly as ::min<double>(1, 2.5) was.
expect_fail "sibling-deduction" "Dog& and Cat& deduce two different Ts" <<'EOF'
#include "whatever.hpp"
class Animal {
 public:
  virtual ~Animal() {}
  virtual void makeSound() const = 0;
  bool operator<(const Animal& o) const { return this < &o; }
};
class Dog : public Animal { public: void makeSound() const {} };
class Cat : public Animal { public: void makeSound() const {} };
void f() {
  Dog d;
  Cat c;
  (void)::min(d, c);
}
EOF

expect_ok "sibling-explicit-T" "naming T stops deduction and lets both convert" <<'EOF'
#include "whatever.hpp"
class Animal {
 public:
  virtual ~Animal() {}
  virtual void makeSound() const = 0;
  bool operator<(const Animal& o) const { return this < &o; }
};
class Dog : public Animal { public: void makeSound() const {} };
class Cat : public Animal { public: void makeSound() const {} };
void f() {
  Dog d;
  Cat c;
  (void)::min<Animal>(d, c);
}
EOF

# My first draft of main.cpp wrote makeNoise in Cat. Against a non-virtual or
# a merely virtual base that is a silent bug - a second function nobody calls.
# Against a PURE virtual one it is a build error, because Cat never overrides
# anything and stays abstract.
expect_fail "misspelled-override" "a typo'd override leaves the derived abstract" <<'EOF'
#include "whatever.hpp"
class Animal {
 public:
  virtual ~Animal() {}
  virtual void makeSound() const = 0;
};
class Cat : public Animal {
 public:
  void makeNoise() const {}  // not an override of anything
};
void f() { delete new Cat(); }
EOF

# Animal::operator= is protected so this cannot be written by accident: two
# base references, one assignment, and a Dog quietly loses its Brain.
expect_fail "slicing-assignment" "a protected operator= blocks base-ref slicing" <<'EOF'
#include "whatever.hpp"
class Animal {
 public:
  virtual ~Animal() {}
  virtual void makeSound() const = 0;
 protected:
  Animal& operator=(const Animal&) { return *this; }
};
class Dog : public Animal {
 public:
  void makeSound() const {}
};
void f() {
  Dog d1, d2;
  Animal& a = d1;
  Animal& b = d2;
  a = b;
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
