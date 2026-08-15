# Makefile Auto-Generation System

## Overview
This system allow us to automatically generate professional, modern Makefiles for
C++ projects with:
- Auto-detection of source files
- Google Test Integration
- Modern build structure (build/obj, build/bin/)
- clean, professional output
- code quality checking (norminette)
- Consistent structure across all exercices

## Files
1. `Makefile.in` -Template
The master template with placeholders that get replaced during generation:
- $CXX -Compiler
- $CXXFLAGS -Compiler flags
- $SRC -Source Files
- $Target -Executable name
- $GTEST_DIR -Path to Google test
- $CHECKER -Path to checker script
- $LIB_DIR -Path to libcpp(if using)
- $BUILD_LIBCPP - Whether to build libcpp

2. `generate_makefile.py` - Generator_script
Python script that:
- auto-detects source files (*.cpp, *.c)
- Auto-detects Google test location
- Auto-detects checker script
- Generates target name from directory
- Creates ready-to-use Makefile

3. `gen_class.py` - Class generator

Writes the boilerplate every 42 C++ class repeats, so the only thing left to
type is the part that is actually about the exercise. See below.

4. `check_all.sh` - Whole-module verification

Builds everything from clean, runs every test, then goes looking for the
failures a passing suite cannot see. See below.

5. `norminette.sh` - a **Python** script despite the name. Runs clang-format,
   and reports clang-tidy / cppcheck / cpplint as skipped when they are not
   installed.

# Quick start
## Makefiles (auto-detect everything)

```sh
./gen_make.py                    # the current directory
./gen_make.py /path/to/exercise
```

> Regenerating **overwrites hand-edits** — module 09's `EXEC_BIN` names and
> module 08's `test` / `quick` / `verify` targets are hand-written. Prefer
> editing `Makefile.in` and regenerating deliberately, or let `gen_class.py
> --update-makefile` patch `SRC_LIST` in place.

## Classes

```sh
./gen_class.py                   # interactive wizard - the usual way
./gen_class.py Bureaucrat        # wizard, name pre-filled
./gen_class.py --self-test       # generate one of every kind and compile it
```

The wizard asks what kind of class you want and what goes in it, then writes
the `.hpp` and `.cpp` with everything that is dictated rather than decided:

- the 42 header block, with the `By:` line **copied from a neighbouring file**
  so a directory never ends up with two different logins in it;
- the include guard spelled after the path — `cpp_module05/ex00/Bureaucrat.hpp`
  becomes `CPP_MODULE05_EX00_BUREAUCRAT_HPP_`;
- Orthodox Canonical Form: default constructor with every member
  value-initialised, copy constructor, copy assignment with the
  self-assignment guard, destructor — `virtual` the moment anything can derive
  from it;
- a constructor taking every attribute, `explicit` when it takes exactly one;
- getters returning `const T&` for class types and by value for primitives
  (and `isSigned()` rather than `getIsSigned()` for a `bool` already named like
  a question), setters for everything that is not `const`;
- a free `operator<<` — never a `friend`, which the evaluation sheet flags;
- nested exception classes deriving from `std::exception`, with the `what()`
  bodies in the `.cpp` where a non-template function body belongs;
- forward declarations for types used only through a reference or pointer, and
  `#include "X.hpp"` for types stored by value.

### Kinds

| kind | what you get |
| --- | --- |
| `base` | a plain concrete class in OCF |
| `polymorphic` | the same, with a virtual destructor |
| `abstract` | at least one pure virtual; a pure **destructor** if you name none |
| `interface` | all pure virtual, no data, header-only (the `AMateria` shape) |
| `derived` | inherits, with the base wired into every constructor and `operator=` |
| `template` | a class template, entirely in the header as C++98 requires |
| `utility` | non-instantiable, all static (the `ScalarConverter` shape) |
| `exception` | derives from `std::exception` with a `what()` override |

### Non-interactive

Every question is also a flag, which is what makes a whole hierarchy fast:

```sh
./gen_class.py Animal --kind abstract --attr 'std::string type' \
    --pure 'void makeSound() const' --ostream
./gen_class.py Dog --kind derived --base Animal --attr 'Brain* brain' --ostream
./gen_class.py Cat --kind derived --base Animal --attr 'Brain* brain' --ostream
```

Useful extras:

- `--protected-pure` puts the pure virtuals under `protected:` — the
  template-method shape, where a public member validates and then delegates
  (module 05's `AForm::executeAction`). Public is the default, since
  `Animal::makeSound` is the commoner one.
- `--with-test` also writes `tests/<name>_test.cpp` with the canonical-form
  assertions already written: copy, assign, independence, self-assignment
  through an alias so `-Wself-assign-overloaded` stays quiet.
- `--update-makefile` adds the new `.cpp` to `SRC_LIST` **in place**, keeping
  `main.cpp` last. Deliberately surgical rather than a `gen_make.py`
  regeneration, which would discard hand-edits.
- `--dry-run` prints instead of writing; `--force` overwrites.
- `--namespace`, `--template-param`, `--no-setters`, `--no-value-ctor`,
  `--login`, `--email`.

### Why `--self-test` exists

A generator that emits code nobody compiles is a generator that quietly rots.
`./gen_class.py --self-test` writes one class of every kind — plus a derived
class on top of the abstract one, a class mentioning types it does not own, and
a class whose member names are long enough to force every line-wrapping branch
— then checks all of it:

1. every file is exactly 80 columns with a well-formed 42 header;
2. all the kinds compile **and link together** under
   `-std=c++98 -Wall -Wextra -Werror -pedantic-errors`;
3. the resulting binary runs;
4. two translation units including each header link — the real test for "a
   function implemented in a header file", which is an instant zero;
5. no `using namespace`, no `friend`, no C functions anywhere in the output
   (checked on comment-stripped copies, so explaining a rule is not mistaken
   for breaking it).

It found four genuine bugs the first time it ran, and two more after that. If
you change the templates, run it.

Generated files are already `clang-format -style=Google` clean, so `make
format` is a no-op on them.

## Verifying a whole module

```sh
./autotools/check_all.sh                     # cpp_module08 (default)
./autotools/check_all.sh cpp_module07 cpp_module08
SEEDS="1 2 3" ./autotools/check_all.sh       # more fuzz seeds
QUICK=1 ./autotools/check_all.sh             # skip valgrind and sanitizers
```

Six passes per exercise:

| pass | what it catches |
| --- | --- |
| `build` | `make fclean && make` — the exercise's own Makefile, from nothing |
| `c++98-strict` | `c++ -std=c++98 -Wall -Wextra -Werror -pedantic-errors` straight at `*.cpp`, because a Makefile can lie about its own flags |
| `make test` | the assertions, `eval.sh`, `mutants.sh`, `compile_fail.sh` |
| `valgrind` | leaks **and** errors on every binary, `--error-exitcode=42` |
| `sanitizers` | a rebuild under `-fsanitize=address,undefined` |
| `fuzz seeds` | the randomised suites re-run on other seeds |

The sanitizer pass is not redundant with valgrind. Valgrind sees the heap;
ASan also sees **stack and global** overflows, and UBSan catches signed
overflow and bad shifts — which is precisely the class of bug `Span`'s
unsigned-distance trick exists to avoid, and precisely the class valgrind
would let through.

Then the lab (`cpp_module08/all.cpp`, C++17, different flags) and
`gen_class.py --self-test`.

### Reading the output

Every result line starts with `ok`, `FAIL` or `n/a` — that first word is the
only one that says whether something is wrong. The second column names what
happened in that check's own vocabulary, and on a healthy run it reads
alarming on purpose: `rejected` means the compiler refused code that must not
compile, and `killed` means an injected bug was caught by the tests. Both are
passes. The words that are *not* passes are `SURVIVED`, `COMPILED`, `NOWARN`,
`ILLEGAL` and `PATTERN`, and each is printed as `FAIL`.

Colours turn themselves off when stdout is not a terminal, so a log pasted
elsewhere stays readable. `NO_COLOR=1` forces that too.

### It has been shown to fail

A checker nobody has broken is a checker nobody has tested. Three bugs were
injected into a throwaway copy of `ex01` to confirm each mechanism actually
fires:

| injected | caught by |
| --- | --- |
| read one past a stack array | `sanitizers` **and** `valgrind` |
| `new int[8]` nothing frees | `valgrind`, and `sanitizers` (`detect_leaks=1`) |
| `maxVal - minVal` as signed ints | `sanitizers` only |

The third is worth dwelling on: on this hardware the signed overflow wraps to
the same bits as the correct unsigned subtraction, so every assertion still
passed and the fuzzer stayed green. Nothing but UBSan could see it. That is
the argument for the pass existing.

