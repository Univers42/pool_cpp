#!/bin/sh
# Mutation test for ex02 identify().
#
# A passing test suite proves nothing until you show it can fail. Each row
# injects one realistic defect and requires tests/test.cpp to catch it.
#
# Two outcomes count as success:
#   killed    the mutant built, and the tests failed it
#   rejected  the mutant did not compile — for dynamic_cast that IS the
#             correct detection, so those rows use `mutate_rejected`
#
#   ./tests/mutants.sh        (from ex02/, or from tests/ — either works)

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
SRC="Functions.cpp"           # the file under mutation
DEPS="Base.cpp"               # other sources the tests need
TEST="tests/test.cpp"
OUT="build/mutants"

BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"; GREY="\033[90m"

mkdir -p "$OUT"
total=0; survived=0; broken=0

build_mutant() {                       # $1 = sed expr on $SRC, $2 = extra file
  sed "$1" "$SRC" > "$OUT/$SRC" 2>/dev/null
  cmp -s "$OUT/$SRC" "$SRC" && return 2
  $CXX $STD -w -I. $TEST "$OUT/$SRC" $DEPS -o "$OUT/t" 2>"$OUT/build.log"
}

# mutate <name> <sed-expr> <what defect this simulates>
mutate() {
  total=$((total + 1))
  build_mutant "$2"
  case $? in
    2) printf "  ${RED}%-9s${OFF} %-24s %s\n" "PATTERN" "$1" "sed matched nothing"
       broken=$((broken + 1)); return ;;
    0) ;;
    *) printf "  ${RED}%-9s${OFF} %-24s %s\n" "NOBUILD" "$1" "expected to compile"
       broken=$((broken + 1)); return ;;
  esac
  if "./$OUT/t" >"$OUT/run.log" 2>&1; then
    printf "  ${RED}%-9s${OFF} %-24s %s\n" "SURVIVED" "$1" "$3"
    survived=$((survived + 1))
  else
    printf "  ${GREEN}%-9s${OFF} %-24s ${GREY}%s${OFF}\n" "killed" "$1" "$3"
  fi
}

# For defects the LANGUAGE catches. Compiling is the failure here.
mutate_rejected() {
  total=$((total + 1))
  build_mutant "$2"
  case $? in
    2) printf "  ${RED}%-9s${OFF} %-24s %s\n" "PATTERN" "$1" "sed matched nothing"
       broken=$((broken + 1)) ;;
    0) printf "  ${RED}%-9s${OFF} %-24s %s\n" "SURVIVED" "$1" "compiled, but should not have"
       survived=$((survived + 1)) ;;
    *) printf "  ${GREEN}%-9s${OFF} %-24s ${GREY}%s${OFF}\n" "rejected" "$1" "$3" ;;
  esac
}

printf "${BOLD}ex02 mutation test${OFF}  — 'killed' by the tests, 'rejected' by the compiler\n\n"

mutate "swap-A-and-B" \
  's|if (dynamic_cast<A\*>(p))|if (dynamic_cast<B*>(p))|' \
  "pointer overload reports B for an A"

mutate "drop-C-branch" \
  's|else if (dynamic_cast<C\*>(p))|else if (false)|' \
  "C falls through to Unknown"

mutate "static-instead-of-dynamic" \
  's|if (dynamic_cast<A\*>(p))|if (static_cast<A*>(p))|' \
  "unchecked downcast — always true, so everything is an A"

mutate "tryRef-always-matches" \
  's|^    return false;|    return true;|' \
  "the catch reports success, so everything is an A"

mutate "tryRef-never-matches" \
  's|^    return true;|    return false;|' \
  "no type ever matches, so everything is Unknown"

mutate "reference-order-swapped" \
  's|if (tryRef<A>(p, "A")) return;|if (tryRef<B>(p, "B")) return;|' \
  "reference overload tries B first and mislabels"

mutate "unknown-reported-as-A" \
  's|"Unknown"|"A"|g' \
  "both overloads label an unrecognised object as A"

mutate "generate-always-A" \
  's|return new B();|return new A();|' \
  "generate never produces a B"

# The language-level guarantee: dynamic_cast requires a polymorphic source.
mutate_rejected "base-not-polymorphic" \
  's|#include "Functions.hpp"|#include "Functions.hpp"\nstruct NotPoly { int x; };\nstruct NotPolyChild : NotPoly { int y; };\nstatic void probe(NotPoly* p) { (void)dynamic_cast<NotPolyChild*>(p); }|' \
  "dynamic_cast on a non-polymorphic type"

printf "\n"
if [ "$survived" -eq 0 ] && [ "$broken" -eq 0 ]; then
  printf "${GREEN}${BOLD}All %d mutants killed or rejected.${OFF}\n" "$total"
  exit 0
fi
[ "$survived" -gt 0 ] && printf \
  "${RED}${BOLD}%d of %d survived${OFF} — blind spots in tests/test.cpp\n" "$survived" "$total"
[ "$broken" -gt 0 ] && printf \
  "${RED}${BOLD}%d mutant(s) mis-built${OFF} — harness problem, not a result\n" "$broken"
exit 1
