#!/usr/bin/env bash
# =============================================================================
#  tests/mutants.sh — a mutation-testing drill for ex01
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
SRCS="Bureaucrat.cpp Form.cpp"

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

printf "\n%s%s Sabotage drill — ex01%s\n" "$BOLD" "$CYAN" "$RESET"
printf "%s Each line breaks the code on purpose; DETECTED is the good outcome.%s\n\n" "$DIM" "$RESET"

# ── Form: the grade bounds ──────────────────────────────────────────────────
#   Widens the bound so a form can require the impossible grade 0.
mutate "Form lets sign grade 0 through" Form.cpp \
  's/if (grade < 1)/if (grade < 0)/'
#   Widens the other bound so a form can require grade 151.
mutate "Form lets grade 151 through" Form.cpp \
  's/if (grade > 150)/if (grade > 151)/'
#   Refuses grade 1, which the subject says is legal.
mutate "Form rejects the legal grade 1" Form.cpp \
  's/if (grade < 1)/if (grade < 2)/'
#   Refuses grade 150, likewise legal.
mutate "Form rejects the legal grade 150" Form.cpp \
  's/if (grade > 150)/if (grade > 149)/'
#   Throws the wrong exception type at each bound; both still throw.
mutate "Form swaps its two bound exceptions" Form.cpp \
  's/if (grade < 1) throw GradeTooHighException();/if (grade < 1) throw GradeTooLowException();/; s/if (grade > 150) throw GradeTooLowException();/if (grade > 150) throw GradeTooHighException();/'
#   Skips validation on gradeToExecute, so a form can be built with an
#   out-of-range exec grade.
mutate "only the sign grade is validated" Form.cpp \
  's/_gradeToExecute(validateGrade(gradeToExecute))/_gradeToExecute(gradeToExecute)/'

# ── Form: beSigned, the whole point of ex01 ─────────────────────────────────
#   Turns the rank test into >=, so a bureaucrat at exactly the required grade
#   is refused. The subject says equal is enough.
mutate "beSigned off-by-one (>= instead of >)" Form.cpp \
  's/bureaucrat.getGrade() > _gradeToSign/bureaucrat.getGrade() >= _gradeToSign/'
#   Signs based on gradeToExecute instead of gradeToSign — the wrong field
#   entirely.
mutate "beSigned compares against the exec grade" Form.cpp \
  's/bureaucrat.getGrade() > _gradeToSign/bureaucrat.getGrade() > _gradeToExecute/'
#   Sets _isSigned first, then throws, so a REFUSED form ends up signed
#   anyway.
mutate "beSigned signs before it checks" Form.cpp \
  's|^  if (bureaucrat.getGrade() > _gradeToSign) throw GradeTooLowException();|  _isSigned = true;\n  if (bureaucrat.getGrade() > _gradeToSign) throw GradeTooLowException();|'
#   Removes the assignment: no form can ever be signed.
mutate "beSigned never signs" Form.cpp \
  's/^  _isSigned = true;//'
#   Removes the check: every bureaucrat can sign anything.
mutate "beSigned never refuses" Form.cpp \
  's/if (bureaucrat.getGrade() > _gradeToSign) throw GradeTooLowException();//'
#   Flips > to <, so exactly the wrong bureaucrats are allowed to sign.
mutate "the rank test is inverted" Form.cpp \
  's/bureaucrat.getGrade() > _gradeToSign/bureaucrat.getGrade() < _gradeToSign/'

# ── Form: state and OCF ─────────────────────────────────────────────────────
#   Forms arrive pre-signed instead of unsigned.
mutate "a form is born signed" Form.cpp \
  's/_isSigned(false), _gradeToSign(validateGrade/_isSigned(true), _gradeToSign(validateGrade/'
#   Copying a signed form loses the signature.
mutate "copy ctor forgets the signature" Form.cpp \
  's/_isSigned(other._isSigned), _gradeToSign(other._gradeToSign)/_isSigned(false), _gradeToSign(other._gradeToSign)/'
#   Assignment stops carrying the signed flag.
mutate "operator= assigns nothing" Form.cpp \
  's/if (this != &other) _isSigned = other._isSigned;//'
#   Same as above but via the default constructor path.
mutate "default ctor is born signed" Form.cpp \
  's/Form::Form() : _name("default"), _isSigned(false)/Form::Form() : _name("default"), _isSigned(true)/'

# ── Bureaucrat: bounds and direction (carried over from ex00) ───────────────
#   ex01's own copy of Bureaucrat, upper bound widened.
mutate "Bureaucrat lets grade 0 through" Bureaucrat.cpp \
  's/if (grade < 1)/if (grade < 0)/'
#   ex01's own copy of Bureaucrat, lower bound widened.
mutate "Bureaucrat lets grade 151 through" Bureaucrat.cpp \
  's/if (grade > 150)/if (grade > 151)/'
#   Makes increment ADD instead of subtract. Grade 1 is the highest, so
#   incrementing must lower the number.
mutate "incrementGrade goes the wrong way" Bureaucrat.cpp \
  's/void Bureaucrat::incrementGrade() { _grade = validateGrade(_grade - 1); }/void Bureaucrat::incrementGrade() { _grade = validateGrade(_grade + 1); }/'
#   Gives ++ its own raw --_grade with no bounds check, so it can walk past
#   grade 1.
mutate "prefix ++ bypasses validateGrade" Bureaucrat.cpp \
  's/Bureaucrat& Bureaucrat::operator++() { return (incrementGrade(), \*this); }/Bureaucrat\& Bureaucrat::operator++() { --_grade; return (*this); }/'
#   Increments before taking the snapshot, so b++ returns the value from AFTER
#   instead of before.
mutate "postfix ++ returns the new value" Bureaucrat.cpp \
  's/Bureaucrat Bureaucrat::operator++(int) { Bureaucrat tmp(\*this); incrementGrade(); return (tmp); }/Bureaucrat Bureaucrat::operator++(int) { incrementGrade(); Bureaucrat tmp(*this); return (tmp); }/'

# ── Bureaucrat::signForm — the two sentences the subject fixes ──────────────
#   Prints the failure sentence on success and vice versa.
mutate "signForm swaps its two messages" Bureaucrat.cpp \
  's/" signed "/" couldn'"'"'t sign "/'
#   Never calls beSigned, so it always prints the success line without signing
#   anything.
mutate "signForm reports success on failure" Bureaucrat.cpp \
  's/    form.beSigned(\*this);//'
#   Drops the 'because <reason>.' clause the subject requires.
mutate "signForm swallows the reason" Bureaucrat.cpp \
  's/ << " because " << e.what() << "\."//'
#   Narrows the catch to `int`, so a real exception escapes signForm to the
#   caller instead of being reported.
mutate "signForm lets the exception escape" Bureaucrat.cpp \
  's/  } catch (std::exception& e) {/  } catch (int e) { (void)e;/'

# ── The exception hierarchy ─────────────────────────────────────────────────
# Dropping the base leaves a class that still looks like an exception and is
# still catchable by its own name — only a std::exception& handler notices.
#   Removes the base class; only a std::exception& handler notices.
mutate "Form::GradeTooLow stops deriving from std::exception" Form.hpp \
  's/class GradeTooLowException : public std::exception {/class GradeTooLowException {/'
#   Same, on Bureaucrat's exception.
mutate "Bureaucrat::GradeTooHigh stops deriving from std::exception" Bureaucrat.hpp \
  's/class GradeTooHighException : public std::exception {/class GradeTooHighException {/'
#   Throws `42` rather than an exception object, so no std::exception& handler
#   can catch it.
mutate "beSigned throws a bare int" Form.cpp \
  's/if (bureaucrat.getGrade() > _gradeToSign) throw GradeTooLowException();/if (bureaucrat.getGrade() > _gradeToSign) throw 42;/'
#   Makes Form's two what() strings identical.
mutate "both Form bounds report the same message" Form.cpp \
  's/return ("grade is too low")/return ("grade is too high (1 is the highest)")/'

# ── operator<< ──────────────────────────────────────────────────────────────
#   Prints 'yes' when unsigned and 'no' when signed.
mutate "form operator<< inverts the signed flag" Form.cpp \
  's/(form.isSigned() ? "yes" : "no")/(form.isSigned() ? "no" : "yes")/'
#   Prints gradeToExecute in the sign slot and vice versa. Both numbers still
#   appear, so a test that only greps for them passes.
mutate "form operator<< swaps its two grades" Form.cpp \
  's/", grade to sign: " << form.getGradeToSign() << ", grade to execute: " << form.getGradeToExecute()/", grade to sign: " << form.getGradeToExecute() << ", grade to execute: " << form.getGradeToSign()/'
#   Removes the trailing '.' from the bureaucrat's output line.
mutate "bureaucrat operator<< drops the period" Bureaucrat.cpp \
  's/ << "\."//'

printf "\n  %s%d detected%s · %s%d survived%s\n\n" "$GREEN" "$detected" "$RESET" "$YELLOW" "$survived" "$RESET"
[ "$survived" -eq 0 ]
