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
  function pointers, since `std::function`/lambdas are C++11. The three
  creators live in an anonymous namespace and share one signature, which is
  what lets a single pointer type hold all of them.
- On a match it builds the form **first** and prints `Intern creates <form>`
  **second**, so a throwing constructor can never produce an announcement for
  a form that does not exist. The line prints the built form's own
  `getName()`, which makes it self-verifying.
- On no match it prints an explicit error that echoes the bad name and lists
  the names it does know, then returns `NULL`. Returning `NULL` rather than
  throwing is deliberate: the subject's own example assigns the result
  straight into an `AForm*` with no `try`/`catch` around it.
- **The caller owns the returned form and must `delete` it.** That is forced
  by the signature — `AForm` is abstract, so it cannot be returned by value.
- `Intern` has no state at all — the subject says no name, no grade, no unique
  characteristics — but still ships the Orthodox Canonical Form four, all
  empty. `makeForm` is `const`: making a form does not change the intern.

Everything else (`Bureaucrat`, `AForm`, the three concrete forms) is the ex02
hierarchy, copied unchanged:

- `AForm::execute()` is public and **non-virtual**; it centralizes the
  signed/grade checks and delegates to the **protected** pure virtual
  `executeAction()` of each concrete form (Template Method), so no form can
  forget or skip a check.
- Every class here is in full **Orthodox Canonical Form**, default constructor
  included. Module 05's subject exempts only the exception classes — *"Please
  note that exception classes do not have to be designed in Orthodox Canonical
  Form. However, every other class must follow it."* — and the evaluation sheet
  refuses to grade an exercise whose non-interface classes fall short. A
  default-constructed form targets `"default"` and still carries its class's
  real name and grades; nothing else calls it, which is why `tests/test.cpp`
  gives it its own section and `tests/mutants.sh` its own mutants.
- `RobotomyRequestForm` uses `std::rand()` for its 50 % outcome — the only RNG
  available in C++98.

## Build / run / test

```sh
make            # builds build/bin/ex03
make run        # build + run the demo main
make test       # builds and runs tests/test.cpp (exits non-zero on failure)
make re         # full rebuild — required after editing any .hpp (no -MMD here)
make fclean     # removes build/ and any stray *_shrubbery files
```

The demo main follows the subject example (robotomy request on "Bender"),
then walks all three form names, the unknown-name and near-miss error paths,
an end-to-end intern → sign → execute flow, and the OCF of a stateless class.

`tests/test.cpp` is ex02's suite plus four Intern sections — **93 checks**
total. The Intern sections cover: each known name yielding the right concrete
type with its own grades and unsigned, the `Intern creates <form>`
announcement, the target string actually reaching the form (proved by
executing the shrubbery form and finding `<target>_shrubbery`, then again with
a second target so a hard-coded one cannot pass), unknown and near-miss names
returning `NULL` with an error and no announcement, and `makeForm` being
callable on a `const Intern`.

```sh
bash tests/mutants.sh
```

The sabotage drill breaks the sources one mutation at a time and checks that
the suite notices. **34 mutants, 0 survivors** — including eight aimed at the
Intern (swapped table rows, a typo'd key, ignoring the name, passing the name
as the target, a missing announcement, and both failure-path regressions), and
three on the OCF default constructors.

Executing the shrubbery form — demo or test — writes a `<target>_shrubbery`
file in the working directory. The tests clean up after themselves; `make
fclean` sweeps whatever the demo left behind.
