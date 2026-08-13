#!/bin/sh
# Casts lab runner -- cpp_module06/casts_lab
#
#   ./run.sh            build and run all three labs
#   ./run.sh ex00       just one lab (ex00 | ex01 | ex02)
#   ./run.sh ubsan      rerun under -fsanitize=undefined (the A/B that matters)
#   ./run.sh negative   compile the must-not-compile files and show the errors
#   ./run.sh opt        ex01's type punning at -O0 vs -O2
#   ./run.sh clean
#
# Nothing here touches the graded exNN/ directories.

set -e

CXX="${CXX:-c++}"
STD="-std=c++98"
WARN="-Wall -Wextra"          # deliberately NOT -Werror: this code misbehaves on purpose
OUT="build"

B="\033[1m"; R="\033[0m"; GREY="\033[90m"

banner() { printf "\n${B}%s${R}\n%s\n" "$1" "$(printf '=%.0s' $(seq 1 72))"; }

build_one() {
  mkdir -p "$OUT"
  printf "${GREY}  %s %s %s %s -o %s${R}\n" "$CXX" "$STD" "$WARN" "$1" "$OUT/$2"
  $CXX $STD $WARN "$1" -o "$OUT/$2"
}

run_all() {
  for ex in ex00_static_cast ex01_reinterpret_cast ex02_dynamic_cast; do
    banner "BUILD $ex"
    build_one "$ex.cpp" "$ex"
    "./$OUT/$ex"
  done
}

run_one() {
  src=$(ls "$1"_*.cpp 2>/dev/null | head -1)
  [ -n "$src" ] || { echo "no lab matching '$1' (try ex00, ex01, ex02)"; exit 1; }
  bin=$(basename "$src" .cpp)
  banner "BUILD $bin"
  build_one "$src" "$bin"
  "./$OUT/$bin"
}

run_ubsan() {
  banner "DEBUGGING: which tool catches which bug"
  cat <<'EOF'
Every bug in this lab compiles, runs, and exits 0. That is the whole problem.
This pass runs the same sources under three different instruments and shows
exactly where each one succeeds and where it goes blind. "It ran fine" is not
evidence of correctness -- it is evidence you did not ask.
EOF
  mkdir -p "$OUT"

  printf "\n${B}-- 1) plain build: ex00 scenario 2 looks completely normal --${R}\n"
  $CXX $STD $WARN ex00_static_cast.cpp -o "$OUT/ex00_plain" 2>/dev/null
  "./$OUT/ex00_plain" 2>&1 | sed -n '/A) unguarded/,/UNSAFE/p' | sed 's/^/  /'
  printf "  exit status: %s   <- a clean bill of health for undefined behaviour\n" "$?"

  printf "\n${B}-- 2) same source, -fsanitize=undefined --${R}\n"
  $CXX $STD $WARN -g -fsanitize=undefined ex00_static_cast.cpp -o "$OUT/ex00_ubsan"
  "./$OUT/ex00_ubsan" 2>&1 | grep -E "^ex00_static_cast\.cpp:[0-9]+.*runtime error" \
    | sed 's/^/  /' || echo "  (no UBSan diagnostic -- check your clang build)"
  cat <<'EOF'

  Two different bugs, both named with a file:line:
    scenario 2 -> the out-of-range double->int conversion
    scenario 3 -> "downcast of address ... which does not point to an object
                   of type 'Circle'". UBSan's vptr check reads the same type
                   info dynamic_cast would have read. It is dynamic_cast's
                   verdict, delivered after the fact, in a debug build only.
EOF

  printf "\n${B}-- 3) valgrind on the reinterpret_cast lab --${R}\n"
  if command -v valgrind >/dev/null 2>&1; then
    $CXX $STD $WARN -g ex01_reinterpret_cast.cpp -o "$OUT/ex01_vg" 2>/dev/null
    valgrind -q "./$OUT/ex01_vg" >/dev/null 2>"$OUT/vg.log" || true
    echo "  lines valgrind flagged:"
    grep -oE "ex01_reinterpret_cast\.cpp:[0-9]+" "$OUT/vg.log" | sort -u | sed 's/^/    /'
    cat <<'EOF'

  Those are scenario 2 -- reading Data's bytes as a Telemetry gives an
  uninitialised read, and valgrind sees it.

  Now notice what is NOT in that list: scenario 3. The multiple-inheritance
  offset bug does not appear at all. Not one line. And it is the most
  destructive bug in this lab -- it silently WRITES to the wrong member.
  Valgrind cannot see it because there is nothing to see: every read and
  write is perfectly in-bounds of a live, initialised Car object. The
  pointer just designates the wrong half of it. That is a TYPE error, and
  valgrind does not track types.
EOF
  else
    echo "  valgrind not installed, skipping"
  fi

  banner "COVERAGE -- what actually catches each bug"
  cat <<'EOF'
  bug                                    -Wall  UBSan  valgrind  dynamic_cast
  ---------------------------------------------------------------------------
  ex00 #2  double->int out of range        no    YES     no        n/a
  ex00 #3  static_cast to wrong type       no    YES     no        YES
  ex01 #2  round trip to wrong type        no     no    YES        n/a
  ex01 #3  MI offset, direct cast         YES     no     no        n/a
  ex01 #3D MI offset via uintptr_t          no     no     no        n/a   <--
  ex01 #4  strict aliasing               partial partial no        n/a

  Read the row marked <--. Launder the pointer through uintptr_t -- which is
  precisely the ex01 Serializer pattern -- and NOTHING catches it. No warning,
  no sanitizer, no valgrind. That row is the reason this module exists: the
  named casts are a discipline you apply by reading, because past a certain
  point the tools stop being able to help you.

  Practical order of attack when a cast bug is suspected:
    1. -Wall -Wextra, and actually read the warnings
    2. -fsanitize=undefined  (catches bad downcasts AND numeric UB)
    3. valgrind              (catches wrong-type reads and OOB)
    4. print the pointers    (printf %p before and after -- an offset that
                              changes when it should not, or does not change
                              when it should, is the MI bug on sight)
    5. reread every cast     (the only thing that finds row 5)
EOF
}

run_negative() {
  banner "NEGATIVE TESTS -- these MUST fail to compile"
  echo "Each error below is the compiler doing its job. Read them; the day you"
  echo "hit one for real, the fix is almost never 'use a blunter cast'."
  for f in negative/*.cpp; do
    printf "\n${B}-- %s --${R}\n" "$f"
    sed -n '3,12p' "$f" | sed 's/^\/\/ \?//'
    printf "${GREY}  \$ %s %s %s${R}\n" "$CXX" "$STD" "$f"
    if $CXX $STD -fsyntax-only "$f" 2>"$OUT/err.log"; then
      printf "  \033[31mUNEXPECTED: this compiled. It should not have.\033[0m\n"
    else
      sed -n '1,6p' "$OUT/err.log" | sed 's/^/  /'
      printf "  \033[32m-> rejected at compile time, as intended\033[0m\n"
    fi
  done
}

run_opt() {
  banner "OPTIMISER A/B -- strict aliasing (ex01 scenario 4)"
  echo "Same source, two optimisation levels. If the outputs differ, the code"
  echo "has undefined behaviour and the -O0 run was luck, not correctness."
  mkdir -p "$OUT"
  for lvl in O0 O2; do
    $CXX $STD $WARN "-$lvl" -Wstrict-aliasing ex01_reinterpret_cast.cpp \
      -o "$OUT/ex01_$lvl" 2>/dev/null
    printf "\n${B}-- -%s --${R}\n" "$lvl"
    "./$OUT/ex01_$lvl" | sed -n '/4. Type punning/,/optimiser may assume/p' \
      | grep -E "reinterpret_cast<float\*>|int i =" || true
  done
  echo ""
  echo "On this compiler the two agree -- clang happens to generate the load"
  echo "you expected. That agreement is NOT a guarantee: it is one compiler,"
  echo "one version, one flag set. The code is still UB, and std::memcpy is"
  echo "the fix that is actually portable."
}

mkdir -p "$OUT"
case "${1:-all}" in
  all)      run_all ;;
  ex00|ex01|ex02) run_one "$1" ;;
  ubsan)    run_ubsan ;;
  negative) run_negative ;;
  opt)      run_opt ;;
  clean)    rm -rf "$OUT"; echo "cleaned" ;;
  *)        echo "usage: ./run.sh [all|ex00|ex01|ex02|ubsan|negative|opt|clean]"; exit 1 ;;
esac
