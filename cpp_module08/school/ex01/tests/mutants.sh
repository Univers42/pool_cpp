#!/bin/sh
# Breaks Span on purpose, one bug at a time, and checks tests/test.cpp
# notices. Several of these are mistakes the 42 evaluation sheet names by
# hand — "finding the shortest span can't be done only by subtracting the two
# lowest numbers" is the first mutant below, and it has to die.
#
# Span.hpp carries the range overload and Span.cpp everything else, so the
# whole exercise is copied into build/mutants/ and one of the two files is
# rewritten there before the test is rebuilt against it.
#
#   ./tests/mutants.sh      (from ex01/ or from tests/)
#   VG=1 ./tests/mutants.sh runs each mutant under valgrind
#
# One defect deliberately has no mutant: removing the `if (this != &rhs)`
# guard from operator=. It is an EQUIVALENT MUTANT here — the members are a
# std::vector and an unsigned int, and self-assigning either is well defined,
# so the guard saves work and changes no observable behaviour. No runtime
# assertion could kill it, and pretending otherwise would be theatre. (It is
# not equivalent for a class that owns raw memory: see module 07's Array,
# where the same guard removal is caught by a live-object counter.)

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
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

# mutate <file> <name> <sed-expr> <what defect this simulates>
mutate() {
  total=$((total + 1))
  cp Span.hpp Span.cpp "$OUT/"
  sed "$3" "$1" > "$OUT/$1" 2>/dev/null

  if cmp -s "$OUT/$1" "$1"; then
    printf "  ${RED}FAIL${OFF} %-9s %-26s %s\n" "PATTERN" "$2" "sed matched nothing"
    broken=$((broken + 1)); return
  fi

  if ! $CXX $STD -w "$OUT/Span.cpp" "$OUT/tests/test.cpp" -o "$OUT/t" \
      2>/dev/null; then
    printf "  ${GREEN}ok${OFF}   %-9s %-26s ${GREY}%s${OFF}\n" "killed(c)" "$2" "$4"
    return
  fi

  # A mutant may also abort or segfault; any non-zero exit is a kill.
  if $RUNNER "./$OUT/t" >"$OUT/run.log" 2>&1; then
    printf "  ${RED}FAIL${OFF} %-9s %-26s %s\n" "SURVIVED" "$2" "$4"
    survived=$((survived + 1))
  else
    printf "  ${GREEN}ok${OFF}   %-9s %-26s ${GREY}%s${OFF}\n" "killed" "$2" "$4"
  fi
}

printf "${BOLD}ex01 mutation test${OFF}  27 injected bugs, all of which must be caught\n"
printf "${GREY}  every line should say ok. 'killed' means a bug was injected and the\n  tests caught it, which is the result being tested for.${OFF}\n\n"


# ── shortestSpan: the algorithm the evaluation sheet warns about ─────────────
# Replaces the fold with a naive implementation that COMPILES, so the kill has
# to come from the assertions rather than from the compiler. This is the one
# mutant in the file I most wanted to see die properly.
mutate Span.cpp "two-lowest-subtracted" \
  's|  return std::inner_product(sorted.begin() + 1, sorted.end(), sorted.begin(),|  return static_cast<unsigned int>(sorted[1]) - static_cast<unsigned int>(sorted[0]);|; /^ *std::numeric_limits<unsigned int>::max(),$/d; /^ *Smaller(), UnsignedGap());$/d' \
  "the sheet's named mistake: subtract the two lowest and call it a day"

mutate Span.cpp "no-sort" \
  's|  std::sort(sorted.begin(), sorted.end());||' \
  "adjacent gaps of an UNSORTED sequence: neighbours are no longer nearest"

mutate Span.cpp "sorts-in-place" \
  's|  std::vector<int> sorted(_vec);|  std::vector<int>\& sorted = const_cast<std::vector<int>\&>(_vec);|' \
  "sorts the stored values instead of a copy: a const query with a side effect"

mutate Span.cpp "fold-starts-at-zero" \
  's|std::numeric_limits<unsigned int>::max(),|0u,|' \
  "the fold's identity is wrong, so the minimum is always 0"

mutate Span.cpp "fold-picks-larger" \
  's|    return a < b ? a : b;|    return a > b ? a : b;|' \
  "Smaller() returns the larger: shortestSpan reports the widest gap"

mutate Span.cpp "pairs-each-with-itself" \
  's|std::inner_product(sorted.begin() + 1, sorted.end(), sorted.begin(),|std::inner_product(sorted.begin(), sorted.end(), sorted.begin(),|' \
  "the two ranges are no longer offset, so every gap is 0"

mutate Span.cpp "short-too-few-check" \
  '/unsigned int Span::shortestSpan/,/^}/{s|if (_vec.size() < 2) throw NotEnoughElementsException();||;}' \
  "shortestSpan answers on 0 or 1 values instead of refusing"


# ── longestSpan ──────────────────────────────────────────────────────────────
mutate Span.cpp "longest-operands-swapped" \
  's|return UnsignedGap()(maxVal, minVal);|return UnsignedGap()(minVal, maxVal);|' \
  "min - max instead of max - min: the unsigned wrap makes it look plausible"

mutate Span.cpp "longest-uses-min-twice" \
  's|const int maxVal = \*std::max_element(_vec.begin(), _vec.end());|const int maxVal = *std::min_element(_vec.begin(), _vec.end());|' \
  "max_element replaced by min_element: longestSpan is always 0"

mutate Span.cpp "long-too-few-check" \
  '/unsigned int Span::longestSpan/,/^}/{s|if (_vec.size() < 2) throw NotEnoughElementsException();||;}' \
  "longestSpan dereferences min_element on an empty vector"


# ── the unsigned-distance trick ──────────────────────────────────────────────
mutate Span.cpp "gap-operands-swapped" \
  's|return static_cast<unsigned int>(high) - static_cast<unsigned int>(low);|return static_cast<unsigned int>(low) - static_cast<unsigned int>(high);|' \
  "UnsignedGap subtracts the wrong way round"

mutate Span.cpp "gap-off-by-one" \
  's|return static_cast<unsigned int>(high) - static_cast<unsigned int>(low);|return static_cast<unsigned int>(high) - static_cast<unsigned int>(low) + 1;|' \
  "every distance is one too large: an inclusive-count confusion"


# ── addNumber(int) and the capacity invariant ────────────────────────────────
mutate Span.cpp "capacity-off-by-one" \
  's|if (_vec.size() >= _maxSize) throw SpanFullException();|if (_vec.size() > _maxSize) throw SpanFullException();|' \
  "N+1 values fit: the classic >= vs > slip"

mutate Span.cpp "no-capacity-check" \
  's|if (_vec.size() >= _maxSize) throw SpanFullException();||' \
  "addNumber never refuses, so N stops meaning anything"

mutate Span.cpp "add-prepends" \
  's|_vec.push_back(n);|_vec.insert(_vec.begin(), n);|' \
  "values are stored in reverse: the spans still work, the order does not"

mutate Span.cpp "full-off-by-one" \
  's|bool Span::full() const { return _vec.size() >= _maxSize; }|bool Span::full() const { return _vec.size() > _maxSize; }|' \
  "full() is never true, so callers cannot tell when to stop"

mutate Span.cpp "size-returns-capacity" \
  's|return static_cast<unsigned int>(_vec.size());|return _maxSize;|' \
  "size() reports the capacity instead of the contents"

mutate Span.cpp "reserve-becomes-resize" \
  's|_vec.reserve(N);|_vec.resize(N);|' \
  "Span(5) is born holding five zeros instead of being empty"


# ── the canonical form ───────────────────────────────────────────────────────
mutate Span.cpp "assign-forgets-capacity" \
  's|    this->_maxSize = rhs._maxSize;||' \
  "operator= copies the values but keeps the target's old N"

mutate Span.cpp "assign-forgets-values" \
  's|    this->_vec = rhs._vec;||' \
  "operator= copies N but leaves the values behind"

mutate Span.cpp "copy-ctor-drops-values" \
  's|Span::Span(const Span& src) : _maxSize(src._maxSize), _vec(src._vec) {}|Span::Span(const Span\& src) : _maxSize(src._maxSize) {}|' \
  "the copy constructor produces an empty Span of the right capacity"

mutate Span.cpp "print-separator" \
  's|if (it != span.begin()) os << ", ";||' \
  "operator<< runs the values together with no separator"


# ── the range overload, which lives in the header ────────────────────────────
mutate Span.hpp "range-no-check" \
  's|if (std::distance(first, last) + _vec.size() > _maxSize) {|if (false) {|' \
  "the range overload ignores capacity entirely"

mutate Span.hpp "range-ignores-existing" \
  's|std::distance(first, last) + _vec.size() > _maxSize|_vec.size() > _maxSize|' \
  "capacity is checked against what is already there, not against the total"

mutate Span.hpp "range-off-by-one" \
  's|std::distance(first, last) + _vec.size() > _maxSize|std::distance(first, last) + _vec.size() >= _maxSize|' \
  "a range that fills the Span exactly is rejected"

mutate Span.hpp "range-prepends" \
  's|_vec.insert(_vec.end(), first, last);|_vec.insert(_vec.begin(), first, last);|' \
  "a bulk insert lands at the front, silently reordering the Span"

mutate Span.hpp "range-check-after-insert" \
  's|    if (std::distance(first, last) + _vec.size() > _maxSize) {|    _vec.insert(_vec.end(), first, last);\n    if (_vec.size() > _maxSize) {|' \
  "insert first, check second: a rejected range leaves half the damage behind"


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
