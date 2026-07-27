# ex03 — At least this beats coffee-making

## The concept

This exercise closes Module 05 with a small **factory**: the `Intern` maps a
runtime string (`"robotomy request"`) to the construction of the matching
concrete class, returning it through the abstract base pointer (`AForm*`).
The caller never names a concrete form type — it asks for a form by name and
receives something it can only use through the `AForm` interface
(sign / execute / print). That is the point: dynamic dispatch decouples
*what the caller asks for* from *which class gets instantiated*.

The subject explicitly forbids the naive `if/else if/else` chain, forcing the
idiomatic data-driven shape: a lookup table of `{name, creator-function}`
pairs. Adding a fourth form means adding one table row, not another branch.

## How this code does it

- `Intern::makeForm(name, target)` (Intern.cpp) walks a static array of
  `{const char*, AForm* (*)(const std::string&)}` entries — plain C++98
  function pointers, since `std::function`/lambdas are C++11. On a match it
  prints `Intern creates <form>` and returns the new form with `target`
  forwarded to its constructor; on no match it prints an error and returns
  `NULL`. The caller owns the returned form and must `delete` it.
- `Intern` has no state at all — the subject says no name, no grade, no unique
  characteristics — but still ships the Orthodox Canonical Form four
  (module-wide requirement).
- Everything else (`Bureaucrat`, `AForm`, the three concrete forms) is the
  ex02 hierarchy: `AForm::execute()` centralizes the signed/grade checks and
  delegates to the private virtual `executeAction()` of each concrete form
  (Template Method), so forms cannot forget the checks.
- `RobotomyRequestForm` uses `std::rand()` for its 50 % outcome —
  the only RNG available in C++98.

## Build / run / test

```sh
make            # builds build/bin/ex03
make run        # build + run the demo main
make test       # builds and runs tests/test.cpp (exits non-zero on failure)
make fclean     # removes build/
```

The demo main follows the subject example (robotomy request on "Bender"),
then shows the other two form names and the unknown-name error path.
`tests/test.cpp` checks: each known name yields the right concrete class,
the target string reaches the form (executing the shrubbery form creates
`test_shrubbery`, which the test removes), an unknown name returns `NULL`,
and an unsigned form refuses to execute. Executing the shrubbery form —
demo or test — writes a `<target>_shrubbery` file in the working directory.
