#!/bin/sh
# Breaks whatever.hpp on purpose, one bug at a time, and checks that
# tests/test.cpp notices. A suite that always passes is not worth much.
#
# The header has no .cpp to swap at link time, so the exercise is copied into
# build/mutants/ with the broken header in place and the test built there.
#
#   ./tests/mutants.sh      (from ex00/ or from tests/)

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
SRC="whatever.hpp"            # the file under mutation
TEST="tests/test.cpp"
OUT="build/mutants"

BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"; GREY="\033[90m"

# Start from an empty scratch dir. A leftover copy from a previous run can
# satisfy an include that the harness no longer provides, which hides a
# broken harness behind a clean sweep.
rm -rf "$OUT"
mkdir -p "$OUT/tests"
cp "$TEST" "$OUT/tests/test.cpp"
total=0; survived=0; broken=0

# The unmutated exercise must build AND pass before any mutant is judged.
# A compile failure counts as a kill below, so anything that stops the suite
# building at all - a missing include, a header that did not get copied here -
# would otherwise read as a clean sweep while testing nothing.
cp "$SRC" "$OUT/$SRC"
if ! $CXX $STD -w "$OUT/tests/test.cpp" -o "$OUT/baseline" 2>"$OUT/baseline.log"; then
  printf "${RED}${BOLD}baseline does not compile${OFF} - the harness is broken, not $SRC\n"
  sed 's/^/  /' "$OUT/baseline.log"
  exit 1
fi
if ! "./$OUT/baseline" >"$OUT/baseline.run" 2>&1; then
  printf "${RED}${BOLD}baseline does not pass${OFF} - fix tests/test.cpp before mutating\n"
  exit 1
fi

# mutate <name> <sed-expr> <what defect this simulates>
#
# Three outcomes count as a kill:
#   killed      the suite ran and failed
#   killed(c)   the suite refused to compile against the mutant, which is
#               still the tests rejecting it (e.g. returning by value breaks
#               the &::min(x, y) identity checks)
mutate() {
  total=$((total + 1))
  sed "$2" "$SRC" > "$OUT/$SRC" 2>/dev/null

  if cmp -s "$OUT/$SRC" "$SRC"; then
    printf "  ${RED}%-9s${OFF} %-24s %s\n" "PATTERN" "$1" "sed matched nothing"
    broken=$((broken + 1)); return
  fi

  if ! $CXX $STD -w "$OUT/tests/test.cpp" -o "$OUT/t" 2>/dev/null; then
    printf "  ${GREEN}%-9s${OFF} %-24s ${GREY}%s${OFF}\n" "killed(c)" "$1" "$3"
    return
  fi

  if "./$OUT/t" >"$OUT/run.log" 2>&1; then
    printf "  ${RED}%-9s${OFF} %-24s %s\n" "SURVIVED" "$1" "$3"
    survived=$((survived + 1))
  else
    printf "  ${GREEN}%-9s${OFF} %-24s ${GREY}%s${OFF}\n" "killed" "$1" "$3"
  fi
}

printf "${BOLD}ex00 mutation test${OFF}  - 'killed' means tests/test.cpp caught the bug\n\n"


mutate "swap-noop" \
  's|^  T temp = a;|  return; T temp = a;|' \
  "swap does nothing at all"

mutate "swap-lost-temp" \
  's|^  b = temp;|  b = a;|' \
  "no temporary: both variables end up holding b"

mutate "swap-half-done" \
  's|^  a = b;|  ;|' \
  "swap writes b but never writes a"

mutate "swap-extra-copy" \
  's|^  a = b;|  a = temp;|' \
  "swap assigns the temporary back onto a"


# Written with the operands flipped rather than as (a <= b), so the mutant
# needs no operator the real header does not need. Otherwise it would die at
# compile time on the test's types and never reach the tie assertion - a kill
# that proves nothing about the suite.
mutate "min-tie-first" \
  's|return ((a < b) ? a : b);|return ((b < a) ? b : a);|' \
  "correct min, except a tie returns the FIRST argument - value tests are blind to this"

mutate "min-is-max" \
  's|return ((a < b) ? a : b);|return ((a > b) ? a : b);|' \
  "min and max swapped"

mutate "min-always-first" \
  's|return ((a < b) ? a : b);|return (a);|' \
  "min ignores the comparison"

mutate "min-by-value" \
  's|^const T& min(const T& a, const T& b) {|T min(const T\& a, const T\& b) {|' \
  "min returns a copy, so the reference identity guarantee is gone"

mutate "min-copies-args" \
  's|^const T& min(const T& a, const T& b) {|const T\& min(T a, T b) {|' \
  "min takes its arguments by value - a dangling reference and 2 copies"


mutate "max-tie-first" \
  's|return ((a > b) ? a : b);|return ((b > a) ? b : a);|' \
  "correct max, except a tie returns the FIRST argument"

mutate "max-is-min" \
  's|return ((a > b) ? a : b);|return ((a < b) ? a : b);|' \
  "max behaves like min"

mutate "max-always-second" \
  's|return ((a > b) ? a : b);|return (b);|' \
  "max ignores the comparison"

mutate "max-by-value" \
  's|^const T& max(const T& a, const T& b) {|T max(const T\& a, const T\& b) {|' \
  "max returns a copy"

printf "\n"
if [ "$survived" -eq 0 ] && [ "$broken" -eq 0 ]; then
  printf "${GREEN}${BOLD}All %d mutants killed.${OFF}\n" "$total"
  exit 0
fi
[ "$survived" -gt 0 ] && printf \
  "${RED}${BOLD}%d of %d survived${OFF} - blind spots in tests/test.cpp\n" "$survived" "$total"
[ "$broken" -gt 0 ] && printf \
  "${RED}${BOLD}%d mutant(s) failed to apply${OFF} - harness problem, not a result\n" "$broken"
exit 1
