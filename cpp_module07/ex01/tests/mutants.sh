#!/bin/sh
# Breaks iter.hpp on purpose, one bug at a time, and checks tests/test.cpp
# notices. The loop is one line, and almost every version below still adds up
# to the right sum, so only the order check and the guard slots catch them.
#
# The header has no .cpp to swap at link time, so the exercise is copied into
# build/mutants/ with the broken header in place and the test built there.
#
#   ./tests/mutants.sh      (from ex01/ or from tests/)

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
SRC="iter.hpp"
TEST="tests/test.cpp"
OUT="build/mutants"

BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"; GREY="\033[90m"

mkdir -p "$OUT/tests"
cp "$TEST" "$OUT/tests/test.cpp"
total=0; survived=0; broken=0

# mutate <name> <sed-expr> <what defect this simulates>
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

  # A mutant that reads out of bounds may segfault or be caught by the
  # assertions; either way a non-zero exit is a kill.
  if "./$OUT/t" >"$OUT/run.log" 2>&1; then
    printf "  ${RED}%-9s${OFF} %-24s %s\n" "SURVIVED" "$1" "$3"
    survived=$((survived + 1))
  else
    printf "  ${GREEN}%-9s${OFF} %-24s ${GREY}%s${OFF}\n" "killed" "$1" "$3"
  fi
}

printf "${BOLD}ex01 mutation test${OFF}  - 'killed' means tests/test.cpp caught the bug\n\n"

LOOP='for (size_t i = 0; i < length; ++i) f(array\[i\]);'

mutate "off-by-one-over" \
  "s|$LOOP|for (size_t i = 0; i <= length; ++i) f(array[i]);|" \
  "reads one element past the end - a sum check often misses this"

mutate "off-by-one-under" \
  "s|$LOOP|for (size_t i = 0; i + 1 < length; ++i) f(array[i]);|" \
  "silently skips the last element"

mutate "skip-first" \
  "s|$LOOP|for (size_t i = 1; i < length; ++i) f(array[i]);|" \
  "starts at index 1"

mutate "backwards" \
  "s|$LOOP|for (size_t i = length; i-- > 0;) f(array[i]);|" \
  "right count, right sum, wrong ORDER"

mutate "every-other" \
  "s|$LOOP|for (size_t i = 0; i < length; i += 2) f(array[i]);|" \
  "visits every second element"

mutate "twice-each" \
  "s|$LOOP|for (size_t i = 0; i < length; ++i) { f(array[i]); f(array[i]); }|" \
  "calls the callback twice per element"

mutate "never-runs" \
  "s|$LOOP|(void)array; (void)f; (void)length;|" \
  "the loop body never executes"

mutate "always-first" \
  "s|$LOOP|for (size_t i = 0; i < length; ++i) f(array[0]);|" \
  "passes element 0 every time - same count, wrong elements"

mutate "fixed-length" \
  "s|$LOOP|for (size_t i = 0; i < 3; ++i) f(array[i]);|" \
  "ignores length and always does three"

mutate "copies-element" \
  "s|$LOOP|for (size_t i = 0; i < length; ++i) { T tmp = array[i]; f(tmp); }|" \
  "passes a copy, so mutations never reach the caller's array"

mutate "byref-functor" \
  's|void iter(T\* array, const size_t length, F f) {|void iter(T* array, const size_t length, F\& f) {|' \
  "takes F by reference - rejects the temporaries every call site passes"

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
