# ex00 — Mommy, when I grow up, I want to be a bureaucrat!

## The concept

This exercise introduces **C++ exceptions** as the mechanism for enforcing a
class invariant. A `Bureaucrat` has a constant name and a grade that must
always sit in `[1, 150]` — and, counter-intuitively, **1 is the highest grade
and 150 the lowest**. Instead of returning error codes or silently clamping,
every operation that could break the invariant (construction, increment,
decrement) `throw`s one of two dedicated exception types:

- `Bureaucrat::GradeTooHighException` — the grade would go above 1
  (i.e. below the number 1),
- `Bureaucrat::GradeTooLowException` — the grade would go below 150
  (i.e. above the number 150).

Two lessons are baked in:

1. **Exceptions separate detection from handling.** The class detects the
   violation at the exact point it would occur; the caller decides what to do
   in a `try`/`catch` block, possibly far away. A `Bureaucrat` object can
   therefore *never exist* in an invalid state — if the constructor throws,
   the object is never created.
2. **Custom exception types carry meaning.** Both classes derive from
   `std::exception` and override `what()`, so callers can catch the precise
   type or just `std::exception&` — the subject's example `catch` works for
   both. In C++98 the override must keep the base's exception specification,
   hence `const char* what() const throw()` (no `noexcept` — that is C++11).

## How the code demonstrates it

- `Bureaucrat.hpp` — the class in Orthodox Canonical Form (default ctor, copy
  ctor, copy assignment, dtor) plus the `(name, grade)` constructor and the
  two nested exception classes. The exception classes deliberately skip OCF —
  the subject explicitly allows it. `_name` is `const`: it is set once at
  construction and copy assignment only copies the grade.
- `Bureaucrat.cpp` — all range checks funnel through one private
  `validateGrade(int)` helper, so the constructor, `incrementGrade()` and
  `decrementGrade()` share a single source of truth for the invariant.
  Mutators validate the *prospective* grade **before** touching `_grade`,
  giving the strong exception guarantee: a failed increment/decrement leaves
  the object unchanged. Because grade 1 is the best, `incrementGrade()`
  does `--_grade` and `decrementGrade()` does `++_grade`.
- The free `operator<<` prints the subject's exact format:
  `<name>, bureaucrat grade <grade>.`
- `main.cpp` — demo of every required behavior: printing via `operator<<`,
  incrementing 3 → 2 → 1, then each of the four throw paths (increment past
  1, decrement past 150, construct with 0, construct with 151), all caught as
  `std::exception&` per the subject's example.

## Build / run / test

```sh
make            # builds build/bin/ex00
make run        # build + execute the demo
make test       # builds AND runs tests/test.cpp (build/bin/test)
make fclean     # removes build/
```

`tests/test.cpp` is a standalone C++98 program (own `main()`, linked against
the exercise objects minus `main.o`) that exits non-zero on failure. It
checks: grades 1/150 construct while 0/151 throw the correct exception type,
increment/decrement move in the right direction, a throwing mutation leaves
the grade unchanged, `operator<<` output matches the subject format
byte-for-byte, and copy construction/assignment behave with the `const` name.

Compiled with `c++ -std=c++98 -Wall -Wextra -Werror`; both binaries are
valgrind-clean (0 leaks, 0 errors).
