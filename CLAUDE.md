# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

42 School C++ piscine solutions: `cpp_module00`–`cpp_module09`, one exercise per `exNN/` subdirectory. All code targets **C++98** (`-std=c++98 -Wall -Wextra -Werror`, compiler `c++`). Each module dir contains its subject PDF (`en.subject.pdf`).

## Build and test

Everything builds per exercise, from inside the exercise directory:

```sh
cd cpp_moduleNN/exNN
make            # builds to build/bin/exNN (objects in build/obj/)
make run        # build + execute
make re         # full rebuild
make clean      # remove build/obj
make fclean     # remove build/ entirely
```

Exception: cpp_module09's binaries carry the subject names — `build/bin/btc`, `build/bin/RPN`, `build/bin/PmergeMe` (hand-edited `EXEC_BIN` lines; regenerating those Makefiles would revert them to exNN).

**Never use the root Makefile** — it hardcodes a WSL path (`/mnt/c/Users/dylan/...`) and is broken on this machine; its `genmake` target calls a nonexistent `autotools/env.sh`.

### Tests

Every exercise in modules 04–09 (plus module01 ex04/ex05) has `tests/test.cpp` — a standalone assert-style C++98 program with its own `main()` (no gtest). Test binaries link against the exercise objects minus `main.o`, so tests can define their own `main()`.

```sh
make test               # modules 04–09: builds AND runs every tests/*.cpp, fails on non-zero exit
./build/bin/<testname>  # run a single test binary directly
```

Modules 00–03 still carry older generated Makefiles whose `test` target only builds (echoes instead of running) — regenerate with gen_make.py to get the running behavior. The gtest machinery in the Makefiles (`make gtest`, `HAS_GTEST`) is vestigial: `vendor/gtest/lib` was never built and no test uses gtest.

### Lint / format

```sh
make format      # clang-format -i, Google style (matches .clang-format)
make norminette  # runs autotools/norminette.sh — a PYTHON script despite the .sh name; needs clang-format + cpplint
```

### Makefiles are generated

Per-exercise Makefiles are generated from `autotools/Makefile.in` by `autotools/gen_make.py` (byte-identical except `SRC_LIST` and `EXEC_BIN`; modules 04–09 were regenerated in July 2026, modules 00–03 still carry older generations). `SRC_LIST` is a hardcoded file list — **adding a new .cpp requires editing SRC_LIST** or regenerating:

```sh
python3 autotools/gen_make.py <exercise-dir>   # auto-detects sources (excludes tests/, build/), names target after the dir
```

Template-wide fixes belong in `autotools/Makefile.in` + regeneration, not per-file edits. There is **no header dependency tracking** (no `-MMD`): after editing a `.hpp`, use `make re`.

## Architecture

- Layout: `cpp_moduleNN/exNN/` with sources flat in the exercise dir, optional `tests/`, `build/` output. No code sharing between exercises — each re-vendors its class files.
- Module map: 00 basics (megaphone, PhoneBook, Account statics) · 01 memory/references (Zombie, Weapon/Human, Harl) · 02 fixed-point `Fixed` class + BSP · 03 inheritance (ClapTrap family) · 04 polymorphism (Animal, AMateria) · 05 exceptions (Bureaucrat/Form) · 06 casts · 07 templates (header-only) · 08 STL containers (Span, MutantStack) · 09 STL apps (BitcoinExchange, RPN, PmergeMe).
- Class style: Orthodox Canonical Form throughout (default ctor, copy ctor, copy assignment, dtor), `explicit` converting ctors, free `operator<<` overloads, virtual dtors on bases.
- Root-level `ex00`–`ex03` are a reworked copy of cpp_module04 that adds the homegrown `postman.hpp` test framework (`ASSERT_R` macros, singleton TestReport). Their Makefiles were generated for a two-levels-deep location, so `make norminette`/gtest paths resolve outside the repo there; plain `make` works.
- `vendor/`: `gtest` (googletest submodule, never built), `libcpp` (helper lib, not actually wired into any module), `scripts` (submodule with SSH-only URL — clone fails without SSH keys).

### Deliberate implementations — do not "simplify"

- `cpp_module02` `Fixed` implements +, −, ×, ÷ with pure bitwise algorithms (XOR/AND full adder, two's complement, shift-and-add multiply, binary long division) — documented in its ex02 README. Fuzzer deviations within ~1.5% relative error are 8-bit fixed-point quantization, not bugs.
- `cpp_module09/ex01` RPN uses `std::stack<int, std::list<int> >` deliberately: module 09 forbids reusing containers across exercises, and this preserves vector/deque for ex02 PmergeMe.
- `cpp_module00/ex00/megaphone.cpp` declares `__attribute__((weak)) int main` so test binaries can override main at link time.
- Modules 04–09 mark deliberate shortcuts with `// ponytail:` comments naming the ceiling and upgrade trigger — read the comment before "fixing" one.
- `cpp_module01/ex04` ships a `std::string::find`-based replacer measured 2.4–11.5x faster than the hand-rolled KMP it replaced; the KMP survives in `tests/kmp_ref.hpp` as the differential test oracle.

## Code style

Google C++ style: 2-space indent, 80 columns, include guards `CPP_MODULENN_EXNN_NAME_HPP_`, enforced by root `.clang-format` / `.cpplintrc` / `.editorconfig`. Some module-level `.vscode` settings say tabs/size-4 — they are wrong; trust the code and `.clang-format`. Exception: `cpp_module00/ex02/Account.hpp` is the subject-provided file (tabs, `#pragma once`) — never reformat it. Note: clang-format is not installed on this machine, so `make format` is a no-op — keep style by hand.

## Repo state notes

Modules 04–09 (plus module01 ex04/ex05) were fully repaired, torture-tested, and dead-code-purged in July 2026: every exercise builds warning-free under `-std=c++98 -Wall -Wextra -Werror`, `make test` passes (suites are mutation-checked), and valgrind reports zero leaks/errors on program and test binaries, success and error paths alike. cpp_module05's exercise dirs were renumbered to match the subject, and cpp_module09/ex00 was stripped to the subject-compliant core (its vendored csv/REPL/table machinery was deleted). Remaining known issues, all in areas that repair did not touch:

- **Modules 00–03 still have committed `build/bin` binaries** (`build/` is `.gitignore`d going forward, but tracked files stay tracked); `make fclean` there deletes tracked files — restore with `git checkout -- <paths>`.
- `cpp_module01` ex01/ex03 have 0-byte `tests/test.cpp` — `make test` there fails with "undefined reference to main".
- Root-level `ex00`–`ex03` (the postman-framework module04 copies) remain as-is; their Makefiles' `../../` paths resolve outside the repo.
- `git submodule status` fatally errors: a gitlink at `cpp_module04/evals42` has no `.gitmodules` entry.
- clang-format and cpplint are not installed on this machine — `make format` and `make norminette` are no-ops.
