#!/bin/sh
# The 42 evaluation sheet for this exercise, mechanised. Everything here is a
# rule that gets a project flagged or ungraded rather than merely marked down,
# and every one of them is a property of the SOURCE, which is why none of it
# can live in tests/test.cpp.
#
#   ./tests/eval.sh      (from ex02/ or from tests/)
#
# Sheet items covered:
#   prerequisites  compiles with c++ and -Wall -Wextra -Werror, C++98 only
#   ungradeable    a function implemented in a header (templates excepted)
#   ungradeable    a Makefile with the wrong flags or the wrong compiler
#   ungradeable    no main() with enough tests to prove the program works
#   ungradeable    a non-interface class not in orthodox canonical form
#   forbidden fn   a C function (*alloc, *printf, free)
#   forbidden fn   using namespace <ns>, or the friend keyword
#   forbidden fn   an external library, or anything newer than C++98
#   ex02           inherits std::stack and offers all of its member functions
#   ex02           has an iterator, and the subject's example operations are
#                  possible using it
#   ex02           more tests in main() than the subject's own

cd "$(dirname "$0")/.." || exit 1

CXX="${CXX:-c++}"
STD="-std=c++98"
WARN="-Wall -Wextra -Werror"
OUT="build/eval"
EX="ex02"

# The turn-in files. tests/ and build/ are mine, not the exercise's, and are
# deliberately out of scope: an evaluator only ever sees what is below.
SOURCES="MutantStack.hpp main.cpp"
HEADERS="MutantStack.hpp"

# Colours only when a human is watching: piping this into a file or a chat
# should not paste a wall of escape codes.
if [ -t 1 ] && [ -z "$NO_COLOR" ]; then
  BOLD="\033[1m"; OFF="\033[0m"; GREEN="\033[32m"; RED="\033[31m"
  GREY="\033[90m"; YELLOW="\033[33m"; CYAN="\033[36m"
else
  BOLD=""; OFF=""; GREEN=""; RED=""; GREY=""; YELLOW=""; CYAN=""
fi

mkdir -p "$OUT/stripped"
total=0; failed=0

# The forbidden-keyword greps below run against comment-stripped copies, not
# the sources. A comment that says "not a friend" is not a use of `friend`,
# and a checker that cannot tell the difference trains you to stop explaining
# yourself. (String literals containing // would confuse this; there are none,
# and the compiler checks are the real gate either way.)
strip_comments() {
  awk '
    BEGIN { inblock = 0 }
    {
      line = $0; out = ""; i = 1
      while (i <= length(line)) {
        two = substr(line, i, 2)
        if (inblock) {
          if (two == "*/") { inblock = 0; i += 2 } else { i++ }
        } else if (two == "/*") { inblock = 1; i += 2 }
        else if (two == "//") { break }
        else { out = out substr(line, i, 1); i++ }
      }
      print out
    }' "$1"
}

SCAN=""
for source in $SOURCES; do
  strip_comments "$source" > "$OUT/stripped/$source"
  SCAN="$SCAN $OUT/stripped/$source"
done

pass() {
  total=$((total + 1))
  printf "  ${GREEN}ok${OFF}   %-24s ${GREY}%s${OFF}\n" "$1" "$2"
}

fail() {
  total=$((total + 1)); failed=$((failed + 1))
  printf "  ${RED}FAIL${OFF} %-24s %s\n" "$1" "$2"
  [ -n "$3" ] && printf "%s\n" "$3" | sed 's/^/             /'
}

info() {
  printf "  ${YELLOW}n/a${OFF}  %-24s ${GREY}%s${OFF}\n" "$1" "$2"
}

heading() { printf "\n  ${BOLD}%s${OFF}\n" "$1"; }

# absent <name> <extended-regex> <description>  - must not appear in the code
absent() {
  hits=$(grep -nE "$2" $SCAN 2>/dev/null)
  if [ -z "$hits" ]; then
    pass "$1" "$3"
  else
    fail "$1" "$3" "$hits"
  fi
}

# present <name> <extended-regex> <files> <description>  - must appear
present() {
  if grep -qE "$2" $3 2>/dev/null; then
    pass "$1" "$4"
  else
    fail "$1" "$4" "no line matching: $2"
  fi
}

printf "${BOLD}%s evaluation checklist${OFF}  - the 42 grading sheet, mechanised\n" "$EX"


# ── prerequisites ────────────────────────────────────────────────────────────
heading "prerequisites"

present "makefile-compiler" "^CXX *= *c\+\+" "Makefile" \
  "the Makefile compiles with c++"
present "makefile-warnings" "\-Wall.*\-Wextra.*\-Werror" "Makefile" \
  "and with -Wall -Wextra -Werror"
present "makefile-standard" "\-std=c\+\+98" "Makefile" \
  "and with -std=c++98"

# The evaluator's own command line, not the Makefile's. If these disagree, the
# Makefile is decoration.
if $CXX $STD $WARN -I. *.cpp -o "$OUT/direct" 2>"$OUT/build.log"; then
  pass "direct-build" "c++ $STD $WARN *.cpp builds, zero warnings"
else
  fail "direct-build" "the evaluator's own command line fails" \
    "$(cat "$OUT/build.log")"
fi

# -pedantic-errors turns every extension and every post-C++98 construct the
# compiler would otherwise tolerate into a hard error. This is the check that
# actually enforces "C++98 only".
if $CXX $STD -pedantic-errors -I. -fsyntax-only *.cpp 2>"$OUT/pedantic.log"; then
  pass "strict-c++98" "clean under -std=c++98 -pedantic-errors"
else
  fail "strict-c++98" "something in here is not C++98" \
    "$(head -20 "$OUT/pedantic.log")"
fi


# ── things that make the exercise ungradeable ────────────────────────────────
heading "ungradeable if present"

# A non-inline function body in a header is an ODR violation the moment two
# translation units include it. Rather than guess with grep, link two.
printf '#include "%s"\nint main() { return 0; }\n' "$HEADERS" > "$OUT/tu1.cpp"
printf '#include "%s"\n' "$HEADERS" > "$OUT/tu2.cpp"
if $CXX $STD -I. "$OUT/tu1.cpp" "$OUT/tu2.cpp" -o "$OUT/two" \
    2>"$OUT/odr.log"; then
  pass "no-function-in-header" \
    "two TUs including MutantStack.hpp link: it is all one class template"
else
  fail "no-function-in-header" \
    "a non-template function is defined in a header" "$(cat "$OUT/odr.log")"
fi

if $CXX $STD $WARN -I. -fsyntax-only "$OUT/tu2.cpp" 2>"$OUT/self.log"; then
  pass "header-self-contained" "MutantStack.hpp compiles on its own"
else
  fail "header-self-contained" "the header needs someone else's includes" \
    "$(cat "$OUT/self.log")"
fi

if grep -qE "^[a-zA-Z_].*\bmain *\(" main.cpp; then
  scenarios=$(grep -cE "^static void scenario_" main.cpp)
  pass "main-exists" "main.cpp defines main() and runs $scenarios scenarios"
else
  fail "main-exists" "the subject requires a main with enough tests"
fi


# ── forbidden ────────────────────────────────────────────────────────────────
heading "forbidden"

absent "no-using-namespace" "using +namespace" \
  "no 'using namespace <ns>'"
absent "no-friend" "(^|[^_a-zA-Z])friend[^_a-zA-Z]" \
  "no 'friend' keyword: c is protected, which is enough"
absent "no-c-alloc" "(^|[^_a-zA-Z:.])(malloc|calloc|realloc|free) *\(" \
  "no *alloc / free"
absent "no-c-printf" "(^|[^_a-zA-Z:.])[a-z]*printf *\(" \
  "no printf family"
absent "no-c-string-fns" "(^|[^_a-zA-Z:.])(strcpy|strcat|strlen|strcmp|memcpy|memset) *\(" \
  "no C string/memory functions"

# Every <angled> include must be a C++98 standard header, and every "quoted"
# one must be a file that ships with the exercise.
CXX98_HEADERS="algorithm bitset complex deque exception fstream functional
iomanip ios iosfwd iostream istream iterator limits list locale map memory new
numeric ostream queue set sstream stack stdexcept streambuf string typeinfo
utility valarray vector cassert cctype cerrno cfloat ciso646 climits clocale
cmath csetjmp csignal cstdarg cstddef cstdio cstdlib cstring ctime cwchar
cwctype"
# Folded to a single space-separated line: the list is written across several
# lines for readability, and the substring match below needs a space, not a
# newline, on both sides of every name.
CXX98_HEADERS=$(printf "%s" "$CXX98_HEADERS" | tr '\n' ' ')
strays=""
for header in $(grep -hoE '#include *<[^>]+>' $SCAN | sed 's/.*<\(.*\)>/\1/'); do
  case " $CXX98_HEADERS " in
    *" $header "*) ;;
    *) strays="$strays $header" ;;
  esac
done
for header in $(grep -hoE '#include *"[^"]+"' $SCAN | sed 's/.*"\(.*\)"/\1/'); do
  [ -f "$header" ] || strays="$strays $header"
done
if [ -z "$strays" ]; then
  pass "no-external-include" "only C++98 standard headers and own files"
else
  fail "no-external-include" "not a C++98 standard header:$strays"
fi

absent "no-cxx11-keywords" \
  "(^|[^_a-zA-Z])(nullptr|constexpr|decltype|static_assert|noexcept|override|final|thread_local)([^_a-zA-Z]|$)" \
  "no C++11 keywords"
absent "no-cxx11-library" \
  "std::(unordered_[a-z]+|array|to_string|move|forward|shared_ptr|unique_ptr|nullptr_t|minmax_element|begin|end)\b" \
  "no C++11 library names"
absent "no-defaulted-deleted" "= *(delete|default) *;" \
  "no '= delete' / '= default'"
absent "no-lambda" "\[[&=]?\] *\(" \
  "no lambdas"
# A range-for has a colon and no semicolon between its parentheses; a C++98
# for always has two semicolons. Excluding parentheses as well is what keeps
# `for (X::iterator it = c.begin(); ...)` from matching on its own scope
# resolution operator. -pedantic-errors is the real gate; this only names it.
absent "no-range-for" "for *\([^;()]*:[^;()]*\)" \
  "no range-based for"
absent "no-long-long" "(^|[^_a-zA-Z])long +long([^_a-zA-Z]|$)" \
  "no long long"
absent "no-greedy-brackets" "<[^<>]*<[^<>]*>>" \
  "no >> closing nested templates"

if grep -qE "^(LIBS|LDFLAGS) *= *$" Makefile; then
  pass "no-external-library" "LIBS and LDFLAGS are empty"
else
  fail "no-external-library" "the Makefile links something" \
    "$(grep -nE '^(LIBS|LDFLAGS)' Makefile)"
fi


# ── orthodox canonical form ──────────────────────────────────────────────────
heading "orthodox canonical form"

ocf=0
grep -q "^  MutantStack() : std::stack<T, Container>() {}" $HEADERS && ocf=$((ocf + 1))
grep -q "^  MutantStack(const MutantStack& src)" $HEADERS && ocf=$((ocf + 1))
grep -q "^  MutantStack& operator=(const MutantStack& rhs)" $HEADERS && ocf=$((ocf + 1))
grep -q "^  ~MutantStack() {}" $HEADERS && ocf=$((ocf + 1))
if [ "$ocf" -eq 4 ]; then
  pass "mutantstack-canonical" "MutantStack defines all four canonical members"
else
  fail "mutantstack-canonical" "MutantStack defines only $ocf of the four"
fi

present "assign-self-guard" "if \\(this != &rhs\\)" "$HEADERS" \
  "operator= guards against self-assignment"
present "assign-returns-this" "return \\(\\*this\\);" "$HEADERS" \
  "and returns *this, so a = b = c chains"

# main.cpp's Ui helper is not part of the exercise, but an evaluator will still
# read it, so it declares all four canonical members - privately and without a
# definition, the module 06 ScalarConverter shape, so it cannot be built.
ui_members=0
grep -q "^  Ui();" main.cpp && ui_members=$((ui_members + 1))
grep -q "^  Ui(const Ui& other);" main.cpp && ui_members=$((ui_members + 1))
grep -q "^  Ui& operator=(const Ui& other);" main.cpp && ui_members=$((ui_members + 1))
grep -q "^  ~Ui();" main.cpp && ui_members=$((ui_members + 1))
if [ "$ui_members" -eq 4 ]; then
  pass "ui-helper-canonical" \
    "main.cpp's Ui declares all four, privately: non-instantiable"
else
  fail "ui-helper-canonical" \
    "Ui declares $ui_members of the four canonical members"
fi


# ── ex02-specific sheet items ────────────────────────────────────────────────
heading "$EX specific"

# "There is a MutantStack class that inherits from std::stack and offers all
# of its member functions." Public inheritance is what makes that true without
# writing a single forwarder, so the sheet item is really about the one word.
present "inherits-std-stack" "class MutantStack : public std::stack<T, Container>" \
  "$HEADERS" "public inheritance from std::stack: every member comes for free"

if grep -qE "(push|pop|top|size|empty) *\\(.*\\) *\\{" $HEADERS; then
  fail "no-reimplementation" "a std::stack member is reimplemented here" \
    "$(grep -nE '(push|pop|top|size|empty) *\(.*\) *\{' $HEADERS)"
else
  pass "no-reimplementation" \
    "push/pop/top/size/empty are inherited, not rewritten"
fi

# "It has an iterator." All four, in fact - the two const ones are what make a
# const MutantStack usable at all.
for kind in iterator const_iterator reverse_iterator const_reverse_iterator; do
  present "typedef-$kind" "typedef typename Container::$kind $kind;" \
    "$HEADERS" "re-exports Container::$kind"
done

accessors=0
for accessor in "iterator begin()" "iterator end()" "const_iterator begin() const" \
                "const_iterator end() const" "reverse_iterator rbegin()" \
                "reverse_iterator rend()" "const_reverse_iterator rbegin() const" \
                "const_reverse_iterator rend() const"; do
  grep -q "$accessor" $HEADERS && accessors=$((accessors + 1))
done
if [ "$accessors" -eq 8 ]; then
  pass "eight-accessors" "begin/end/rbegin/rend, const and mutable: all eight"
else
  fail "eight-accessors" "only $accessors of the eight accessors are present"
fi

# The C++98 rule that makes this exercise a rite of passage: `c` is a member of
# a dependent base, so unqualified lookup will not find it.
if grep -qE "return +c\\." $HEADERS; then
  fail "qualified-base-member" "an accessor uses bare c. instead of this->c." \
    "$(grep -nE 'return +c\.' $HEADERS)"
else
  pass "qualified-base-member" "every accessor goes through this->c"
fi
present "uses-this-arrow" "this->c\\." "$HEADERS" \
  "which is mandatory: c lives in a dependent base class"

# "There is at least a main() function that has more tests than the ones from
# the subject." The subject's main is one scenario; this one runs several, and
# tests/subject.cpp keeps the original around with its output asserted.
subject_scenarios=$(grep -cE "^static void scenario_" main.cpp)
if [ "$subject_scenarios" -gt 1 ]; then
  pass "more-than-the-subject" \
    "main.cpp runs $subject_scenarios scenarios, the subject's being one"
else
  fail "more-than-the-subject" "main.cpp does not go past the subject's example"
fi

if [ -f tests/subject.cpp ] && [ -f tests/cumulative.cpp ]; then
  pass "subject-and-cumulative" \
    "the subject's main is asserted, and ex00/ex01 are driven off this class"
else
  fail "subject-and-cumulative" "tests/subject.cpp or tests/cumulative.cpp is missing"
fi


printf "\n"
if [ "$failed" -eq 0 ]; then
  printf "${GREEN}${BOLD}All %d checks passed.${OFF}\n" "$total"
  exit 0
fi
printf "${RED}${BOLD}%d of %d checks failed${OFF} - this would cost points at the defence\n" \
  "$failed" "$total"
exit 1
