# casts_lab — where each cast earns its keep, and where it hurts you

Runnable companions to `ex00`/`ex01`/`ex02`. One lab per exercise, each an A/B:
the version that looks fine next to the version that *is* fine, with the
verdict printed underneath.

Nothing here is part of the graded build. The labs live at the module root
rather than inside `exNN/` on purpose — `autotools/gen_make.py` only excludes
`tests/` and `build/` when it globs sources, so a second `main()` dropped into
an exercise directory gets swept into `SRC_LIST` on the next regeneration and
breaks the link. Each `exNN/` stays exactly as the evaluator expects.

```sh
./run.sh            # build and run all three labs
./run.sh ex00       # one lab (ex00 | ex01 | ex02)
./run.sh negative   # compile the must-not-compile files, show the real errors
./run.sh ubsan      # which tool catches which bug  <- the payoff
./run.sh opt        # strict aliasing at -O0 vs -O2
./run.sh clean
```

Built with `-Wall -Wextra` but deliberately **not** `-Werror`: this code
misbehaves on purpose, and one of the warnings is a lesson in itself.

## The one-line version

| cast | checked when | adjusts pointers | can say "no" |
|---|---|---|---|
| `static_cast` | compile time | **yes** | no |
| `reinterpret_cast` | never | **no** | no |
| `dynamic_cast` | run time | **yes** | **yes** |

Every bug in this lab is a consequence of one of those three columns.

## What each lab shows

**`ex00_static_cast.cpp`** — `static_cast` is checked at compile time and never
at run time, so every trap is a case where the compile-time check passes and
nothing is left to save you. Numeric truncation (defined, merely lossy) →
out-of-range `double`→`int` (undefined, and the actual bug that reshaped
`ScalarConverter`) → downcasting to the wrong class (silent corruption) →
multiple inheritance, where `static_cast` correctly shifts the pointer by 16
bytes. That last one sets up ex01.

**`ex01_reinterpret_cast.cpp`** — the one portable use is the same-type round
trip `T*` → `uintptr_t` → `T*`, which is exactly `Serializer`. Then: the same
integer deserialized as the wrong struct; the multiple-inheritance offset that
`reinterpret_cast` *skips*, so `broken->radioTag = 0` silently zeroes
`Engine::engineTag` instead; and strict aliasing.

The scenario worth your attention is **3D**. Scenario 3B casts `Car*` →
`Radio*` directly and clang catches it (`-Wreinterpret-base-class`). 3D reaches
the identical broken pointer by going through `uintptr_t` — pointer-to-integer,
then integer-to-pointer, each step individually the blessed ex01 pattern. The
compiler never sees a `Car`→`Radio` conversion, so the warning disappears while
the bug remains. Verified: the file produces exactly one warning, and it is on
3B, not 3D.

**`ex02_dynamic_cast.cpp`** — the only cast that asks the object and can answer
no. Checked vs unchecked downcast; the `if (static_cast<A*>(p))` guard that is
**always true** and reads as careful code to every future reviewer; pointer form
(returns `NULL`) vs reference form (throws); cross-casting sideways between
siblings, which `dynamic_cast` can do and `static_cast` cannot even compile.

**`negative/`** — four files that must *fail* to compile. `run.sh negative`
compiles them for real and prints clang's actual errors, because a compile error
is the cheapest possible outcome and recognising these four on sight is worth
more than memorising rules.

## The part that matters: what the tools cannot do

`./run.sh ubsan` runs the labs under `-fsanitize=undefined` and valgrind and
reports what each one finds. Measured on this machine, not asserted:

| bug | `-Wall` | UBSan | valgrind | `dynamic_cast` |
|---|---|---|---|---|
| ex00 #2 out-of-range `double`→`int` | no | **yes** | no | n/a |
| ex00 #3 `static_cast` to wrong type | no | **yes** | no | **yes** |
| ex01 #2 round trip to wrong type | no | no | **yes** | n/a |
| ex01 #3 MI offset, direct cast | **yes** | no | no | n/a |
| **ex01 #3D MI offset via `uintptr_t`** | **no** | **no** | **no** | n/a |
| ex01 #4 strict aliasing | partial | partial | no | n/a |

Two rows deserve comment.

**ex01 #3** is invisible to valgrind, which surprises people. Valgrind sees no
error because there *is* no memory error: every read and write is in-bounds of a
live, initialised `Car`. The pointer simply designates the wrong half of it.
That is a type error, and valgrind does not track types.

**ex01 #3D** is caught by nothing at all. That row is why this module exists.
Past a certain point the named casts are a discipline you apply by reading,
because the tools have run out of things to check.

When you suspect a cast bug: read the warnings, then `-fsanitize=undefined`,
then valgrind, then `printf("%p")` on both sides of the cast — an offset that
changes when it shouldn't, or doesn't change when it should, is the
multiple-inheritance bug on sight. Then reread every cast, which is the only
thing that finds the last row.

## Rules of thumb

- `static_cast` for numeric conversions, upcasts, and downcasts where you
  *already know* the dynamic type. It will not range-check for you.
- `reinterpret_cast` for `T*` ↔ `uintptr_t` round trips back to the **same**
  type, and for inspecting bytes through `char*`. Never with a class hierarchy —
  it skips the adjustment that makes inheritance work.
- `dynamic_cast` whenever "what type is this actually?" is a real question.
  Needs a polymorphic source type; a virtual destructor counts.
- A compile error from `static_cast` is it working. Switching to
  `reinterpret_cast` to silence it removes the only thing that was checking you.
