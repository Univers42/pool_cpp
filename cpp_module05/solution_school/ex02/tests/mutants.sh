#!/usr/bin/env bash
# =============================================================================
#  tests/mutants.sh — a mutation-testing drill for ex02
# =============================================================================
#
#  1. WHY THIS FILE EXISTS
#  -----------------------
#  `make test` can only tell you the suite PASSES. It cannot tell you the suite
#  is WORTH anything — a tests/test.cpp containing nothing but
#  `int main() { return 0; }` passes too. This script answers the harder
#  question:
#
#        "If I broke the code on purpose, would the tests notice?"
#
#  2. HOW IT WORKS
#  ---------------
#  For every entry in the list at the bottom of this file:
#
#      a. copy all of this exercise's sources into a scratch directory
#         (mktemp -d — your real files are NEVER touched)
#      b. plant ONE deliberate bug in the copy, using `sed`
#      c. rebuild tests/test.cpp against the broken copy
#      d. run it
#
#  The suite SHOULD fail at step (d). If it passes, that bug is invisible to
#  your tests, and that is a hole worth knowing about.
#
#  3. VOCABULARY — a standard technique, not a local invention
#  -----------------------------------------------------------
#      mutant          one deliberately broken copy of the program
#      killed          the suite failed on it   -> GOOD, the tests work
#                      (printed below as [DETECTED])
#      survived        the suite still passed   -> BAD, a blind spot
#      mutation score  killed / total. The target in this repo is 100%.
#      equivalent      a mutant whose change cannot alter behaviour, so nothing
#        mutant        can ever kill it (`i = i + 0`). This is the known
#                      false-positive class of the technique.
#
#  CAREFUL — THIS FILE IS NOT ITSELF A MUTANT.
#  This file is the DRIVER (the harness). A mutant is a temporary broken COPY
#  of the program that this script creates, uses, and throws away. One mutant
#  exists per entry in the list at the bottom, and each lives only for the few
#  seconds it takes to compile and run it. Nothing in your repository is ever
#  modified:
#
#      your real sources
#             |  cp
#             v
#      /tmp/xxxxx/src        <- a throwaway copy
#             |  sed  (plant exactly one bug)
#             v
#      >>> THE MUTANT <<<  + tests/test.cpp  --> compile --> run
#                                                              |
#                                                  killed? or survived?
#
#  So: 123 mutants across this module means 123 short-lived broken copies,
#  not 123 files on disk.
#
#  4. BY HAND, BY SCRIPT, OR BY TOOL?
#  ----------------------------------
#  All three exist. They are the same idea at three levels of automation:
#
#    BY HAND    Every developer already does the informal version: comment out
#               a line, re-run the tests, see whether anything goes red. That
#               is one mutant, evaluated by hand. Very common, rarely called
#               by this name — and not repeatable, so it is forgotten the
#               moment you move on.
#
#    BY SCRIPT  What this file is. The LIST of bugs is hand-picked, but running
#               them is automated, repeatable and CI-friendly. Commonly called
#               a "poor man's" mutation tester. Building one is a normal thing
#               to do when no mature tool fits the project — which is the case
#               here: C++98, no external dependencies allowed, hand-rolled
#               Makefile.
#
#    BY TOOL    The industrial norm. The tools below parse the code (syntax
#               tree or LLVM IR) and generate every mutant a set of operators
#               allows — typically thousands — with no list to maintain:
#
#                 PIT / pitest .......... Java
#                 Stryker ............... JavaScript, TypeScript, C#, Scala
#                 mutmut, cosmic-ray .... Python
#                 cargo-mutants ......... Rust
#                 mull .................. C / C++ (mutates the LLVM IR)
#
#  In short: hand-PICKING which bugs to plant is normal on a small project;
#  hand-RUNNING them is the part worth automating, and that is all this script
#  does. On a real codebase, reach for a tool from the last group instead.
#  Section 7 lists what the `sed` shortcut costs.
#
#  5. READING THE OUTPUT
#  ---------------------
#      [DETECTED] <label>   <reason>
#              The good case. <reason> is the first "FAIL:" line the suite
#              printed — the exact check that noticed — or "compile error" if
#              the broken code would not even build.
#
#      [survived] <label>   the suite did not notice
#              A hole. Add a check to tests/test.cpp that would have failed,
#              then re-run until it reads [DETECTED].
#
#      [STALE]    <label>   pattern no longer matches
#              The sed pattern matched nothing, so NOTHING was mutated and the
#              resulting "pass" would be meaningless. The source was renamed or
#              reformatted since the mutant was written — fix the pattern.
#              Deliberately counted as a survivor so it can never masquerade as
#              a success.
#
#  The script exits 0 only when zero mutants survived, so it is CI-friendly.
#
#  6. ADDING YOUR OWN MUTANT
#  -------------------------
#      mutate "<short label>" <TargetFile.cpp> '<sed expression>'
#
#  The third argument is ordinary `sed -i` script text, usually
#  's/original/replacement/'. Two rules of thumb learned the hard way here:
#
#    * Prefer a mutant that still COMPILES. One caught by the compiler only
#      proves the compiler was paying attention, not your suite. Deleting a
#      value often orphans a variable and trips -Werror=unused-*; keeping that
#      variable referenced forces the SUITE to be the thing that catches it.
#
#    * Prefer a mutant that is wrong somewhere nobody would think to look —
#      "wrong only at grade 100" rather than "wrong at grade 1". Those are the
#      ones that prove an exhaustive sweep earns its keep instead of a spot
#      check that happens to pass.
#
#  7. LIMITATIONS OF THIS sed-BASED APPROACH
#  -----------------------------------------
#    * Patterns are literal text. Reformat the source and a mutant silently
#      stops mutating anything — which is exactly why [STALE] exists.
#    * Only the mutants written below are ever tried; coverage of the mutation
#      space is whatever we thought to list.
#    * "compile error" counts as detected, a weaker signal than a test failing.
#    * Every mutant costs a full rebuild, so a whole run takes a while.
#
#  8. REFERENCES — this is a documented technique, not a local habit
#  ----------------------------------------------------------------
#  ORIGINS (proposed independently, twice, in consecutive years):
#
#    R. G. Hamlet, "Testing Programs with the Aid of a Compiler",
#      IEEE Transactions on Software Engineering, SE-3(4):279-290, 1977.
#
#    R. A. DeMillo, R. J. Lipton and F. G. Sayward,
#      "Hints on Test Data Selection: Help for the Practicing Programmer",
#      IEEE Computer, 11(4):34-41, April 1978.
#
#  The DeMillo-Lipton-Sayward paper is the one usually cited as the origin,
#  and it is also where the two assumptions the whole technique rests on come
#  from. They are worth knowing, because they are the answer to "why would
#  planting a one-character bug prove anything?":
#
#      COMPETENT PROGRAMMER HYPOTHESIS — programs written by competent people
#      are already NEARLY correct, so real defects tend to be small deviations
#      from a correct program rather than wholesale nonsense. That is why a
#      single swapped operator or off-by-one is a realistic bug to simulate.
#
#      COUPLING EFFECT — a test suite that detects all the simple faults in a
#      program also tends to detect the complex ones. That is why killing
#      small mutants is evidence about the suite in general, and not just
#      evidence about those particular typos.
#
#    T. A. Budd, "Mutation Analysis of Program Test Data",
#      PhD thesis, Yale University, 1980.
#
#  SURVEY (the standard reference if you want the whole field in one place):
#
#    Y. Jia and M. Harman, "An Analysis and Survey of the Development of
#      Mutation Testing", IEEE Transactions on Software Engineering,
#      37(5):649-678, 2011.
#
#  INDUSTRIAL USE (evidence it is not only an academic exercise):
#
#    G. Petrovic and M. Ivankovic, "State of Mutation Testing at Google",
#      ICSE-SEIP 2018 — mutation testing applied to Google's monorepo,
#      surfaced during code review rather than as a separate run.
# =============================================================================

set -u   # abort if an unset variable is ever expanded, so a typo in a variable
         # name fails loudly instead of silently becoming an empty string.

# ANSI colour codes. $'...' is the bash quoting form that interprets \033.
RESET=$'\033[0m'; BOLD=$'\033[1m'; DIM=$'\033[2m'
GREEN=$'\033[92m'; YELLOW=$'\033[93m'; CYAN=$'\033[96m'

# HERE = the exercise directory. This script lives in <exercise>/tests/, so the
# exercise itself is one level up. cd+pwd turns it into an absolute path.
HERE="$(cd "$(dirname "$0")/.." && pwd)"
# WORK = a fresh scratch directory; every destructive thing happens inside it.
WORK="$(mktemp -d)"
# Remove the scratch directory when the script exits, however it exits
# (normally, via error, or via Ctrl-C).
trap 'rm -rf "$WORK"' EXIT

# The sources to compile for each mutant. tests/test.cpp supplies its own
# main(), so main.cpp is deliberately NOT in this list.
SRCS="AForm.cpp Bureaucrat.cpp ShrubberyCreationForm.cpp RobotomyRequestForm.cpp PresidentialPardonForm.cpp"

detected=0   # running tally of mutants the suite killed
survived=0   # ... and of the ones it missed

# mutate <label> <file-to-break> <sed-expression>
mutate() {
  local label="$1" file="$2" expr="$3"

  # --- step a: a pristine copy of the exercise in the scratch directory ------
  rm -rf "$WORK/src"
  mkdir -p "$WORK/src/tests"
  cp "$HERE"/*.cpp "$HERE"/*.hpp "$WORK/src/" 2>/dev/null
  cp "$HERE"/tests/test.cpp "$WORK/src/tests/" 2>/dev/null

  # --- step b: plant the bug. `sed -i` edits the COPY in place ---------------
  if ! sed -i "$expr" "$WORK/src/$file"; then
    printf "  %s[skipped]%s %-52s %ssed failed%s\n" "$YELLOW" "$RESET" "$label" "$DIM" "$RESET"
    return
  fi

  # Guard against a silent false success: if the mutated copy is byte-identical
  # to the original then the pattern matched nothing, the code is not actually
  # broken, and the suite would "pass" for the wrong reason entirely.
  if cmp -s "$WORK/src/$file" "$HERE/$file"; then
    printf "  %s[STALE]%s   %-52s %spattern no longer matches%s\n" "$YELLOW" "$RESET" "$label" "$DIM" "$RESET"
    survived=$((survived + 1))
    return
  fi

  # Build the object list: every source, taken from the mutated copy.
  local objs=""
  for s in $SRCS; do objs="$objs $WORK/src/$s"; done

  # --- step c: rebuild the suite against the broken sources -----------------
  # Same flags as the Makefile. If the mutant will not compile, the bug was
  # still caught — by the compiler rather than by a test, which is a weaker
  # but still valid result.
  local out
  if ! out=$(c++ -std=c++98 -Wall -Wextra -Werror -I"$WORK/src" \
                 $objs "$WORK/src/tests/test.cpp" -o "$WORK/mutant" 2>&1); then
    printf "  %s[DETECTED]%s %-52s %scompile error%s\n" "$GREEN" "$RESET" "$label" "$DIM" "$RESET"
    detected=$((detected + 1))
    return
  fi

  # --- step d: run it -------------------------------------------------------
  # A ZERO exit status means the suite still passed, i.e. the mutant SURVIVED.
  # Running from "$WORK" keeps any <target>_shrubbery files the forms write in
  # the scratch directory instead of scattering them across the repo.
  if out=$(cd "$WORK" && ./mutant 2>&1); then
    printf "  %s[survived]%s %-52s %sthe suite did not notice%s\n" "$YELLOW" "$RESET" "$label" "$DIM" "$RESET"
    survived=$((survived + 1))
  else
    # Non-zero: the suite failed, so the mutant was killed. Lift the first
    # "FAIL:" line out of its output so the report names the check that caught
    # it — that is the single most useful piece of information here.
    local first
    first=$(printf '%s\n' "$out" | grep -m1 'FAIL:' | sed 's/.*FAIL: //')
    printf "  %s[DETECTED]%s %-52s %s%s%s\n" "$GREEN" "$RESET" "$label" "$DIM" "${first:-non-zero exit}" "$RESET"
    detected=$((detected + 1))
  fi
}

printf "\n%s%s Sabotage drill — ex02%s\n" "$BOLD" "$CYAN" "$RESET"
printf "%s Each line breaks the code on purpose; DETECTED is the good outcome.%s\n\n" "$DIM" "$RESET"

# ── The execute() gate — the heart of ex02 ──────────────────────────────────
#   Deletes the first precondition, so an unsigned form executes.
mutate "execute skips the signed check" AForm.cpp \
  's/if (!_isSigned) throw NotSignedException();//'
#   Deletes the second precondition, so anyone can execute any signed form.
mutate "execute skips the grade check" AForm.cpp \
  's/if (executor.getGrade() > _gradeToExecute) throw GradeTooLowException();//'
#   Both checks remain but in the wrong order, so an unsigned form reports the
#   wrong reason.
mutate "execute checks grade before signed" AForm.cpp \
  's/^  if (!_isSigned) throw NotSignedException();//; s/^  if (executor.getGrade() > _gradeToExecute) throw GradeTooLowException();/  if (executor.getGrade() > _gradeToExecute) throw GradeTooLowException();\n  if (!_isSigned) throw NotSignedException();/'
#   An executor at exactly gradeToExecute is refused, though the subject says
#   that grade suffices.
mutate "execute off-by-one (>= instead of >)" AForm.cpp \
  's/executor.getGrade() >= _gradeToExecute/XX/; s/executor.getGrade() > _gradeToExecute/executor.getGrade() >= _gradeToExecute/'
#   Uses _gradeToSign where _gradeToExecute is meant — the wrong field.
mutate "execute gates on the sign grade" AForm.cpp \
  's/executor.getGrade() > _gradeToExecute/executor.getGrade() > _gradeToSign/'
#   Runs both checks then returns without invoking executeAction(), so nothing
#   ever happens.
mutate "execute never calls the action" AForm.cpp \
  's/  executeAction(executor);//'

# ── AForm: signing and validation (carried over from ex01) ──────────────────
#   Turns the rank test into >=, so a bureaucrat at exactly the required grade
#   is refused. The subject says equal is enough.
mutate "beSigned off-by-one (>= instead of >)" AForm.cpp \
  's/bureaucrat.getGrade() > _gradeToSign/bureaucrat.getGrade() >= _gradeToSign/'
#   Sets _isSigned first, then throws, so a REFUSED form ends up signed
#   anyway.
mutate "beSigned signs before it checks" AForm.cpp \
  's|^  if (bureaucrat.getGrade() > _gradeToSign) throw GradeTooLowException();|  _isSigned = true;\n  if (bureaucrat.getGrade() > _gradeToSign) throw GradeTooLowException();|'
#   Forms arrive pre-signed instead of unsigned.
mutate "a form is born signed" AForm.cpp \
  's/_isSigned(false), _gradeToSign(validateGrade/_isSigned(true), _gradeToSign(validateGrade/'
#   Copying a signed form loses the signature.
mutate "copy ctor forgets the signature" AForm.cpp \
  's/_isSigned(other._isSigned), _gradeToSign(other._gradeToSign)/_isSigned(false), _gradeToSign(other._gradeToSign)/'
#   Assignment stops carrying the signed flag.
mutate "operator= assigns nothing" AForm.cpp \
  's/if (this != &other) _isSigned = other._isSigned;//'

# ── The concrete forms: wrong grades, wrong actions ─────────────────────────
#   Gives ShrubberyCreationForm 137/145 instead of the subject's 145/137.
mutate "shrubbery grades swapped" ShrubberyCreationForm.cpp \
  's/AForm("ShrubberyCreationForm", 145, 137)/AForm("ShrubberyCreationForm", 137, 145)/g'
#   Changes the robotomy exec grade from 45 to 46.
mutate "robotomy exec grade wrong" RobotomyRequestForm.cpp \
  's/AForm("RobotomyRequestForm", 72, 45)/AForm("RobotomyRequestForm", 72, 46)/g'
#   Changes the pardon sign grade from 25 to 26.
mutate "pardon sign grade wrong" PresidentialPardonForm.cpp \
  's/AForm("PresidentialPardonForm", 25, 5)/AForm("PresidentialPardonForm", 26, 5)/g'
#   Changes the pardon message so it no longer names Zaphod Beeblebrox.
mutate "pardon forgets Zaphod" PresidentialPardonForm.cpp \
  's/ has been pardoned by Zaphod Beeblebrox/ was pardoned/'
#   Removes the '* drilling noises *' line the subject requires.
mutate "robotomy never drills" RobotomyRequestForm.cpp \
  's/std::cout << "\* drilling noises \*" << std::endl;//'
#   Replaces the 50/50 coin flip with a constant, so the robotomy never fails.
mutate "robotomy always succeeds" RobotomyRequestForm.cpp \
  's/if (std::rand() % 2)/if (1)/'
#   Removes the loop that writes the ASCII trees, leaving an empty file.
mutate "shrubbery writes no file" ShrubberyCreationForm.cpp \
  's/for (int i = 0; i < 3; ++i) ofs << tree << "\\n";//'
#   Calls AForm() instead of AForm(other), so a copied form silently resets
#   its name, grades and signature.
mutate "copy ctor drops the base sub-object" RobotomyRequestForm.cpp \
  's/: AForm(other), _target(other._target)/: AForm(), _target(other._target)/'
#   Copies _target but not the base half, so assignment loses the signed flag.
mutate "derived operator= skips the base" RobotomyRequestForm.cpp \
  's/AForm::operator=(other); _target = other._target;/_target = other._target;/'

# ── Bureaucrat: the handshake ───────────────────────────────────────────────
#   Prints 'couldn't execute' on success and vice versa.
mutate "executeForm swaps its two messages" Bureaucrat.cpp \
  's/" executed "/" couldn'"'"'t execute "/'
#   Never calls execute(), so it always prints the success line.
mutate "executeForm reports success on failure" Bureaucrat.cpp \
  's/    form.execute(\*this);//'
#   Prints the failure sentence on success and vice versa.
mutate "signForm swaps its two messages" Bureaucrat.cpp \
  's/" signed "/" couldn'"'"'t sign "/'

# ── The OCF default constructors ────────────────────────────────────────────
# Required by the subject but called by nothing else, so without their own
# mutants the checks covering them could be passing vacuously.
#   Puts 72/46 in RobotomyRequestForm's default constructor. Nothing else
#   calls it, so only a test aimed at it notices.
mutate "default ctor grades wrong" RobotomyRequestForm.cpp \
  's/AForm("RobotomyRequestForm", 72, 45), _target("default")/AForm("RobotomyRequestForm", 72, 46), _target("default")/'
#   Gives the default-constructed pardon a truncated class name.
mutate "default ctor name wrong" PresidentialPardonForm.cpp \
  's/AForm("PresidentialPardonForm", 25, 5), _target("default")/AForm("Presidential", 25, 5), _target("default")/'
#   Puts 146 in the shrubbery default constructor's sign grade.
mutate "default ctor sign grade wrong" ShrubberyCreationForm.cpp \
  's/ShrubberyCreationForm::ShrubberyCreationForm() : AForm("ShrubberyCreationForm", 145, 137)/ShrubberyCreationForm::ShrubberyCreationForm() : AForm("ShrubberyCreationForm", 146, 137)/'

# ── The exception hierarchy ─────────────────────────────────────────────────
# Dropping the base leaves a class that still looks like an exception and is
# still catchable by its own name — only a std::exception& handler notices.
#   Removes the base class from the third exception type.
mutate "AForm::NotSigned stops deriving from std::exception" AForm.hpp \
  's/class NotSignedException : public std::exception {/class NotSignedException {/'
#   Removes the base class from AForm's TooLow.
mutate "AForm::GradeTooLow stops deriving from std::exception" AForm.hpp \
  's/class GradeTooLowException : public std::exception {/class GradeTooLowException {/'
#   Same, on Bureaucrat's exception.
mutate "Bureaucrat::GradeTooHigh stops deriving from std::exception" Bureaucrat.hpp \
  's/class GradeTooHighException : public std::exception {/class GradeTooHighException {/'
#   Throws `42` instead of NotSignedException. The compile-time upcast check
#   still passes, so only the per-site runtime checks catch this.
mutate "execute() throws a bare int when unsigned" AForm.cpp \
  's/if (!_isSigned) throw NotSignedException();/if (!_isSigned) throw 42;/'

printf "\n  %s%d detected%s · %s%d survived%s\n\n" "$GREEN" "$detected" "$RESET" "$YELLOW" "$survived" "$RESET"
[ "$survived" -eq 0 ]
