# Module 05 — Repetition and Exceptions

## What this module teaches

The arc of Module 05 is **C++ exceptions**, from a single class enforcing its
own invariant to a full class hierarchy where exceptions, abstraction, and a
factory work together:

1. **ex00** — throw to protect one class's invariant.
2. **ex01** — throw/catch between two collaborating classes.
3. **ex02** — an abstract base class centralizes the checks (Template Method);
   derived classes supply only the varying behavior.
4. **ex03** — a factory returns concrete forms through the abstract base
   pointer, so the caller never names a concrete type.

Everything is strict C++98 (`-std=c++98 -Wall -Wextra -Werror`), Orthodox
Canonical Form on all classes (exception classes exempt), and custom
exceptions derive from `std::exception` with `what() const throw()`.

## ex00 — Bureaucrat

A `Bureaucrat` has a `const` name and a grade in [1, 150], where **1 is the
highest grade**. Any operation that would break the range (construction,
increment, decrement) throws `GradeTooHighException` or
`GradeTooLowException` — so an invalid `Bureaucrat` can never exist. All
checks funnel through one private `validateGrade()` helper, and mutators
validate before modifying (strong exception guarantee). The demo exercises
all four throw paths; the tests verify boundary grades, exception types,
that a failed mutation leaves the object unchanged, and the exact
`operator<<` format.

## ex01 — Form

Two classes negotiate through exceptions: `Form::beSigned()` throws
`GradeTooLowException` when the bureaucrat's rank is insufficient, and
`Bureaucrat::signForm()` catches it, printing
`<name> signed <form>` or `<name> couldn't sign <form> because <reason>.`
The owner of the invariant throws; the caller decides how to handle it. The
exercise also solves the circular-dependency problem with forward
declarations in the headers. Tests cover constructor bounds, sign
success/failure, and that `signForm()` never propagates.

## ex02 — AForm and concrete forms

`Form` becomes abstract **`AForm`**. Its non-virtual `execute()` performs the
two checks every form shares (signed? executor grade high enough?), throwing
`NotSignedException` or `GradeTooLowException`, and only then calls the
protected pure virtual hook `executeAction()` — the Template Method pattern.
Three concrete forms implement the hook:

| Form | sign | exec | action |
|---|---:|---:|---|
| `ShrubberyCreationForm` | 145 | 137 | writes ASCII trees to `<target>_shrubbery` |
| `RobotomyRequestForm` | 72 | 45 | 50% success (`std::rand()`) |
| `PresidentialPardonForm` | 25 | 5 | pardon by Zaphod Beeblebrox |

`Bureaucrat::executeForm()` catches failures and reports them. Tests assert
the grade table, that unsigned/under-grade execution throws, that the
shrubbery file is created, and the exact output messages.

## ex03 — Intern

A small **factory**: `Intern::makeForm(name, target)` maps a runtime string
to the right concrete form, returned as `AForm*` (caller must `delete` it).
The subject forbids an `if/else` chain, so the lookup is a data-driven table
of `{name, creator-function-pointer}` pairs — adding a form is one table row.
Unknown names print an error and return `NULL`. The demo follows the
subject's "robotomy request on Bender" example; tests check each name yields
the right class, the target reaches the form, and the `NULL` path.

## Build / run / test

Each exercise builds independently:

```sh
cd exNN          # ex00, ex01, ex02, or ex03
make             # build to build/bin/exNN
make run         # build + run the demo
make test        # build + run tests/ (exits non-zero on failure)
make fclean      # remove build artifacts
```
