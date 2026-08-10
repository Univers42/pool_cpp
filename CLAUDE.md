# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

42 School C++ piscine solutions: `cpp_module00`–`cpp_module09`, one exercise per `exNN/` subdirectory. All code targets **C++98** (`c++ -std=c++98 -Wall -Wextra -Werror`, zero warnings). Each module dir carries its subject (`en.subject.pdf`, or `subject.txt` in module04) and a `README.md` narrative of what the exercises teach — read the module README before changing an exercise; it usually explains why the code looks the way it does.

## Build and test

Everything builds **per exercise, from inside the exercise directory**:

```sh
cd cpp_moduleNN/exNN
make            # → build/bin/exNN   (objects in build/obj/)
make run        # build + execute
make test       # build and run every tests/*.cpp
make re         # full rebuild
make clean      # remove build/obj
make fclean     # remove build/ entirely
```

- **cpp_module09 binaries carry subject names** — `build/bin/btc`, `build/bin/RPN`, `build/bin/PmergeMe`. Those `EXEC_BIN` lines are hand-edited; regenerating those Makefiles reverts them to `exNN`.
- **Never use the root `Makefile`.** It hardcodes a WSL path (`/mnt/c/Users/dylan/...`) that doesn't exist here, and its `genmake` target shells out to `autotools/env.sh`, which doesn't exist either.
- **No header dependency tracking** (no `-MMD` anywhere): after editing a `.hpp`, `make re`, not `make`.

### Tests

Tests are standalone assert-style C++98 programs with their own `main()` — **no gtest**, despite the `make gtest` / `HAS_GTEST` machinery in every Makefile (`vendor/gtest` was never built and nothing includes it). Test binaries link the exercise objects minus `main.o` (`OBJ_NO_MAIN`), which is what lets them supply their own `main`.

```sh
make test                # build + run all tests/*.cpp, aborts on first non-zero exit
./build/bin/<testname>   # run one test binary directly (name = tests/<testname>.cpp)
```

`make test` only *runs* tests in exercises with a regenerated Makefile: **module01 ex04/ex05, and modules 05–09**. In module00, module01 ex00–ex03/ex06, module02, module03 and `cpp_module04/from_school`, the older `test` target builds the binaries and then merely echoes "Executing …" without running them. Regenerate with `gen_make.py` to get running behavior.

### Lint / format

```sh
make norminette   # autotools/norminette.sh — a PYTHON script despite the .sh name
make format       # clang-format -i, hardcoded -style=Google (ignores repo .clang-format overrides)
```

On this machine only `clang-format` is installed; `norminette` reports `clang-tidy`, `cppcheck` and `cpplint` as *skipped*. It currently **exits non-zero on formatting diffs in code that is otherwise finished** — a norminette failure is not by itself evidence you broke something.

### Makefiles are generated — do not hand-edit them casually

Per-exercise Makefiles come from `autotools/Makefile.in` via `autotools/gen_make.py`:

```sh
python3 autotools/gen_make.py <exercise-dir>   # auto-detects sources (skips tests/, build/), names target after the dir
```

`SRC_LIST` is a **hardcoded file list**, so adding a new `.cpp` means editing `SRC_LIST` or regenerating. Template-wide fixes belong in `autotools/Makefile.in` followed by regeneration, never in a per-exercise copy. Regeneration overwrites hand-edits — including module09's `EXEC_BIN` names.

## Architecture

Sources sit flat in each `exNN/`, with an optional `tests/` and generated `build/`. **There is no code sharing between exercises** — each one re-vendors its own class files, so a fix to `Animal.cpp` in ex01 does not reach ex02. Expect and preserve that duplication.

Module map: 00 basics (megaphone, PhoneBook, `Account` statics) · 01 memory/references (Zombie, Weapon/Human, Harl, file replacer) · 02 fixed-point `Fixed` + BSP · 03 inheritance (ClapTrap family) · 04 polymorphism (Animal, AMateria) · 05 exceptions (Bureaucrat/Form/Intern) · 06 casts · 07 templates (header-only — `SRC_LIST` is just `main.cpp`) · 08 STL containers (Span, MutantStack) · 09 STL applied (BitcoinExchange, RPN, PmergeMe).

Class conventions throughout: Orthodox Canonical Form (default ctor, copy ctor, copy assignment, dtor), `explicit` converting ctors, free `operator<<` overloads, virtual dtors on every base. Deep-copy assignment follows **clone first, delete second, assign last**, so a throwing copy leaves the old state intact.

`vendor/`: `gtest` (googletest submodule, never built), `libcpp` (two files, `assertion.{cpp,hpp}`, not wired into any module), `scripts` (submodule of helper shell/python tools).

### Deliberate implementations — do not "simplify"

- `// ponytail:` comments (24 of them, modules 01/05/06/08/09) mark accepted shortcuts and name the ceiling and the trigger to revisit. Read the comment before "fixing" the code under it.
- `cpp_module02`'s `Fixed` implements `+ − × ÷` with pure bitwise algorithms (XOR/AND full adder, two's complement, shift-and-add, binary long division). Fuzzer deviations within ~1.5% relative error are 8-bit fixed-point quantization, not bugs.
- `cpp_module09/ex01` uses `std::stack<int, std::list<int> >` on purpose: module 09 forbids reusing a container across exercises, so this reserves vector/deque for ex02's PmergeMe.
- `cpp_module00/ex00/megaphone.cpp:120` declares `__attribute__((weak)) int main` so test binaries can override it at link time.
- `cpp_module01/ex04` ships a `std::string::find`-based replacer that measured 2.4–11.5× faster than the hand-rolled KMP it replaced; the KMP survives as `tests/kmp_ref.hpp`, a differential oracle. Don't "restore" it as the implementation.

## Code style

Google style: 2-space indent, 80 columns, include guards spelled `CPP_MODULENN_EXNN_NAME_HPP_`, 42-header comment block at the top of each file. Enforced by root `.clang-format` / `.cpplintrc` / `.editorconfig`. Some module-level `.vscode` settings claim tabs/size-4 — they are wrong for C++; trust `.clang-format`. Exception: `cpp_module00/ex02/Account.hpp` is subject-provided (tabs, `#pragma once`) — never reformat it.

## Repo state notes

Modules 04–09 plus module01 ex04/ex05 were repaired and torture-tested in July 2026: warning-free builds, mutation-checked suites, valgrind-clean on success *and* error paths. Modules 00–03 are piscine originals that build but never got that treatment. Known rough edges:

- **cpp_module04 is mid-restructure and does not build as-is.** The working tree deleted `ex00`–`ex02` and most of `ex03` (only `Ice.cpp` + `MateriaSource.cpp` and no Makefile remain) and the module README; untracked `from_school/` holds the school originals (ex00–ex03 + a vendored `libcpp/`), `training/` holds scratch practice. Recover deleted originals from git rather than rewriting them.
- **cpp_module05 has three copies of the same exercises**: tracked `ex00`–`ex03`, untracked `solution/` (byte-identical to the tracked ones), and untracked `solution_school/` (a genuinely different variant, with an extra `tests/ocf_mutants.sh`). Edit the tracked `exNN/` unless told otherwise.
- **15 `build/bin` binaries are still tracked** in modules 00–03. `build/` is gitignored going forward, but tracked stays tracked — `make fclean` there deletes tracked files; restore with `git checkout -- <paths>`.
- `cpp_module01` ex01/ex03 have **0-byte `tests/test.cpp`** — `make test` there fails with "undefined reference to main".
- `git submodule status` **fatally errors**: a gitlink at `cpp_module04/evals42` (an empty directory) has no `.gitmodules` entry.
