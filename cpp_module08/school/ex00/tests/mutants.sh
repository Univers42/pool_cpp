#!/bin/sh
# Breaks easyfind.hpp on purpose, one bug at a time, and checks tests/test.cpp
# notices. A suite that has never been shown to fail is a suite nobody has
# tested; this is the part that tests the tests.
#
# The header has no .cpp to swap at link time, so the exercise is copied into
# build/mutants/ with the broken header in place and the test rebuilt there.
#
#   ./tests/mutants.sh      (from ex00/ or from tests/)
#   VG=1 ./tests/mutants.sh runs each mutant under valgrind
#
# Two defects deliberately have no mutant here:
#   * replacing std::find with a correct hand-written loop. It is an EQUIVALENT
#     MUTANT — identical behaviour, so no runtime assertion could ever kill it
#     — yet the evaluation sheet counts it as wrong. It is caught by grep, in
#     tests/eval.sh, which is the only tool that can see it.
#   * dropping #include <algorithm>. Whether that still compiles depends on
#     what the *test* happens to include first, so the answer would say more
#     about test.cpp than about the header. eval.sh compiles the header on its
#     own instead, which asks the question properly.

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
SRC="easyfind.hpp"
TEST="tests/test.cpp"
OUT="build/mutants"
RUNNER=""
[ -n "$VG" ] && RUNNER="valgrind -q --error-exitcode=9 --leak-check=full"

# Colours only when a human is watching: piping this into a file or a chat
# should not paste a wall of escape codes.
if [ -t 1 ] && [ -z "$NO_COLOR" ]; then
  BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"
  GREY="\033[90m"; YELLOW="\033[33m"; CYAN="\033[36m"
else
  BOLD=""; OFF=""; GREEN=""; RED=""; GREY=""; YELLOW=""; CYAN=""
fi

mkdir -p "$OUT/tests"
cp "$TEST" "$OUT/tests/test.cpp"
cp tests/check.hpp "$OUT/tests/check.hpp"
total=0; survived=0; broken=0

# mutate <name> <sed-expr> <what defect this simulates>
mutate() {
  total=$((total + 1))
  sed "$2" "$SRC" > "$OUT/$SRC" 2>/dev/null

  if cmp -s "$OUT/$SRC" "$SRC"; then
    printf "  ${RED}FAIL${OFF} %-9s %-24s %s\n" "PATTERN" "$1" "sed matched nothing"
    broken=$((broken + 1)); return
  fi

  if ! $CXX $STD -w "$OUT/tests/test.cpp" -o "$OUT/t" 2>/dev/null; then
    printf "  ${GREEN}ok${OFF}   %-9s %-24s ${GREY}%s${OFF}\n" "killed(c)" "$1" "$3"
    return
  fi

  # A mutant may also abort or segfault; any non-zero exit is a kill.
  if $RUNNER "./$OUT/t" >"$OUT/run.log" 2>&1; then
    printf "  ${RED}FAIL${OFF} %-9s %-24s %s\n" "SURVIVED" "$1" "$3"
    survived=$((survived + 1))
  else
    printf "  ${GREEN}ok${OFF}   %-9s %-24s ${GREY}%s${OFF}\n" "killed" "$1" "$3"
  fi
}

printf "${BOLD}ex00 mutation test${OFF}  14 injected bugs, all of which must be caught\n"
printf "${GREY}  every line should say ok. 'killed' means a bug was injected and the\n  tests caught it, which is the result being tested for.${OFF}\n\n"


# ── the miss: the whole reason this is not just a call to std::find ───────────
mutate "no-miss-check" \
  's|if (it == container.end())||g' \
  "neither overload checks for a miss: end() is handed back as if it were a hit"

mutate "no-throw-mutable" \
  '/typename T::iterator it =/,/return (it);/{s|throw std::out_of_range("easyfind: value not found");||;}' \
  "only the MUTABLE overload stops throwing - a const-only suite misses this"

mutate "no-throw-const" \
  '/typename T::const_iterator it =/,/return (it);/{s|throw std::out_of_range("easyfind: value not found");||;}' \
  "only the CONST overload stops throwing - the mirror blind spot"

mutate "inverted-check" \
  's|if (it == container.end())|if (it != container.end())|g' \
  "the condition is flipped: hits throw and misses return end()"

mutate "empty-shortcut" \
  's|typename T::iterator it =|if (container.empty()) return container.end(); typename T::iterator it =|' \
  "an empty container quietly returns end() instead of throwing"

mutate "wrong-exception-type" \
  's|throw std::out_of_range|throw std::runtime_error|g' \
  "runtime_error instead of out_of_range: catch clauses in real code stop matching"

mutate "wrong-message" \
  's|"easyfind: value not found"|"error"|g' \
  "what() no longer says which function failed or why"


# ── the search itself ────────────────────────────────────────────────────────
mutate "needle-off-by-one" \
  's|std::find(container.begin(), container.end(), value)|std::find(container.begin(), container.end(), value + 1)|g' \
  "searches for value + 1: right shape, wrong answer everywhere"

mutate "skip-first-element" \
  's|std::find(container.begin(), container.end(), value)|std::find(++container.begin(), container.end(), value)|g' \
  "the search starts at index 1, so a hit at index 0 becomes a miss"

mutate "half-range" \
  's|std::find(container.begin(), container.end(), value)|std::find(container.begin(), container.begin(), value)|g' \
  "an empty search range: every lookup misses"


# ── what is returned ─────────────────────────────────────────────────────────
mutate "return-next" \
  's|return (it);|return (++it);|g' \
  "off by one: the iterator points just past the element that matched"

mutate "return-begin" \
  's|return (it);|return (container.begin());|g' \
  "the position is thrown away and begin() returned - correct only for index 0"

mutate "pass-by-value" \
  's|typename T::iterator easyfind(T& container, int value)|typename T::iterator easyfind(T container, int value)|' \
  "the container is copied, so the returned iterator dangles into a dead temporary"

mutate "const-returns-mutable" \
  's|typename T::const_iterator easyfind(const T& container, int value)|typename T::iterator easyfind(const T\& container, int value)|' \
  "the const overload promises a mutable iterator it cannot produce"


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
