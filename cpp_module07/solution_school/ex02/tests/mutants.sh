#!/bin/sh
# Breaks Array.hpp on purpose, one bug at a time, and checks tests/test.cpp
# notices. Some of these are the bugs I actually had: freeing before
# allocating in operator=, and the missing delete[] in cloneBuffer's catch.
# They only get caught because Bomb and Mine count how many are alive.
#
# The header has no .cpp to swap at link time, so the exercise is copied into
# build/mutants/ with the broken header in place and the test built there.
#
#   ./tests/mutants.sh      (from ex02/ or from tests/)
#   VG=1 ./tests/mutants.sh runs each mutant under valgrind

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
SRC="Array.hpp"
TEST="tests/test.cpp"
OUT="build/mutants"
RUNNER=""
[ -n "$VG" ] && RUNNER="valgrind -q --error-exitcode=9 --leak-check=full"

BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"; GREY="\033[90m"

# Start from an empty scratch dir. A leftover copy from a previous run can
# satisfy an include that the harness no longer provides, which hides a
# broken harness behind a clean sweep.
rm -rf "$OUT"
mkdir -p "$OUT/tests"
cp "$TEST" "$OUT/tests/test.cpp"
# test.cpp also drives iter, bigint and vect2, so they have to travel with it.
# Without them every mutant fails to COMPILE, and a compile failure counts as
# a kill below - which is exactly how this suite once passed while testing
# nothing at all. The baseline check underneath is what makes that loud.
cp iter.hpp bigint.hpp bigint.cpp vect2.hpp vect2.cpp "$OUT/"
AUX="$OUT/bigint.cpp $OUT/vect2.cpp"
total=0; survived=0; broken=0

# The unmutated exercise must build AND pass before any mutant is judged.
# Otherwise "killed(c)" means the harness is broken, not the code.
cp "$SRC" "$OUT/$SRC"
if ! $CXX $STD -w "$OUT/tests/test.cpp" $AUX -o "$OUT/baseline" 2>"$OUT/baseline.log"; then
  printf "${RED}${BOLD}baseline does not compile${OFF} - the harness is broken, not Array.hpp\n"
  sed 's/^/  /' "$OUT/baseline.log"
  exit 1
fi
if ! "./$OUT/baseline" >"$OUT/baseline.run" 2>&1; then
  printf "${RED}${BOLD}baseline does not pass${OFF} - fix tests/test.cpp before mutating\n"
  exit 1
fi

# mutate <name> <sed-expr> <what defect this simulates>
mutate() {
  total=$((total + 1))
  sed "$2" "$SRC" > "$OUT/$SRC" 2>/dev/null

  if cmp -s "$OUT/$SRC" "$SRC"; then
    printf "  ${RED}%-9s${OFF} %-24s %s\n" "PATTERN" "$1" "sed matched nothing"
    broken=$((broken + 1)); return
  fi

  if ! $CXX $STD -w "$OUT/tests/test.cpp" $AUX -o "$OUT/t" 2>/dev/null; then
    printf "  ${GREEN}%-9s${OFF} %-24s ${GREY}%s${OFF}\n" "killed(c)" "$1" "$3"
    return
  fi

  # A mutant may also abort or segfault; any non-zero exit is a kill.
  if $RUNNER "./$OUT/t" >"$OUT/run.log" 2>&1; then
    printf "  ${RED}%-9s${OFF} %-24s %s\n" "SURVIVED" "$1" "$3"
    survived=$((survived + 1))
  else
    printf "  ${GREEN}%-9s${OFF} %-24s ${GREY}%s${OFF}\n" "killed" "$1" "$3"
  fi
}

printf "${BOLD}ex02 mutation test${OFF}  - 'killed' means tests/test.cpp caught the bug\n\n"


# These two patch the sized constructor, which is spelled with a ternary
# (n > 0 ? new T[n]() : NULL) so an empty Array allocates nothing. Patterns
# here match the source literally, so a rewrite of that line lands as
# "sed matched nothing" rather than as a passing mutant - which is the point
# of reporting a failed apply separately from a survivor.
mutate "no-value-init" \
  's|new T\[n\]() : NULL|new T[n] : NULL|' \
  "new T[n] without (): ints hold garbage instead of 0"

mutate "off-by-one-alloc" \
  's|new T\[n\]() : NULL), _size(n)|new T[n + 1]() : NULL), _size(n + 1)|' \
  "allocates and reports one element too many"

mutate "default-ctor-allocates" \
  's|Array() : _array(NULL), _size(0) {}|Array() : _array(new T[1]()), _size(1) {}|' \
  "the default ctor preventively allocates, which the subject forbids"


mutate "bounds-off-by-one" \
  's|if (index >= _size) throw OutOfBoundsException();|if (index > _size) throw OutOfBoundsException();|g' \
  "index == size is allowed through: a one-element overrun"

mutate "no-bounds-check" \
  's|if (index >= _size) throw OutOfBoundsException();||g' \
  "operator[] does not check at all"

mutate "const-no-bounds-check" \
  's|^  const T& operator\[\](unsigned int index) const {\n|&|; /const T& operator\[\](unsigned int index) const {/{n;s|if (index >= _size) throw OutOfBoundsException();||;}' \
  "only the CONST overload lost its check - a suite that never uses a const Array misses this"

mutate "signed-index" \
  's|T& operator\[\](unsigned int index) {|T\& operator[](int index) {|' \
  "a signed index: -2 no longer wraps into the >= _size check"

mutate "wrong-message" \
  's|return "Error: Array index is out of bounds!";|return "out of range";|' \
  "what() no longer says what the subject specified"


mutate "shallow-copy-ctor" \
  's|: _array(cloneBuffer(src._array, src._size)), _size(src._size) {}|: _array(src._array), _size(src._size) {}|' \
  "the copy ctor shares the buffer - the classic double free"

mutate "copy-ctor-size-only" \
  's|: _array(cloneBuffer(src._array, src._size)), _size(src._size) {}|: _array(new T[src._size]()), _size(src._size) {}|' \
  "the copy ctor allocates but never copies the contents"

# This one is worth reading carefully. Removing the self-assignment guard is
# the textbook Rule-of-Three bug, and here it is NOT a bug: cloneBuffer builds
# the new buffer before delete[] _array runs, so a self-assignment reads live
# memory and still produces the right answer (checked under valgrind).
# For every value-based assertion it is an EQUIVALENT MUTANT - no test could
# ever kill it, and chasing it would be chasing a difference that isn't there.
# It dies only because the guard's real purpose - skipping the work - is
# itself asserted, via Tracked::constructed.
mutate "no-self-assign-guard" \
  's|if (this != &rhs) {|if (true) {|' \
  "guard removed: correct, but self-assignment now reallocates and recopies"

mutate "assign-skips-empty" \
  's|if (this != &rhs) {|if (this != \&rhs \&\& rhs._size > 0) {|' \
  "operator= early-returns on an empty rhs, keeping the target's old contents"

mutate "assign-keeps-old-size" \
  's|_size = rhs._size;||' \
  "operator= copies the buffer but forgets to update the size"

mutate "leaky-assign" \
  's|      delete\[\] _array;||' \
  "operator= abandons the old buffer instead of freeing it"

mutate "dtor-no-delete" \
  's|~Array() { delete\[\] _array; }|~Array() {}|' \
  "the destructor leaks the whole buffer"

mutate "dtor-scalar-delete" \
  's|~Array() { delete\[\] _array; }|~Array() { delete _array; }|' \
  "delete instead of delete[]: only the first element is destroyed"


mutate "clone-no-cleanup" \
  's|      delete\[\] fresh;||' \
  "cloneBuffer's catch rethrows without freeing - the 16-bytes-lost leak"

mutate "clone-swallows" \
  's|^      throw;||' \
  "cloneBuffer swallows the exception and returns a half-copied buffer"

mutate "free-before-alloc" \
  's|      T\* fresh = cloneBuffer(rhs._array, rhs._size);\n||; s|      T\* fresh = cloneBuffer(rhs._array, rhs._size);|      delete[] _array; _array = NULL; T* fresh = cloneBuffer(rhs._array, rhs._size);|' \
  "operator= frees first and allocates second - the dangling-pointer double free"

mutate "clone-null-on-empty" \
  's|if (n == 0) return NULL;|if (n <= 1) return NULL;|' \
  "cloneBuffer returns NULL for a 1-element array, so the copy reads NULL"

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
