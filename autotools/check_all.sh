#!/bin/sh
# Whole-module verification: build everything from clean, run every test, then
# go looking for the failures a passing test suite cannot see.
#
#   ./autotools/check_all.sh                    cpp_module08 (default)
#   ./autotools/check_all.sh cpp_module07        another module
#   ./autotools/check_all.sh cpp_module07 cpp_module08
#   SEEDS="1 2 3" ./autotools/check_all.sh       more fuzz seeds
#   QUICK=1 ./autotools/check_all.sh             skip sanitizers and valgrind
#
# For each exercise directory it runs six passes:
#
#   1. clean build      make fclean && make, with the exercise's own flags
#   2. evaluator build  c++ -std=c++98 -Wall -Wextra -Werror straight at *.cpp,
#                       plus -pedantic-errors, because a Makefile can lie
#   3. make test        the assertions, the evaluation checklist, the mutation
#                       run and the compile-failure cases
#   4. valgrind         every binary, leaks AND errors, --error-exitcode=42
#   5. sanitizers       a rebuild under -fsanitize=address,undefined, which is
#                       what actually catches an out-of-bounds read on the
#                       stack or in a global - valgrind only sees the heap
#   6. fuzz seeds       the randomised suites re-run on other seeds, since a
#                       fuzzer that only ever runs seed 42 is a fixed test
#
# Anything non-zero anywhere fails the whole run.

cd "$(dirname "$0")/.." || exit 1
ROOT=$(pwd)

CXX="${CXX:-c++}"
SEEDS="${SEEDS:-7 1234 20260815}"

# Colours only when a human is watching: piping this into a file or a chat
# should not paste a wall of escape codes.
if [ -t 1 ] && [ -z "$NO_COLOR" ]; then
  BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"
  GREY="\033[90m"; YELLOW="\033[33m"; CYAN="\033[36m"
else
  BOLD=""; OFF=""; GREEN=""; RED=""; GREY=""; YELLOW=""; CYAN=""
fi

total=0; failed=0
FAILURES=""

pass() {
  total=$((total + 1))
  printf "    ${GREEN}%-9s${OFF} %-22s ${GREY}%s${OFF}\n" "pass" "$1" "$2"
}

fail() {
  total=$((total + 1)); failed=$((failed + 1))
  FAILURES="$FAILURES\n  $1 ($2)"
  printf "    ${RED}%-9s${OFF} %-22s %s\n" "FAIL" "$1" "$2"
  [ -n "$3" ] && printf "%s\n" "$3" | head -15 | sed 's/^/               /'
}

skip() {
  printf "    ${YELLOW}%-9s${OFF} %-22s ${GREY}%s${OFF}\n" "skip" "$1" "$2"
}

have() { command -v "$1" >/dev/null 2>&1; }

# ── one exercise ─────────────────────────────────────────────────────────────
check_exercise() {
  dir="$1"
  name=$(basename "$dir")
  module=$(basename "$(dirname "$dir")")
  printf "\n  ${BOLD}%s/%s${OFF}\n" "$module" "$name"

  cd "$ROOT/$dir" || return

  # 1. clean build through the exercise's own Makefile
  log=$(make fclean >/dev/null 2>&1; make 2>&1)
  if [ $? -eq 0 ]; then
    pass "build" "make from clean"
  else
    fail "build" "make from clean" "$log"
    cd "$ROOT" || return
    return
  fi

  # 2. the evaluator's command line, not the Makefile's. -pedantic-errors is
  #    the check that actually enforces "C++98 only".
  log=$($CXX -std=c++98 -Wall -Wextra -Werror -pedantic-errors -I. \
        ./*.cpp -o build/bin/.direct 2>&1)
  if [ $? -eq 0 ]; then
    pass "c++98-strict" "-Wall -Wextra -Werror -pedantic-errors"
  else
    fail "c++98-strict" "the evaluator's own command line" "$log"
  fi
  rm -f build/bin/.direct

  # 3. the suite, the checklist, the mutants and the compile-failure cases
  log=$(make test 2>&1)
  if [ $? -eq 0 ]; then
    counts=$(printf "%s" "$log" | grep -oE "[0-9]+ checks, all passed" \
             | grep -oE "^[0-9]+" | paste -sd+ - | bc 2>/dev/null)
    muts=$(printf "%s" "$log" | grep -oE "All [0-9]+ mutants killed" \
           | grep -oE "[0-9]+")
    pass "make test" "${counts:-0} assertions, ${muts:-0} mutants killed"
  else
    fail "make test" "suite, checklist, mutants or compile cases" "$log"
  fi

  # 4. valgrind: leaks and errors, on every binary the exercise produced
  if [ -n "$QUICK" ]; then
    skip "valgrind" "QUICK=1"
  elif ! have valgrind; then
    skip "valgrind" "not installed"
  else
    dirty=""
    for binary in build/bin/*; do
      [ -f "$binary" ] && [ -x "$binary" ] || continue
      case "$binary" in *.direct) continue ;; esac
      if ! valgrind -q --leak-check=full --show-leak-kinds=all \
           --errors-for-leak-kinds=all --error-exitcode=42 \
           "./$binary" >/dev/null 2>"$ROOT/.vg.log"; then
        dirty="$dirty $(basename "$binary")"
      fi
    done
    if [ -z "$dirty" ]; then
      pass "valgrind" "0 leaks, 0 errors, every binary"
    else
      fail "valgrind" "leaks or errors in:$dirty" "$(cat "$ROOT/.vg.log")"
    fi
    rm -f "$ROOT/.vg.log"
  fi

  # 5. ASan + UBSan. Valgrind sees the heap; the sanitizers also see stack and
  #    global overflows, and UBSan catches signed overflow and bad shifts,
  #    which is exactly the class of bug the unsigned-distance trick avoids.
  if [ -n "$QUICK" ]; then
    skip "sanitizers" "QUICK=1"
  else
    sanflags="-fsanitize=address,undefined -fno-omit-frame-pointer -g"
    dirty=""
    sanlog=""
    for source in tests/*.cpp; do
      [ -f "$source" ] || continue
      out="build/bin/.san_$(basename "$source" .cpp)"
      others=$(ls ./*.cpp 2>/dev/null | grep -v '/main\.cpp$' | tr '\n' ' ')
      if ! $CXX -std=c++98 $sanflags -I. $others "$source" -o "$out" \
           2>"$ROOT/.san.log"; then
        dirty="$dirty $(basename "$source")(build)"
        sanlog=$(cat "$ROOT/.san.log")
        continue
      fi
      if ! ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 \
           "./$out" >/dev/null 2>"$ROOT/.san.log"; then
        dirty="$dirty $(basename "$source")"
        sanlog=$(cat "$ROOT/.san.log")
      fi
      rm -f "$out"
    done
    # And the exercise's own main.
    if [ -f main.cpp ]; then
      if $CXX -std=c++98 $sanflags -I. ./*.cpp -o build/bin/.san_main \
           2>"$ROOT/.san.log"; then
        if ! ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 \
             ./build/bin/.san_main >/dev/null 2>"$ROOT/.san.log"; then
          dirty="$dirty main.cpp"
          sanlog=$(cat "$ROOT/.san.log")
        fi
        rm -f build/bin/.san_main
      else
        dirty="$dirty main.cpp(build)"
        sanlog=$(cat "$ROOT/.san.log")
      fi
    fi
    if [ -z "$dirty" ]; then
      pass "sanitizers" "address+undefined clean, bounds included"
    else
      fail "sanitizers" "flagged:$dirty" "$sanlog"
    fi
    rm -f "$ROOT/.san.log"
  fi

  # 6. the randomised suites on seeds other than the built-in one
  if [ -x build/bin/fuzz ]; then
    bad=""
    for seed in $SEEDS; do
      ./build/bin/fuzz "$seed" >/dev/null 2>&1 || bad="$bad $seed"
    done
    if [ -z "$bad" ]; then
      pass "fuzz seeds" "$(echo $SEEDS | wc -w) extra seeds, all clean"
    else
      fail "fuzz seeds" "failed on seed(s):$bad" \
        "$(./build/bin/fuzz ${bad%% *} 2>&1 | tail -12)"
    fi
  else
    skip "fuzz seeds" "no fuzz binary"
  fi

  cd "$ROOT" || return
}

# ── the lab, which plays by different rules ──────────────────────────────────
check_lab() {
  module="$1"
  [ -f "$ROOT/$module/all.cpp" ] || return
  printf "\n  ${BOLD}%s/all.cpp${OFF}  ${GREY}(lab, C++17)${OFF}\n" "$module"
  cd "$ROOT/$module" || return

  log=$(make -s re 2>&1)
  if [ $? -eq 0 ]; then
    pass "build" "c++17 -Wall -Wextra -Werror"
  else
    fail "build" "lab does not compile" "$log"
    cd "$ROOT" || return
    return
  fi

  log=$(./build/bin/lab 2>&1)
  if [ $? -eq 0 ]; then
    pass "assertions" "every assert in the lab held"
  else
    fail "assertions" "the lab aborted" "$(printf '%s' "$log" | tail -12)"
  fi

  if [ -n "$QUICK" ]; then
    skip "valgrind" "QUICK=1"
  elif have valgrind; then
    if valgrind -q --leak-check=full --show-leak-kinds=all \
       --error-exitcode=42 ./build/bin/lab >/dev/null 2>"$ROOT/.vg.log"; then
      pass "valgrind" "0 leaks, 0 errors"
    else
      fail "valgrind" "leaks or errors" "$(cat "$ROOT/.vg.log")"
    fi
    rm -f "$ROOT/.vg.log"
  else
    skip "valgrind" "not installed"
  fi

  if [ -n "$QUICK" ]; then
    skip "sanitizers" "QUICK=1"
  else
    if $CXX -std=c++17 -fsanitize=address,undefined -fno-omit-frame-pointer \
         -g all.cpp -o build/bin/.san 2>"$ROOT/.san.log" && \
       ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 \
         ./build/bin/.san >/dev/null 2>"$ROOT/.san.log"; then
      pass "sanitizers" "address+undefined clean"
    else
      fail "sanitizers" "flagged the lab" "$(cat "$ROOT/.san.log")"
    fi
    rm -f build/bin/.san "$ROOT/.san.log"
  fi

  cd "$ROOT" || return
}

# ── the generator ────────────────────────────────────────────────────────────
check_generator() {
  printf "\n  ${BOLD}autotools/gen_class.py${OFF}\n"
  log=$(python3 "$ROOT/autotools/gen_class.py" --self-test 2>&1)
  if [ $? -eq 0 ]; then
    n=$(printf "%s" "$log" | grep -oE "All [0-9]+ checks passed" \
        | grep -oE "[0-9]+")
    pass "self-test" "${n:-0} checks: every kind compiles and links"
  else
    fail "self-test" "generated code is broken" \
      "$(printf '%s' "$log" | grep -A3 FAIL | head -12)"
  fi
}

# ── main ─────────────────────────────────────────────────────────────────────
MODULES="$*"
[ -z "$MODULES" ] && MODULES="cpp_module08"

printf "${BOLD}${CYAN}whole-module verification${OFF}  ${GREY}%s${OFF}\n" \
  "$MODULES"
[ -n "$QUICK" ] && printf "${YELLOW}  QUICK=1: valgrind and sanitizers skipped${OFF}\n"

for module in $MODULES; do
  if [ ! -d "$ROOT/$module" ]; then
    fail "$module" "no such directory"
    continue
  fi
  for exercise in "$ROOT/$module"/ex*; do
    [ -d "$exercise" ] && [ -f "$exercise/Makefile" ] || continue
    check_exercise "${exercise#$ROOT/}"
  done
  check_lab "$module"
done

check_generator

printf "\n"
if [ "$failed" -eq 0 ]; then
  printf "${GREEN}${BOLD}All %d checks passed.${OFF}\n" "$total"
  exit 0
fi
printf "${RED}${BOLD}%d of %d checks failed:${OFF}%b\n" "$failed" "$total" \
  "$FAILURES"
exit 1
