# ex02 — No, you need form 28B, not 28C...

## Concept

Abstract base classes combined with exceptions. `Form` from ex01 becomes
**`AForm`**, an abstract class: it declares a pure virtual method, so it can
no longer be instantiated — only its concrete children
(`ShrubberyCreationForm`, `RobotomyRequestForm`, `PresidentialPardonForm`)
can. The exercise is the *Template Method* pattern in miniature: the base
class owns the invariant checks, the derived classes own only the varying
behavior.

## How this code demonstrates it

`AForm::execute(Bureaucrat const&)` is a non-virtual public method that does
the two checks every form shares — the form must be signed, and the
executor's grade must be at or above `getGradeToExecute()` — throwing
`NotSignedException` or `GradeTooLowException` otherwise. Only when both
checks pass does it call the protected pure virtual hook
`executeAction(executor)`, which each concrete form implements:

| Form                     | sign | exec | action |
|--------------------------|-----:|-----:|--------|
| `ShrubberyCreationForm`  |  145 |  137 | writes ASCII trees to `<target>_shrubbery` |
| `RobotomyRequestForm`    |   72 |   45 | drilling noises, 50% success (`rand() % 2`) |
| `PresidentialPardonForm` |   25 |    5 | `<target>` pardoned by Zaphod Beeblebrox |

This is the "more elegant" option the subject hints at: checks live in one
place instead of being copy-pasted into three `execute` overrides, and a
derived class *cannot* forget them because the hook is only reachable through
`AForm::execute`.

Key design decisions:

- **Attributes stay private in `AForm`** (name, signed flag, both grades);
  children go through the public getters, per the subject.
- `Bureaucrat::executeForm(AForm const&)` catches any exception from
  `execute()` and prints `<name> couldn't execute <form> because <reason>.`,
  so the demo never terminates on an expected failure; on success it prints
  `<name> executed <form>`.
- Exception classes (`GradeTooHighException`, `GradeTooLowException`,
  `NotSignedException`) derive from `std::exception` and skip Orthodox
  Canonical Form, as the module allows; every other class is canonical
  (concrete forms keep a default constructor with target `"default"` on top
  of the subject's one-argument constructor).
- Randomness uses `std::rand()` seeded once — `<random>` is C++11 and this
  project is strict C++98.

## Build / run / test

```sh
make            # builds to build/bin/ex02
make run        # build + run the demo (creates home_shrubbery in this dir)
make test       # builds tests/test.cpp and runs it; non-zero exit on failure
make fclean     # remove all build artifacts
```

The test program (`tests/test.cpp`) asserts the required grades of all three
forms, that executing an unsigned form throws, that signing/executing below
the required grade throws, that the shrubbery file is actually created (and
deletes it afterwards), and captures stdout to verify the pardon and
robotomy messages.
