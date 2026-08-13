#!/bin/sh
# Mutation test for ex01 Serializer.
#
# A passing test suite proves nothing until you show it can fail. Each row
# below injects one realistic defect into a COPY of Serializer.cpp and requires
# tests/test.cpp to catch it. A survivor is a hole in the tests.
#
#   ./tests/mutants.sh        (from ex01/, or from tests/ — either works)

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
SRC="Serializer.cpp"          # the file under mutation
DEPS=""                       # other objects the tests need
TEST="tests/test.cpp"
OUT="build/mutants"

BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"; GREY="\033[90m"

mkdir -p "$OUT"
total=0; survived=0; broken=0

# mutate <name> <sed-expr> <what defect this simulates>
mutate() {
  total=$((total + 1))
  sed "$2" "$SRC" > "$OUT/$SRC" 2>/dev/null

  if cmp -s "$OUT/$SRC" "$SRC"; then
    printf "  ${RED}%-9s${OFF} %-22s %s\n" "PATTERN" "$1" "sed matched nothing"
    broken=$((broken + 1)); return
  fi

  if ! $CXX $STD -w -I. $TEST "$OUT/$SRC" $DEPS -o "$OUT/t" 2>/dev/null; then
    printf "  ${RED}%-9s${OFF} %-22s %s\n" "NOBUILD" "$1" "mutant did not compile"
    broken=$((broken + 1)); return
  fi

  if "./$OUT/t" >"$OUT/run.log" 2>&1; then
    printf "  ${RED}%-9s${OFF} %-22s %s\n" "SURVIVED" "$1" "$3"
    survived=$((survived + 1))
  else
    printf "  ${GREEN}%-9s${OFF} %-22s ${GREY}%s${OFF}\n" "killed" "$1" "$3"
  fi
}

printf "${BOLD}ex01 mutation test${OFF}  — 'killed' means tests/test.cpp caught the bug\n\n"

mutate "off-by-one" \
  's|return reinterpret_cast<uintptr_t>(ptr);|return reinterpret_cast<uintptr_t>(ptr) + 1;|' \
  "serialize shifts the address by one byte"

mutate "always-zero" \
  's|return reinterpret_cast<uintptr_t>(ptr);|return 0;|' \
  "serialize throws the pointer away"

mutate "deserialize-null" \
  's|return reinterpret_cast<Data\*>(raw);|return NULL;|' \
  "deserialize always returns NULL"

mutate "deserialize-offset" \
  's|return reinterpret_cast<Data\*>(raw);|return reinterpret_cast<Data*>(raw + 8);|' \
  "deserialize lands 8 bytes into the object"

mutate "truncate-to-32-bit" \
  's|return reinterpret_cast<uintptr_t>(ptr);|return static_cast<unsigned int>(reinterpret_cast<uintptr_t>(ptr));|' \
  "the bug uintptr_t exists to prevent"

mutate "xor-scramble" \
  's|return reinterpret_cast<uintptr_t>(ptr);|return reinterpret_cast<uintptr_t>(ptr) ^ 0xff00;|' \
  "serialize scrambles, deserialize does not unscramble"

printf "\n"
if [ "$survived" -eq 0 ] && [ "$broken" -eq 0 ]; then
  printf "${GREEN}${BOLD}All %d mutants killed.${OFF}\n" "$total"
  exit 0
fi
[ "$survived" -gt 0 ] && printf \
  "${RED}${BOLD}%d of %d survived${OFF} — blind spots in tests/test.cpp\n" "$survived" "$total"
[ "$broken" -gt 0 ] && printf \
  "${RED}${BOLD}%d mutant(s) failed to build${OFF} — harness problem, not a result\n" "$broken"
exit 1
