#!/bin/sh
# Breaks MutantStack.hpp on purpose, one bug at a time, and checks
# tests/test.cpp notices. A suite that has never been shown to fail is a suite
# nobody has tested; this is the part that tests the tests.
#
# The header has no .cpp to swap at link time, so the exercise is copied into
# build/mutants/ with the broken header in place and the test rebuilt there.
#
#   ./tests/mutants.sh      (from ex02/ or from tests/)
#   VG=1 ./tests/mutants.sh runs each mutant under valgrind
#
# Two things deliberately have no mutant here:
#   * removing `this->` from `this->c.begin()`. It is a compile error in every
#     instantiation, so it would score a trivial killed(c) and prove nothing
#     about the tests. It belongs in tests/compile_fail.sh, where the point is
#     that the error exists at all, and it is there.
#   * deleting the `if (this != &rhs)` guard in operator=. It is an EQUIVALENT
#     MUTANT: the only member is a std::deque held by the base, and
#     self-assigning one is well defined, so the guard saves work and changes
#     no observable behaviour. No runtime assertion could kill it.

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
SRC="MutantStack.hpp"
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
    printf "  ${RED}FAIL${OFF} %-9s %-26s %s\n" "PATTERN" "$1" "sed matched nothing"
    broken=$((broken + 1)); return
  fi

  if ! $CXX $STD -w "$OUT/tests/test.cpp" -o "$OUT/t" 2>/dev/null; then
    printf "  ${GREEN}ok${OFF}   %-9s %-26s ${GREY}%s${OFF}\n" "killed(c)" "$1" "$3"
    return
  fi

  # A mutant may also abort or segfault; any non-zero exit is a kill.
  if $RUNNER "./$OUT/t" >"$OUT/run.log" 2>&1; then
    printf "  ${RED}FAIL${OFF} %-9s %-26s %s\n" "SURVIVED" "$1" "$3"
    survived=$((survived + 1))
  else
    printf "  ${GREEN}ok${OFF}   %-9s %-26s ${GREY}%s${OFF}\n" "killed" "$1" "$3"
  fi
}

printf "${BOLD}ex02 mutation test${OFF}  20 injected bugs, all of which must be caught\n"
printf "${GREY}  every line should say ok. 'killed' means a bug was injected and the\n  tests caught it, which is the result being tested for.${OFF}\n\n"


# ── the iterators, which are the entire exercise ─────────────────────────────
mutate "begin-is-end" \
  's|iterator begin() { return this->c.begin(); }|iterator begin() { return this->c.end(); }|' \
  "begin() returns end(), so every traversal is empty"

mutate "end-is-begin" \
  's|iterator end() { return this->c.end(); }|iterator end() { return this->c.begin(); }|' \
  "end() returns begin(): the range is empty, or runs off the back"

mutate "begin-skips-one" \
  's|iterator begin() { return this->c.begin(); }|iterator begin() { return ++(this->c.begin()); }|' \
  "the bottom of the stack is never visited"

mutate "end-stops-early" \
  's|iterator end() { return this->c.end(); }|iterator end() { return --(this->c.end()); }|' \
  "the top of the stack is never visited"

mutate "begin-and-end-swapped" \
  's|iterator begin() { return this->c.begin(); }|iterator begin() { return this->c.end(); }|; s|iterator end() { return this->c.end(); }|iterator end() { return this->c.begin(); }|' \
  "both ends swapped: traversal walks backwards off the front"

mutate "const-begin-is-end" \
  's|const_iterator begin() const { return this->c.begin(); }|const_iterator begin() const { return this->c.end(); }|' \
  "only the CONST begin() is wrong - a suite that never iterates a const stack misses it"

mutate "const-end-is-begin" \
  's|const_iterator end() const { return this->c.end(); }|const_iterator end() const { return this->c.begin(); }|' \
  "the mirror blind spot on const end()"


# ── the reverse iterators ────────────────────────────────────────────────────
mutate "rbegin-is-begin" \
  's|reverse_iterator rbegin() { return this->c.rbegin(); }|reverse_iterator rbegin() { return this->c.rend(); }|' \
  "rbegin() returns rend(): reverse traversal sees nothing"

mutate "rend-is-rbegin" \
  's|reverse_iterator rend() { return this->c.rend(); }|reverse_iterator rend() { return this->c.rbegin(); }|' \
  "rend() returns rbegin(): the same, from the other side"

mutate "rbegin-skips-top" \
  's|reverse_iterator rbegin() { return this->c.rbegin(); }|reverse_iterator rbegin() { return ++(this->c.rbegin()); }|' \
  "reverse traversal starts one below the top"

mutate "const-rbegin-forward" \
  's|const_reverse_iterator rbegin() const { return this->c.rbegin(); }|const_reverse_iterator rbegin() const { return this->c.rend(); }|' \
  "only the const reverse begin is wrong"

mutate "const-rend-wrong" \
  's|const_reverse_iterator rend() const { return this->c.rend(); }|const_reverse_iterator rend() const { return this->c.rbegin(); }|' \
  "and only the const reverse end"


# ── the typedefs, which are what generic code actually binds to ──────────────
mutate "iterator-is-const" \
  's|typedef typename Container::iterator iterator;|typedef typename Container::const_iterator iterator;|' \
  "the mutable iterator typedef is secretly const: writes stop compiling"

mutate "reverse-typedef-forward" \
  's|typedef typename Container::reverse_iterator reverse_iterator;|typedef typename Container::iterator reverse_iterator;|' \
  "reverse_iterator is not reverse at all"

mutate "const-iterator-is-mutable" \
  's|typedef typename Container::const_iterator const_iterator;|typedef typename Container::iterator const_iterator;|' \
  "const_iterator drops its constness, so a const stack cannot produce one"


# ── the inheritance and the canonical form ───────────────────────────────────
mutate "private-inheritance" \
  's|class MutantStack : public std::stack<T, Container> {|class MutantStack : private std::stack<T, Container> {|' \
  "private inheritance: push/pop/top vanish and std::stack<int> s(m) stops compiling"

mutate "copy-ctor-drops-contents" \
  's|MutantStack(const MutantStack& src) : std::stack<T, Container>(src) {}|MutantStack(const MutantStack\& src) : std::stack<T, Container>() { (void)src; }|' \
  "the copy constructor default-constructs the base: copies come out empty"

mutate "assign-does-nothing" \
  's|if (this != &rhs) std::stack<T, Container>::operator=(rhs);|(void)rhs;|' \
  "operator= is a no-op, so the target keeps its old contents"

mutate "assign-returns-rhs" \
  's|return (\*this);|return (const_cast<MutantStack\&>(rhs));|' \
  "operator= returns the source instead of the target"

mutate "container-ctor-ignores-it" \
  's|      : std::stack<T, Container>(container) {}|      : std::stack<T, Container>() { (void)container; }|' \
  "the container constructor throws away the container it was handed"


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
