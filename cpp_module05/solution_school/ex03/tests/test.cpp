// Standalone C++98 test suite for ex03: the abstract AForm, its three concrete
// forms, the execute() gate, and the Intern that builds forms from a name.
//
// Covers: everything ex01 checked for Form (bounds, exception identity, the
// beSigned rank rule, OCF under const members), what ex02 adds — the fixed
// grades of the three forms, the two execute() preconditions and their order,
// polymorphic dispatch and destruction through AForm*, each form's actual
// effect, and the two sentences executeForm() must print — plus what ex03 adds:
// makeForm() mapping each known name to the right concrete type with the target
// wired through, and failing loudly on an unknown one.
//
// Every check is written so that a *wrong* implementation fails it — see
// tests/mutants.sh. Exits non-zero on any failure.

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "AForm.hpp"
#include "Bureaucrat.hpp"
#include "Intern.hpp"
#include "PresidentialPardonForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "ShrubberyCreationForm.hpp"

static int g_failures = 0;
static int g_checks = 0;

static void check(bool ok, const char* what) { ++g_checks; if (!ok) { ++g_failures; std::cout << "    FAIL: " << what << std::endl; } }

// Runs a call with std::cout redirected into a string, so the subject's exact
// sentences can be asserted instead of eyeballed, and so the concrete forms'
// own chatter never pollutes the test output.
class Capture {
 public:
  Capture() : _saved(std::cout.rdbuf(_sink.rdbuf())) {}
  ~Capture() { std::cout.rdbuf(_saved); }
  std::string str() const { return (_sink.str()); }

 private:
  std::ostringstream _sink;
  std::streambuf* _saved;
  Capture(const Capture&);
  Capture& operator=(const Capture&);
};

static bool fileExists(const std::string& path) {
  std::ifstream ifs(path.c_str());
  return (ifs.is_open());
}

// ── The three forms carry the grades the subject fixes ──────────────────────
static void test_fixed_grades() {
  ShrubberyCreationForm shrub("home");
  check(shrub.getName() == "ShrubberyCreationForm", "shrubbery is named after its class");
  check(shrub.getGradeToSign() == 145, "shrubbery signs at 145");
  check(shrub.getGradeToExecute() == 137, "shrubbery executes at 137");

  RobotomyRequestForm robot("Bender");
  check(robot.getName() == "RobotomyRequestForm", "robotomy is named after its class");
  check(robot.getGradeToSign() == 72, "robotomy signs at 72");
  check(robot.getGradeToExecute() == 45, "robotomy executes at 45");

  PresidentialPardonForm pardon("Arthur");
  check(pardon.getName() == "PresidentialPardonForm", "pardon is named after its class");
  check(pardon.getGradeToSign() == 25, "pardon signs at 25");
  check(pardon.getGradeToExecute() == 5, "pardon executes at 5");

  // All three start unsigned, like any form.
  check(!shrub.isSigned() && !robot.isSigned() && !pardon.isSigned(), "every form starts unsigned");
}

// ── beSigned still obeys the ex01 rank rule ─────────────────────────────────
static void test_be_signed() {
  // Refused, and the form is left exactly as it was.
  {
    RobotomyRequestForm f("X");
    Bureaucrat low("Low", 73);
    try {
      f.beSigned(low);
      check(false, "beSigned below the sign grade must throw");
    } catch (AForm::GradeTooLowException&) {
    }
    check(!f.isSigned(), "a refused beSigned leaves the form unsigned");
  }

  // Equal grade is enough.
  {
    RobotomyRequestForm f("X");
    Bureaucrat exact("Exact", 72);
    f.beSigned(exact);
    check(f.isSigned(), "equal grade signs the form");
  }

  // The exact boundary for each of the three, swept.
  const int signGrades[3] = {145, 72, 25};
  bool boundaryHolds = true;
  for (int which = 0; which < 3; ++which) {
    for (int g = 1; g <= 150; ++g) {
      ShrubberyCreationForm shrub("t");
      RobotomyRequestForm robot("t");
      PresidentialPardonForm pardon("t");
      AForm* f = which == 0 ? static_cast<AForm*>(&shrub)
               : which == 1 ? static_cast<AForm*>(&robot)
                            : static_cast<AForm*>(&pardon);
      Bureaucrat b("B", g);
      try {
        f->beSigned(b);
        if (g > signGrades[which] || !f->isSigned()) boundaryHolds = false;
      } catch (AForm::GradeTooLowException&) {
        if (g <= signGrades[which] || f->isSigned()) boundaryHolds = false;
      }
    }
  }
  check(boundaryHolds, "each form's sign boundary is exact and inclusive");
}

// ── execute(): both preconditions, and their order ──────────────────────────
static void test_execute_gate() {
  // Unsigned: NotSignedException, even for the highest possible grade.
  {
    PresidentialPardonForm f("X");
    Bureaucrat boss("Boss", 1);
    try {
      Capture mute;
      f.execute(boss);
      mute.str();
      check(false, "executing an unsigned form must throw");
    } catch (AForm::NotSignedException&) {
    }
  }

  // Signed but the executor is too junior: GradeTooLowException.
  {
    PresidentialPardonForm f("X");
    Bureaucrat boss("Boss", 1);
    f.beSigned(boss);
    Bureaucrat junior("Junior", 6);
    try {
      Capture mute;
      f.execute(junior);
      mute.str();
      check(false, "executing above the exec grade must throw");
    } catch (AForm::GradeTooLowException&) {
    }
  }

  // The signed check comes first: an unsigned form reports "not signed" even
  // when the executor would also have been too junior.
  {
    PresidentialPardonForm f("X");
    Bureaucrat junior("Junior", 150);
    bool rightOrder = false;
    try {
      Capture mute;
      f.execute(junior);
      mute.str();
    } catch (AForm::NotSignedException&) {
      rightOrder = true;
    } catch (AForm::GradeTooLowException&) {
    }
    check(rightOrder, "the signed check runs before the grade check");
  }

  // Equal grade executes.
  {
    PresidentialPardonForm f("X");
    Bureaucrat boss("Boss", 1);
    f.beSigned(boss);
    Bureaucrat exact("Exact", 5);
    bool threw = false;
    {
      Capture mute;
      try {
        f.execute(exact);
      } catch (std::exception&) {
        threw = true;
      }
      mute.str();
    }
    check(!threw, "an executor at exactly the exec grade succeeds");
  }

  // The exec boundary, swept for each form.
  const int execGrades[3] = {137, 45, 5};
  bool boundaryHolds = true;
  for (int which = 0; which < 3; ++which) {
    for (int g = 1; g <= 150; ++g) {
      ShrubberyCreationForm shrub("tests_tmp");
      RobotomyRequestForm robot("t");
      PresidentialPardonForm pardon("t");
      AForm* f = which == 0 ? static_cast<AForm*>(&shrub)
               : which == 1 ? static_cast<AForm*>(&robot)
                            : static_cast<AForm*>(&pardon);
      Bureaucrat signer("Signer", 1);
      f->beSigned(signer);
      Bureaucrat b("B", g);
      Capture mute;
      try {
        f->execute(b);
        if (g > execGrades[which]) boundaryHolds = false;
      } catch (AForm::GradeTooLowException&) {
        if (g <= execGrades[which]) boundaryHolds = false;
      } catch (std::exception&) {
        boundaryHolds = false;
      }
      mute.str();
    }
  }
  std::remove("tests_tmp_shrubbery");
  check(boundaryHolds, "each form's exec boundary is exact and inclusive");

  // Signing does not execute, and executing does not un-sign.
  {
    RobotomyRequestForm f("X");
    Bureaucrat boss("Boss", 1);
    f.beSigned(boss);
    {
      Capture mute;
      f.execute(boss);
      mute.str();
    }
    check(f.isSigned(), "executing leaves the form signed");
  }
}

// ── What each concrete form actually does ───────────────────────────────────
static void test_actions() {
  Bureaucrat boss("Boss", 1);

  // PresidentialPardonForm names the target and Zaphod Beeblebrox.
  {
    PresidentialPardonForm f("Arthur");
    f.beSigned(boss);
    std::string out;
    {
      Capture cap;
      f.execute(boss);
      out = cap.str();
    }
    check(out.find("Arthur") != std::string::npos, "pardon names the target");
    check(out.find("Zaphod Beeblebrox") != std::string::npos, "pardon names Zaphod Beeblebrox");
  }

  // RobotomyRequestForm drills, and lands on one of exactly two outcomes.
  {
    RobotomyRequestForm f("Bender");
    f.beSigned(boss);
    bool sawSuccess = false, sawFailure = false, alwaysDrilled = true;
    for (int i = 0; i < 60; ++i) {
      std::string out;
      {
        Capture cap;
        f.execute(boss);
        out = cap.str();
      }
      if (out.find("drilling") == std::string::npos) alwaysDrilled = false;
      if (out.find("robotomized successfully") != std::string::npos) sawSuccess = true;
      else if (out.find("failed") != std::string::npos) sawFailure = true;
      else alwaysDrilled = false;
    }
    check(alwaysDrilled, "robotomy always makes drilling noises");
    // 60 flips: seeing only one outcome is a 2^-59 coincidence, or a bug.
    check(sawSuccess && sawFailure, "robotomy succeeds sometimes and fails sometimes");
  }

  // ShrubberyCreationForm writes <target>_shrubbery and prints nothing.
  {
    const std::string target = "tests_shrub";
    const std::string path = target + "_shrubbery";
    std::remove(path.c_str());
    ShrubberyCreationForm f(target);
    f.beSigned(boss);
    std::string out;
    {
      Capture cap;
      f.execute(boss);
      out = cap.str();
    }
    check(out.empty(), "shrubbery writes a file rather than printing");
    check(fileExists(path), "shrubbery creates <target>_shrubbery");

    std::ifstream ifs(path.c_str());
    std::ostringstream contents;
    contents << ifs.rdbuf();
    ifs.close();
    check(contents.str().size() > 100, "the shrubbery file is not empty");
    std::remove(path.c_str());

    // An unsigned shrubbery must not touch the filesystem at all.
    ShrubberyCreationForm unsignedForm("tests_never");
    std::remove("tests_never_shrubbery");
    try {
      Capture mute;
      unsignedForm.execute(boss);
      mute.str();
    } catch (std::exception&) {
    }
    check(!fileExists("tests_never_shrubbery"), "a refused execute writes no file");
  }
}

// ── Bureaucrat::executeForm, and the two sentences ──────────────────────────
static void test_execute_form() {
  Bureaucrat boss("Boss", 1);

  // Success: "<bureaucrat> executed <form>"
  {
    PresidentialPardonForm f("Arthur");
    f.beSigned(boss);
    std::string out;
    {
      Capture cap;
      boss.executeForm(f);
      out = cap.str();
    }
    check(out.find("Boss executed PresidentialPardonForm") != std::string::npos, "executeForm prints '<name> executed <form>'");
  }

  // Failure: an explicit message naming the reason.
  {
    PresidentialPardonForm f("Arthur");  // unsigned
    std::string out;
    {
      Capture cap;
      boss.executeForm(f);
      out = cap.str();
    }
    check(out.find("Boss couldn't execute PresidentialPardonForm") != std::string::npos, "executeForm prints an explicit failure");
    check(out.find("not signed") != std::string::npos, "the failure names the reason");
    check(out.find("executed PresidentialPardonForm") == std::string::npos || out.find("couldn't") != std::string::npos, "a failure never reads as a success");
  }

  // executeForm reports; it never propagates — not even a form's own throw.
  {
    PresidentialPardonForm f("Arthur");
    Bureaucrat junior("Junior", 150);
    bool escaped = false;
    Capture mute;
    try {
      junior.executeForm(f);
    } catch (std::exception&) {
      escaped = true;
    }
    mute.str();
    check(!escaped, "executeForm never propagates an exception");
  }

  // signForm still behaves as it did in ex01, now taking an AForm.
  {
    RobotomyRequestForm f("X");
    std::string out;
    {
      Capture cap;
      boss.signForm(f);
      out = cap.str();
    }
    check(out == "Boss signed RobotomyRequestForm\n", "signForm prints '<name> signed <form>'");
    check(f.isSigned(), "signForm signs the form");
  }
}

// ── Polymorphism ────────────────────────────────────────────────────────────
static void test_polymorphism() {
  Bureaucrat boss("Boss", 1);

  // The same AForm& produces three different behaviours.
  ShrubberyCreationForm shrub("tests_poly");
  RobotomyRequestForm robot("Bender");
  PresidentialPardonForm pardon("Arthur");
  AForm* forms[3];
  forms[0] = &shrub;
  forms[1] = &robot;
  forms[2] = &pardon;

  std::string outs[3];
  for (int i = 0; i < 3; ++i) {
    forms[i]->beSigned(boss);
    Capture cap;
    forms[i]->execute(boss);
    outs[i] = cap.str();
  }
  std::remove("tests_poly_shrubbery");
  check(outs[1] != outs[2], "different forms produce different output through AForm*");
  check(outs[2].find("Zaphod") != std::string::npos, "the pardon dispatched to its own action");
  check(outs[1].find("drilling") != std::string::npos, "the robotomy dispatched to its own action");

  // operator<< is a free function taking const AForm&, so it works on any of
  // them without knowing the concrete type.
  std::ostringstream os;
  os << *forms[2];
  check(os.str().find("PresidentialPardonForm") != std::string::npos, "operator<< works through the base reference");

  // Deleting through an AForm* must run the concrete destructor. Valgrind is
  // the real judge (make test runs clean under it); here we at least prove the
  // call is dispatched and the object is usable right up to delete.
  AForm* heap = new RobotomyRequestForm("HeapBot");
  heap->beSigned(boss);
  {
    Capture mute;
    heap->execute(boss);
    mute.str();
  }
  check(heap->getName() == "RobotomyRequestForm", "a heap form behaves through the base pointer");
  delete heap;
  check(true, "delete through AForm* completes (virtual dtor)");
}

// ── Orthodox Canonical Form across the hierarchy ────────────────────────────
static void test_ocf() {
  Bureaucrat boss("Boss", 1);

  RobotomyRequestForm source("Original");
  source.beSigned(boss);

  // The copy ctor must forward to AForm(other): the base half comes along.
  RobotomyRequestForm copy(source);
  check(copy.getName() == "RobotomyRequestForm", "copy keeps the name");
  check(copy.getGradeToSign() == 72 && copy.getGradeToExecute() == 45, "copy keeps both grades");
  check(copy.isSigned(), "copy carries the signature from the base sub-object");

  // An unsigned source copies as unsigned.
  RobotomyRequestForm pristine("P");
  RobotomyRequestForm pristineCopy(pristine);
  check(!pristineCopy.isSigned(), "copy of an unsigned form is unsigned");

  // The copy is independent: its target is its own.
  {
    std::string a, b;
    {
      Capture cap;
      copy.execute(boss);
      a = cap.str();
    }
    {
      Capture cap;
      source.execute(boss);
      b = cap.str();
    }
    check(a.find("Original") != std::string::npos && b.find("Original") != std::string::npos, "the copy kept the source's target");
  }

  // Assignment moves the signature through AForm::operator=; the const base
  // members stay put.
  RobotomyRequestForm target("Target");
  check(!target.isSigned(), "target starts unsigned");
  target = source;
  check(target.isSigned(), "assignment copies the signed flag through the base");
  check(target.getGradeToSign() == 72, "assignment keeps the const grades");

  // Assigning an unsigned form over a signed one clears the flag.
  RobotomyRequestForm signedForm("S");
  signedForm.beSigned(boss);
  RobotomyRequestForm unsignedForm("U");
  signedForm = unsignedForm;
  check(!signedForm.isSigned(), "assignment can clear the signed flag");

  // The derived operator= returns *this by reference and chains.
  RobotomyRequestForm x("X"), y("Y"), z("Z");
  z.beSigned(boss);
  RobotomyRequestForm& assigned = (x = y);
  check(&assigned == &x, "assignment returns *this by reference");
  x = y = z;
  check(x.isSigned() && y.isSigned(), "assignment chains");

  // Self-assignment is harmless.
  RobotomyRequestForm& alias = target;
  target = alias;
  check(target.isSigned() && target.getGradeToSign() == 72, "self-assignment is harmless");

  // Assignment must not disturb the source.
  check(source.isSigned() && source.getName() == "RobotomyRequestForm", "assignment leaves the source untouched");
}

// ── The Intern maps a name to a concrete type ───────────────────────────────
static void test_intern_makes_forms() {
  Intern intern;

  // Each known name yields the matching concrete type. getName() comes from the
  // form itself, so this cannot pass by accident of the table's key strings.
  const char* names[3] = {"shrubbery creation", "robotomy request", "presidential pardon"};
  const char* expected[3] = {"ShrubberyCreationForm", "RobotomyRequestForm", "PresidentialPardonForm"};
  const int signGrades[3] = {145, 72, 25};
  const int execGrades[3] = {137, 45, 5};

  for (int i = 0; i < 3; ++i) {
    AForm* form;
    {
      Capture c;
      form = intern.makeForm(names[i], "target");
    }
    check(form != NULL, "a known form name yields a form");
    if (form == NULL) continue;
    check(form->getName() == expected[i], "the name selects the right concrete type");
    // The form must arrive with its own grades, not some default.
    check(form->getGradeToSign() == signGrades[i], "the built form carries its own sign grade");
    check(form->getGradeToExecute() == execGrades[i], "the built form carries its own exec grade");
    check(!form->isSigned(), "a form leaves the intern unsigned");
    delete form;
  }

  // The subject's announcement line.
  {
    Capture c;
    AForm* form = intern.makeForm("robotomy request", "Bender");
    const std::string out = c.str();
    check(out.find("Intern creates") != std::string::npos, "makeForm announces 'Intern creates <form>'");
    check(out.find("RobotomyRequestForm") != std::string::npos, "the announcement names the form that was built");
    delete form;
  }
}

// ── The second parameter really reaches the form ────────────────────────────
static void test_intern_target_wiring() {
  Intern intern;
  Bureaucrat boss("Boss", 1);
  const std::string target = "tests_intern";
  const std::string filename = target + "_shrubbery";
  std::remove(filename.c_str());

  AForm* form;
  {
    Capture c;
    form = intern.makeForm("shrubbery creation", target);
  }
  check(form != NULL, "shrubbery creation is a known name");
  if (form == NULL) return;

  // The only externally visible proof that `target` was not dropped or swapped
  // with the form name: the file the action writes is named after it.
  {
    Capture c;
    boss.signForm(*form);
    boss.executeForm(*form);
  }
  check(fileExists(filename), "the target parameter reaches the form it was built with");
  std::remove(filename.c_str());
  delete form;

  // A different target must produce a different file, or the target could be
  // hard-coded and the check above would still pass.
  AForm* other;
  {
    Capture c;
    other = intern.makeForm("shrubbery creation", "tests_other");
    if (other != NULL) {
      boss.signForm(*other);
      boss.executeForm(*other);
    }
  }
  check(fileExists("tests_other_shrubbery"), "a second target produces its own file");
  check(!fileExists(filename), "the first target's file was not rewritten");
  std::remove("tests_other_shrubbery");
  delete other;
}

// ── An unknown name fails loudly and allocates nothing ──────────────────────
static void test_intern_unknown_name() {
  Intern intern;

  {
    Capture c;
    AForm* form = intern.makeForm("coffee making", "kitchen");
    const std::string out = c.str();
    check(form == NULL, "an unknown form name returns NULL");
    check(!out.empty(), "an unknown form name prints an explicit error");
    check(out.find("coffee making") != std::string::npos, "the error echoes the name that failed");
    check(out.find("Intern creates") == std::string::npos, "a failure never announces a creation");
    delete form;  // deleting NULL is legal; this documents that it is safe
  }

  // Matching is exact — no case-folding, no trimming, no prefix matching. A
  // near-miss must fail rather than silently produce the wrong form.
  const char* nearMisses[4] = {"Robotomy Request", "robotomy", "robotomy request ", ""};
  for (int i = 0; i < 4; ++i) {
    Capture c;
    AForm* form = intern.makeForm(nearMisses[i], "X");
    check(form == NULL, "a near-miss name does not silently build a form");
    delete form;
  }
}

// ── Intern's own orthodox canonical form ────────────────────────────────────
static void test_intern_ocf() {
  Intern a;
  Intern b(a);
  Intern c;
  c = b;
  // Self-assignment must be safe. Routed through an alias because
  // -Wself-assign-overloaded rejects the literal `c = c` under -Werror.
  Intern& alias = c;
  c = alias;

  // An intern has no state, so the only observable property of a copy is that
  // it still does the job identically.
  AForm* fromA;
  AForm* fromC;
  {
    Capture cap;
    fromA = a.makeForm("presidential pardon", "Arthur");
    fromC = c.makeForm("presidential pardon", "Arthur");
  }
  check(fromA != NULL && fromC != NULL, "both interns produce a form");
  if (fromA != NULL && fromC != NULL)
    check(fromA->getName() == fromC->getName(), "a copied intern builds the same form");
  delete fromA;
  delete fromC;

  // makeForm is const, so it must be callable on a const Intern. This is a
  // compile-time assertion as much as a runtime one.
  const Intern constant;
  AForm* form;
  {
    Capture cap;
    form = constant.makeForm("robotomy request", "Marvin");
  }
  check(form != NULL, "makeForm is const and works on a const Intern");
  delete form;
}

// ── The OCF default constructors ────────────────────────────────────────────
// Module 05 exempts only exception classes from Orthodox Canonical Form, so
// each concrete form carries one. No other code here calls them, which is
// exactly why they need their own checks: a wrong grade typed into a
// constructor nothing exercises would otherwise never surface.
static void test_default_constructors() {
  ShrubberyCreationForm shrub;
  RobotomyRequestForm robot;
  PresidentialPardonForm pardon;

  check(shrub.getName() == "ShrubberyCreationForm", "default-constructed shrubbery keeps its class name");
  check(shrub.getGradeToSign() == 145 && shrub.getGradeToExecute() == 137, "default-constructed shrubbery keeps 145/137");
  check(robot.getName() == "RobotomyRequestForm", "default-constructed robotomy keeps its class name");
  check(robot.getGradeToSign() == 72 && robot.getGradeToExecute() == 45, "default-constructed robotomy keeps 72/45");
  check(pardon.getName() == "PresidentialPardonForm", "default-constructed pardon keeps its class name");
  check(pardon.getGradeToSign() == 25 && pardon.getGradeToExecute() == 5, "default-constructed pardon keeps 25/5");
  check(!shrub.isSigned() && !robot.isSigned() && !pardon.isSigned(), "a default-constructed form starts unsigned");

  // It must still behave like a form, not just hold the right numbers.
  Bureaucrat boss("Boss", 1);
  {
    Capture c;
    boss.signForm(pardon);
    boss.executeForm(pardon);
  }
  check(pardon.isSigned(), "a default-constructed form can be signed and executed");

  // Copy and assignment must work off a default-constructed source too.
  RobotomyRequestForm copy(robot);
  check(copy.getGradeToExecute() == 45, "copying a default-constructed form keeps its grades");
  RobotomyRequestForm assigned("X");
  assigned = robot;
  check(assigned.getName() == "RobotomyRequestForm", "assigning from a default-constructed form works");
}

// ── Every exception really derives from std::exception ──────────────────────
// The subject's example catches by `std::exception&`. A class that merely LOOKS
// like an exception — right name, own what(), no inheritance — still satisfies
// a handler naming it directly, and is missed by every generic one. These
// checks pin the relationship down at compile time, then prove it at each site.
static void test_exception_hierarchy() {
  // COMPILE-TIME proof: these references bind only if the derivation is real
  // and public. Remove a `: public std::exception` and this stops compiling.
  Bureaucrat::GradeTooHighException bHigh;
  Bureaucrat::GradeTooLowException bLow;
  AForm::GradeTooHighException fHigh;
  AForm::GradeTooLowException fLow;
  AForm::NotSignedException notSigned;
  const std::exception& asBase1 = bHigh;
  const std::exception& asBase2 = bLow;
  const std::exception& asBase3 = fHigh;
  const std::exception& asBase4 = fLow;
  const std::exception& asBase5 = notSigned;
  check(asBase1.what() != NULL && asBase1.what()[0] != '\0', "Bureaucrat::GradeTooHigh is a std::exception with a message");
  check(asBase2.what() != NULL && asBase2.what()[0] != '\0', "Bureaucrat::GradeTooLow is a std::exception with a message");
  check(asBase3.what() != NULL && asBase3.what()[0] != '\0', "AForm::GradeTooHigh is a std::exception with a message");
  check(asBase4.what() != NULL && asBase4.what()[0] != '\0', "AForm::GradeTooLow is a std::exception with a message");
  check(asBase5.what() != NULL && asBase5.what()[0] != '\0', "AForm::NotSigned is a std::exception with a message");

  // RUN-TIME: every site that can throw, caught only as std::exception&.
  bool site[6];
  for (int i = 0; i < 6; ++i) site[i] = false;

  try { Bureaucrat b("X", 0); } catch (std::exception&) { site[0] = true; }
  try { Bureaucrat b("X", 151); } catch (std::exception&) { site[1] = true; }
  { Bureaucrat b("X", 1);   try { b.incrementGrade(); } catch (std::exception&) { site[2] = true; } }
  { Bureaucrat b("X", 150); try { b.decrementGrade(); } catch (std::exception&) { site[3] = true; } }
  { RobotomyRequestForm f("X"); Bureaucrat junior("J", 150);
    try { f.beSigned(junior); } catch (std::exception&) { site[4] = true; } }
  { RobotomyRequestForm f("X"); Bureaucrat boss("B", 1);
    Capture mute;
    try { f.execute(boss); } catch (std::exception&) { site[5] = true; }
    mute.str(); }

  check(site[0], "Bureaucrat grade 0 is catchable as std::exception&");
  check(site[1], "Bureaucrat grade 151 is catchable as std::exception&");
  check(site[2], "incrementGrade() at the ceiling is catchable as std::exception&");
  check(site[3], "decrementGrade() at the floor is catchable as std::exception&");
  check(site[4], "a refused beSigned is catchable as std::exception&");
  check(site[5], "executing an unsigned form is catchable as std::exception&");

  // Executing signed-but-too-junior is the third distinct failure; it must also
  // reach a std::exception& handler, and say something different from the other two.
  std::string unsignedMsg, tooJuniorMsg;
  { RobotomyRequestForm f("X"); Bureaucrat boss("B", 1);
    Capture mute;
    try { f.execute(boss); } catch (std::exception& e) { unsignedMsg = e.what(); }
    mute.str(); }
  { RobotomyRequestForm f("X"); Bureaucrat boss("B", 1); f.beSigned(boss);
    Bureaucrat junior("J", 150);
    Capture mute;
    try { f.execute(junior); } catch (std::exception& e) { tooJuniorMsg = e.what(); }
    mute.str(); }
  check(!unsignedMsg.empty() && !tooJuniorMsg.empty() && unsignedMsg != tooJuniorMsg,
        "the two execute() failures stay distinguishable through std::exception&");
}

// ── Runner ──────────────────────────────────────────────────────────────────
static void run(const char* name, void (*fn)()) {
  const int failuresBefore = g_failures;
  const int checksBefore = g_checks;
  // A section that throws where it shouldn't is a failure to report, not a
  // crash: without this, one stray exception aborts the whole run and the
  // remaining sections never get to speak.
  try {
    fn();
  } catch (std::exception& e) {
    check(false, "an unexpected exception escaped this section");
    std::cout << "          (" << e.what() << ")" << std::endl;
  } catch (...) {
    check(false, "an unexpected non-standard exception escaped this section");
  }
  std::cout << (g_failures == failuresBefore ? "  [ pass ] " : "  [ FAIL ] ") << name
            << " (" << (g_checks - checksBefore) << " checks)" << std::endl;
}

int main() {
  std::cout << "AForm + Intern test suite" << std::endl;
  run("fixed grades of the three forms", test_fixed_grades);
  run("exceptions derive from std::exception", test_exception_hierarchy);
  run("beSigned rank rule", test_be_signed);
  run("execute() gate", test_execute_gate);
  run("concrete actions", test_actions);
  run("executeForm output", test_execute_form);
  run("polymorphism", test_polymorphism);
  run("orthodox canonical form", test_ocf);
  run("OCF default constructors", test_default_constructors);
  run("intern builds each form", test_intern_makes_forms);
  run("intern wires the target through", test_intern_target_wiring);
  run("intern rejects unknown names", test_intern_unknown_name);
  run("intern orthodox canonical form", test_intern_ocf);

  if (g_failures != 0) {
    std::cout << g_failures << " of " << g_checks << " checks failed" << std::endl;
    return (1);
  }
  std::cout << "all " << g_checks << " checks passed" << std::endl;
  return (0);
}
