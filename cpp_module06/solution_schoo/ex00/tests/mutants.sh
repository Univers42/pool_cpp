#!/bin/sh
# Mutation test for ex00 ScalarConverter.
#
# A passing test suite proves nothing until you show it can fail. Each row
# injects one realistic defect into a COPY of ScalarConverter.cpp and requires
# tests/test.cpp to catch it. A survivor is a hole in the tests.
#
#   ./tests/mutants.sh        (from ex00/, or from tests/ — either works)

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
SRC="ScalarConverter.cpp"     # the file under mutation
DEPS=""                       # other sources the tests need
TEST="tests/test.cpp"
OUT="build/mutants"

BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"; GREY="\033[90m"

mkdir -p "$OUT"
total=0; survived=0; broken=0; moved=0

build_mutant() {
  sed "$1" "$SRC" > "$OUT/$SRC" 2>"$OUT/sed.log"
  if [ -s "$OUT/sed.log" ]; then return 3; fi
  cmp -s "$OUT/$SRC" "$SRC" && return 2
  $CXX $STD -w -I. $TEST "$OUT/$SRC" $DEPS -o "$OUT/t" 2>"$OUT/build.log"
}

# mutate <name> <sed-expr> <what defect this simulates>
mutate() {
  total=$((total + 1))
  build_mutant "$2"
  case $? in
    2) printf "  ${RED}%-9s${OFF} %-22s %s\n" "PATTERN" "$1" "sed matched nothing"
       broken=$((broken + 1)); return ;;
    3) printf "  ${RED}%-9s${OFF} %-22s %s\n" "SEDERR" "$1" "$(cat "$OUT/sed.log")"
       broken=$((broken + 1)); return ;;
    0) ;;
    *) printf "  ${RED}%-9s${OFF} %-22s %s\n" "NOBUILD" "$1" "expected to compile"
       head -2 "$OUT/build.log" | sed 's/^/               /'
       broken=$((broken + 1)); return ;;
  esac
  if "./$OUT/t" >"$OUT/run.log" 2>&1; then
    printf "  ${RED}%-9s${OFF} %-22s %s\n" "SURVIVED" "$1" "$3"
    survived=$((survived + 1))
  else
    printf "  ${GREEN}%-9s${OFF} %-22s ${GREY}%s${OFF}\n" "killed" "$1" "$3"
  fi
}

# Edits that provably do NOT change behaviour, so no test can kill them.
# Listed because one becoming killable means the semantics moved underneath.
equivalent() {
  total=$((total + 1))
  build_mutant "$2"
  case $? in
    2) printf "  ${RED}%-9s${OFF} %-22s %s\n" "PATTERN" "$1" \
         "sed matched nothing — pattern is stale, not a semantic change"
       broken=$((broken + 1)); return ;;
    0) ;;
    *) printf "  ${RED}%-9s${OFF} %-22s %s\n" "NOBUILD" "$1" "expected to compile"
       broken=$((broken + 1)); return ;;
  esac
  if "./$OUT/t" >/dev/null 2>&1; then
    printf "  ${GREY}%-9s${OFF} %-22s ${GREY}%s${OFF}\n" "survived" "$1" "$3"
  else
    printf "  ${RED}%-9s${OFF} %-22s %s\n" "CHANGED" "$1" "was equivalent, now killable"
    moved=$((moved + 1))
  fi
}

printf "${BOLD}ex00 mutation test${OFF}  — 'killed' means tests/test.cpp caught the bug\n\n"

mutate "char-upper-bound" \
  's/inRange(v, 0\.0, 127\.0)/inRange(v, 0.0, 126.0)/' \
  "127 wrongly impossible"

mutate "char-lower-bound" \
  's/inRange(v, 0\.0, 127\.0)/inRange(v, 1.0, 127.0)/' \
  "0 wrongly impossible"

mutate "pointzero-ceiling" \
  's/std::fabs(v) < 1e6/std::fabs(v) <= 1e6/' \
  "1e6 wrongly gets .0"

mutate "pointzero-always" \
  's/v == std::floor(v) \&\&/true \&\&/' \
  "4.2 wrongly gets .0"

mutate "range-exclusive" \
  's/v >= lo \&\& v <= hi/v > lo \&\& v < hi/' \
  "INT_MAX wrongly impossible"

mutate "strtod-to-atoi" \
  's/std::strtod(literal\.c_str(), NULL)/static_cast<double>(std::atoi(literal.c_str()))/' \
  "the original atoi bug this exercise was built around"

mutate "quoted-form-broken" \
  "s/s.length() == 3 \&\& s\[0\] == '\\\\''/s.length() == 4 \&\& s[0] == '\\\\''/" \
  "'a' wrongly rejected"

mutate "accept-two-dots" \
  "s/s\[i\] == '\\.' \&\& !dot/s[i] == '.'/" \
  "4.2.3 wrongly accepted"

mutate "accept-no-digits" \
  's/return (digit)/return (true)/' \
  "\".\" and \"-\" wrongly accepted"

mutate "suffix-any-char" \
  "s/s\[end - 1\] == 'f'/s[end - 1] != 0/" \
  "42x wrongly accepted"

mutate "sign-not-skipped" \
  's/? 1 : 0;/? 0 : 0;/' \
  "+42 wrongly rejected"

mutate "isprint-dropped" \
  's/!std::isprint(static_cast<int>(v))/false/' \
  "control characters printed raw"

mutate "int-rounds" \
  's/static_cast<int>(v) << std::endl/static_cast<int>(v + 0.5) << std::endl/' \
  "int rounds instead of truncating toward zero"

mutate "float-not-narrowed" \
  's/float f = static_cast<float>(v);/float f = static_cast<float>(v * 1.0000001);/' \
  "float precision drift"

mutate "pseudo-whitelist" \
  's/s == "nan" ||/s == "NAN" ||/' \
  "lowercase nan rejected"

printf "\n${BOLD}known-equivalent${OFF} (provably no behaviour change — unkillable)\n"

equivalent "floor-to-ceil" \
  's/v == std::floor(v) \&\&/v == std::ceil(v) \&\&/' \
  "v==floor(v) and v==ceil(v) both mean 'integral'"

equivalent "isnan-redundant" \
  's/!std::isnan(v) \&\&/true \&\&/' \
  "NaN already fails >= and <=, so the guard is belt-and-braces"

printf "\n"
if [ "$survived" -eq 0 ] && [ "$broken" -eq 0 ] && [ "$moved" -eq 0 ]; then
  printf "${GREEN}${BOLD}All %d mutants killed.${OFF}\n" "$((total - 2))"
  exit 0
fi
[ "$survived" -gt 0 ] && printf \
  "${RED}${BOLD}%d survived${OFF} — blind spots in tests/test.cpp\n" "$survived"
[ "$broken" -gt 0 ] && printf \
  "${RED}${BOLD}%d mis-built${OFF} — harness problem, not a result\n" "$broken"
[ "$moved" -gt 0 ] && printf \
  "${RED}${BOLD}%d known-equivalent mutant(s) became killable${OFF} — semantics moved\n" "$moved"
exit 1
