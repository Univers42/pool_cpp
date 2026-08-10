# Module 05 — Repetition and Exceptions

The arc of this module is C++ exceptions, from one class defending its own
invariant to a hierarchy where exceptions, abstraction, and a factory work
together. But the lesson I actually remember is smaller and more humbling:
in this bureaucracy, **grade 1 is the highest and 150 the lowest**, and that
single inversion cost me a real shipped bug. Everything here is strict C++98
(`-std=c++98 -Wall -Wextra -Werror`), Orthodox Canonical Form on every class
(exception classes exempt, as the subject allows), and custom exceptions
deriving from `std::exception` with `what() const throw()`. There was also a
housekeeping episode I'd rather not repeat: at one point my exercise folders
had drifted out of order relative to the subject, and I had to renumber them
so ex00 through ex03 actually matched what the PDF asks. Boring work, but a
repo that lies about its own structure fails a defense before a single line
of code is read.

## ex00 — Bureaucrat

The exercise wants a class that cannot exist in an invalid state: a `const`
name and a grade in [1, 150], where every constructor, increment, and
decrement throws `GradeTooHighException` or `GradeTooLowException` rather
than clamp or ignore. My `Bureaucrat.cpp` funnels every check through one
private `validateGrade(int)` — `grade < 1` throws TooHigh, `grade > 150`
throws TooLow — so the invariant has a single source of truth.

Here is the bug I shipped: my first version threw `GradeTooLowException`
for grades that were too *high*, and the `what()` messages were backwards
too. Reading the code aloud, it sounded right — "grade less than one is too
low" — and that is precisely the trap: in this domain, a smaller number is a
higher rank. It took a failing boundary test to see it. The fix left scars I
kept on purpose: `incrementGrade()` does `--_grade`, `decrementGrade()` does
`++_grade`, the `what()` strings spell out the direction ("Grade too high
(highest is 1)"), and the header carries the invariant as a comment so the
next reader inverts once, at the top, instead of at every call site.

Two more deliberate choices. Mutators validate the *prospective* grade before
touching `_grade` — `validateGrade(_grade - 1); --_grade;` — which is the
strong exception guarantee in its smallest possible form: a failed mutation
leaves the object exactly as it was. And the exception specifications are
C++98 `throw()`; I had written `noexcept` on reflex, and it had to go —
`-std=c++98` does not negotiate. The stream operator prints the subject's
exact format, `<name>, bureaucrat grade <grade>.`, and yes, the period is
load-bearing: the tests compare output byte-for-byte, because graders do.

## ex01 — Form

Two classes now negotiate through exceptions. `Form::beSigned()` owns the
invariant and throws `GradeTooLowException` when `bureaucrat.getGrade() >
_gradeToSign` (equal grade suffices); `Bureaucrat::signForm()` catches it
and turns it into a sentence — `<name> signed <form>` or `<name> couldn't
sign <form> because <reason>.` — and never lets it propagate, because a
refused signature is news, not a crash. That split — the invariant's owner
throws, the caller decides — is the whole exercise.

The mechanical annoyance was the circular dependency: each header needs the
other's type, so both use a forward declaration and include the full header
only in the `.cpp`. And since the form's name and both grades are `const`,
copy assignment can honestly copy only `_isSigned` — the compiler enforces
what the subject calls "constant attributes" better than any comment could.

## ex02 — AForm and the elegant option

`Form` becomes abstract `AForm`, and the subject drops a hint that one design
is "more elegant" than the other. It is, and it has a name: Template Method.
`AForm::execute()` is a non-virtual public method that performs the two
checks every form shares — throw `NotSignedException` if unsigned, throw
`GradeTooLowException` if the executor's grade is above `_gradeToExecute` —
and only then calls the protected pure virtual hook `executeAction()`. The
checks live in one place, and a derived form *cannot* forget them, because
the hook is unreachable except through `execute()`.

The three concrete forms fill in the hook: `ShrubberyCreationForm` (sign 145,
exec 137) writes three ASCII trees to `<target>_shrubbery`;
`RobotomyRequestForm` (72/45) makes drilling noises and succeeds on
`std::rand() % 2`; `PresidentialPardonForm` (25/5) invokes Zaphod Beeblebrox.
One decision I stand by: if the shrubbery file cannot be opened, the form
throws `std::runtime_error("could not open " + filename)` — a form that
cannot do its job should fail through the same channel as every other
failure, and `Bureaucrat::executeForm()` catches it and reports `<name>
couldn't execute <form> because <reason>.` like anything else. The tests are
pleasantly petty: they assert that grade 138 cannot execute a form requiring
137, and that `test_target_shrubbery` really appears on disk.

## ex03 — Intern

The closing factory: `Intern::makeForm(name, target)` maps a runtime string
to a concrete form returned as `AForm*`. The subject explicitly refuses the
if/else forest, which forces the shape I should have reached for anyway: a
static table of `{const char*, AForm* (*)(const std::string&)}` entries in
`Intern.cpp`, walked in a loop, with `sizeof(table) / sizeof(table[0])` as
the row count. Adding a fourth form is one row and one four-line creator
function — no new control flow. In C++98 there is no `std::function` and no
lambda, so these are plain function pointers, and honestly the constraint
improved the code. On a match it prints `Intern creates <form>`; on a miss it
prints an error and returns `NULL`, which the demo exercises with a "coffee
making" request. The trade-off is that the caller owns the result and must
`delete` it — a raw-pointer contract I document rather than pretend away,
because smart pointers are years ahead of this standard.

## What stuck with me

Exceptions are not error codes with drama; they are how a class refuses to
exist in a broken state. Validate before you mutate and the strong guarantee
falls out for free. Exact output is part of the contract — the period in
`bureaucrat grade 3.` failed a test before it failed a grader. And the
grade-inversion bug taught me the most general lesson of the module: when
the domain inverts a convention, translate it once, in one function, and
make the comments state the direction — because code that reads correctly
aloud can still be wrong.

## Building and testing

Each exercise builds independently:

```sh
cd exNN          # ex00, ex01, ex02, or ex03
make             # build to build/bin/exNN
make run         # build and run the demo
make test        # build and run tests/ — exits non-zero on failure
make fclean
```

`make test` genuinely runs the suites: boundary grades on both edges, the
four throw paths, byte-for-byte message checks, the shrubbery file's
existence, and the factory's `NULL` path. Both binaries in every exercise
are valgrind-clean.
