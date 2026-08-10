# ex01 — Form up, maggots!

## Concept

Exceptions across collaborating classes. ex00 introduced throwing on invariant
violations inside one class; here two classes negotiate through exceptions:
`Form::beSigned()` *throws* when a bureaucrat's rank is insufficient, and
`Bureaucrat::signForm()` *catches* that exception to turn it into a
user-facing message instead of letting it propagate. This is the core split:
the class that owns the invariant throws, the caller decides how to handle it.

The exercise also forces the two-class dependency problem: `Bureaucrat.hpp`
and `Form.hpp` each need the other's type. Both headers use a forward
declaration (`class Form;` / `class Bureaucrat;`) and include the full header
only in their `.cpp`, avoiding a circular include.

## Design

- `Form` holds a `const` name, a `const` grade to sign, a `const` grade to
  execute, and a mutable `_isSigned` flag (starts `false`). All private.
- Grade rules mirror the Bureaucrat's: valid range [1, 150], **1 is the
  highest rank**. So grade 0 throws `GradeTooHighException` (above the top),
  151 throws `GradeTooLowException` — checked in the constructor.
- `Form::beSigned(const Bureaucrat&)` throws `Form::GradeTooLowException` if
  `bureaucrat.getGrade() > _gradeToSign`; equal grade is enough. On success it
  flips `_isSigned`.
- `Bureaucrat::signForm(Form&)` calls `beSigned()` in a try/catch and prints
  `<name> signed <form>` or `<name> couldn't sign <form> because <reason>.`
  It never propagates the exception — signing failure is not an error for the
  caller.
- Copy assignment on `Form` can only copy `_isSigned`: the name and grades
  are `const`, which is exactly what the subject's "constant attributes"
  requirement implies. Exception classes derive from `std::exception` and
  override `what()` with a C++98 `throw()` specification (no `noexcept` in
  C++98).
- Orthodox Canonical Form on both classes; exception classes are exempt per
  the module rules.

## Build / run / test

```sh
make            # builds build/bin/ex01
make run        # build + run the demo
make test       # builds tests/test.cpp and runs it (non-zero exit on failure)
make fclean     # remove all build artifacts
```

The demo (`main.cpp`) shows a refused signature, a successful one after
`incrementGrade()`, and out-of-bounds form construction. `tests/test.cpp` is a
standalone assert-style program covering constructor bounds, `beSigned()`
success/failure, `signForm()` never throwing, and const-correct assignment.
