// Standalone C++98 test suite for Form and the Bureaucrat/Form handshake.
//
// Covers: grade bounds on both required grades, exception identity (including
// the fact that Form's exceptions are not Bureaucrat's), the beSigned() rank
// rule and its exact boundary, the strong exception guarantee, the two output
// sentences the subject mandates for signForm(), operator<< format, and the
// Orthodox Canonical Form traps that const members create.
//
// Every check is written so that a *wrong* implementation fails it.
// Exits non-zero on any failure.

#include <iostream>
#include <sstream>
#include <string>

#include "Bureaucrat.hpp"
#include "Form.hpp"

static int g_failures = 0;
static int g_checks = 0;

static void check(bool ok, const char* what) { ++g_checks; if (!ok) { ++g_failures; std::cout << "    FAIL: " << what << std::endl; } }

// Builds a Form and reports what happened, so grades can be swept in a loop:
// -1 = GradeTooHigh, 0 = built, 1 = GradeTooLow, 2 = something else (a bug).
static int classify(int sign, int exec) {
  try {
    Form probe("Probe", sign, exec);
    return (probe.getGradeToSign() == sign && probe.getGradeToExecute() == exec ? 0 : 2);
  } catch (Form::GradeTooHighException&) {
    return (-1);
  } catch (Form::GradeTooLowException&) {
    return (1);
  } catch (std::exception&) {
    return (2);
  }
}

// Runs `fn` with std::cout redirected into a string, so the subject's exact
// sentences can be asserted instead of eyeballed. The buffer is always put
// back, even though nothing here throws.
static std::string capture(void (*fn)(Bureaucrat&, Form&), Bureaucrat& b, Form& f) {
  std::ostringstream sink;
  std::streambuf* saved = std::cout.rdbuf(sink.rdbuf());
  fn(b, f);
  std::cout.rdbuf(saved);
  return (sink.str());
}

static void callSignForm(Bureaucrat& b, Form& f) { b.signForm(f); }

// ── Construction ────────────────────────────────────────────────────────────
static void test_construction() {
  Form blank;
  check(!blank.isSigned(), "default ctor leaves the form unsigned");

  Form f("Contract", 42, 12);
  check(f.getName() == "Contract", "getName returns the constructed name");
  check(f.getGradeToSign() == 42, "getGradeToSign returns the sign grade");
  check(f.getGradeToExecute() == 12, "getGradeToExecute returns the exec grade");
  check(!f.isSigned(), "a new form always starts unsigned");

  // Both bounds are legal values for both grades.
  Form edges("Edges", 1, 150);
  check(edges.getGradeToSign() == 1 && edges.getGradeToExecute() == 150, "grades 1 and 150 are valid");

  // An empty name is a name like any other; the class has no opinion on it.
  Form nameless("", 75, 75);
  check(nameless.getName() == "", "empty name is accepted");

  // The two grades are independent — exec may outrank sign or vice versa.
  Form inverted("Inverted", 10, 140);
  check(inverted.getGradeToSign() == 10 && inverted.getGradeToExecute() == 140, "the two grades are independent");
}

// ── Bounds, swept on both grades ────────────────────────────────────────────
static void test_bounds_sweep() {
  // Every grade in [1, 150] must construct and round-trip, in both slots.
  bool allValid = true;
  for (int g = 1; g <= 150; ++g)
    if (classify(g, g) != 0) allValid = false;
  check(allValid, "all 150 valid grades construct and round-trip");

  // Below 1 is "too high" — the inverted scale is where implementations slip.
  bool allHighSign = true, allHighExec = true;
  for (int g = -50; g <= 0; ++g) {
    if (classify(g, 75) != -1) allHighSign = false;
    if (classify(75, g) != -1) allHighExec = false;
  }
  check(allHighSign, "sign grade <= 0 throws GradeTooHighException");
  check(allHighExec, "exec grade <= 0 throws GradeTooHighException");

  // Above 150 is "too low".
  bool allLowSign = true, allLowExec = true;
  for (int g = 151; g <= 250; ++g) {
    if (classify(g, 75) != 1) allLowSign = false;
    if (classify(75, g) != 1) allLowExec = false;
  }
  check(allLowSign, "sign grade > 150 throws GradeTooLowException");
  check(allLowExec, "exec grade > 150 throws GradeTooLowException");

  // The exact edges, spelled out so a failure names the off-by-one.
  check(classify(0, 75) == -1, "sign grade 0 -> GradeTooHigh");
  check(classify(1, 75) == 0, "sign grade 1 -> valid");
  check(classify(150, 75) == 0, "sign grade 150 -> valid");
  check(classify(151, 75) == 1, "sign grade 151 -> GradeTooLow");
  check(classify(75, 0) == -1, "exec grade 0 -> GradeTooHigh");
  check(classify(75, 151) == 1, "exec grade 151 -> GradeTooLow");

  // A form that fails validation must not come into existence at all. If the
  // checks ran in the constructor body instead of the initializer list, the
  // const members would already be set by the time it threw.
  bool built = false;
  try {
    Form doomed("Doomed", 0, 0);
    built = true;
  } catch (std::exception&) {
  }
  check(!built, "a form with an invalid grade is never constructed");
}

// ── Exception identity ──────────────────────────────────────────────────────
static void test_exceptions() {
  // Catchable by the base reference, as the subject's example requires.
  try {
    Form f("X", 0, 10);
    check(false, "sign grade 0 must throw");
  } catch (std::exception& e) {
    check(e.what() && e.what()[0] != '\0', "GradeTooHigh is a std::exception with a message");
  }

  // The two types are distinct: a TooLow handler must not swallow a TooHigh.
  bool wrongHandler = false;
  try {
    Form f("X", 0, 10);
  } catch (Form::GradeTooLowException&) {
    wrongHandler = true;
  } catch (Form::GradeTooHighException&) {
  }
  check(!wrongHandler, "Form::GradeTooHigh is not caught as Form::GradeTooLow");

  bool wrongHandler2 = false;
  try {
    Form f("X", 151, 10);
  } catch (Form::GradeTooHighException&) {
    wrongHandler2 = true;
  } catch (Form::GradeTooLowException&) {
  }
  check(!wrongHandler2, "Form::GradeTooLow is not caught as Form::GradeTooHigh");

  // Form's exceptions are their own types, unrelated to Bureaucrat's despite
  // the identical spelling. A shared base would make this handler fire.
  bool caughtAsBureaucrat = false;
  try {
    Form f("X", 200, 10);
  } catch (Bureaucrat::GradeTooLowException&) {
    caughtAsBureaucrat = true;
  } catch (Form::GradeTooLowException&) {
  }
  check(!caughtAsBureaucrat, "Form::GradeTooLow is not a Bureaucrat::GradeTooLow");

  // The two messages must differ, or the diagnostic is useless.
  std::string hi, lo;
  try { Form f("X", 0, 10); } catch (std::exception& e) { hi = e.what(); }
  try { Form f("X", 151, 10); } catch (std::exception& e) { lo = e.what(); }
  check(!hi.empty() && !lo.empty() && hi != lo, "the two exceptions carry different messages");

  // what() is stable: same object, same answer twice (no dangling buffer).
  try {
    Form f("X", 0, 10);
  } catch (std::exception& e) {
    check(std::string(e.what()) == std::string(e.what()), "what() is stable across calls");
  }
}

// ── beSigned: the rank rule ─────────────────────────────────────────────────
static void test_be_signed() {
  // A signer who does not outrank the form is refused, and the form is left
  // exactly as it was — the throw must precede the assignment.
  {
    Form f("F", 50, 50);
    Bureaucrat low("Low", 51);
    try {
      f.beSigned(low);
      check(false, "beSigned with a lower grade must throw");
    } catch (Form::GradeTooLowException&) {
    }
    check(!f.isSigned(), "a refused beSigned leaves the form unsigned");
  }

  // Equal grade is enough: the subject says "greater than or equal".
  {
    Form f("F", 50, 50);
    Bureaucrat exact("Exact", 50);
    f.beSigned(exact);
    check(f.isSigned(), "equal grade signs the form");
  }

  // One rank better also signs.
  {
    Form f("F", 50, 50);
    Bureaucrat better("Better", 49);
    f.beSigned(better);
    check(f.isSigned(), "a higher rank (lower number) signs the form");
  }

  // The exact boundary, swept: 1..50 sign, 51..150 do not.
  bool boundaryHolds = true;
  for (int g = 1; g <= 150; ++g) {
    Form f("F", 50, 50);
    Bureaucrat b("B", g);
    try {
      f.beSigned(b);
      if (g > 50 || !f.isSigned()) boundaryHolds = false;
    } catch (Form::GradeTooLowException&) {
      if (g <= 50 || f.isSigned()) boundaryHolds = false;
    }
  }
  check(boundaryHolds, "the sign boundary is exactly at gradeToSign, inclusive");

  // The execute grade must play no part in signing.
  {
    Form f("F", 150, 1);
    Bureaucrat weak("Weak", 150);
    f.beSigned(weak);
    check(f.isSigned(), "gradeToExecute does not gate signing");
  }

  // Signing twice is idempotent, not an error.
  {
    Form f("F", 10, 10);
    Bureaucrat boss("Boss", 1);
    f.beSigned(boss);
    f.beSigned(boss);
    check(f.isSigned(), "signing an already-signed form stays signed");
  }
}

// ── signForm: the subject's two sentences ───────────────────────────────────
static void test_sign_form() {
  // Success path: "<bureaucrat> signed <form>"
  {
    Form f("TopSecret", 10, 10);
    Bureaucrat boss("Boss", 1);
    const std::string out = capture(callSignForm, boss, f);
    check(out == "Boss signed TopSecret\n", "signForm prints '<name> signed <form>'");
    check(f.isSigned(), "a successful signForm signs the form");
  }

  // Failure path: "<bureaucrat> couldn't sign <form> because <reason>."
  {
    Form f("TopSecret", 10, 10);
    Bureaucrat weak("Weak", 100);
    const std::string out = capture(callSignForm, weak, f);
    check(out.find("Weak couldn't sign TopSecret because ") == 0, "signForm prints the refusal sentence");
    check(out[out.size() - 1] == '\n' && out[out.size() - 2] == '.', "the refusal sentence ends with '.'");
    check(!f.isSigned(), "a refused signForm leaves the form unsigned");
  }

  // signForm reports; it never propagates. A caller needs no try/catch.
  {
    Form f("F", 1, 1);
    Bureaucrat weak("Weak", 150);
    bool escaped = false;
    std::ostringstream sink;
    std::streambuf* saved = std::cout.rdbuf(sink.rdbuf());
    try {
      weak.signForm(f);
    } catch (std::exception&) {
      escaped = true;
    }
    std::cout.rdbuf(saved);
    check(!escaped, "signForm never propagates an exception");
  }
}

// ── Printing ────────────────────────────────────────────────────────────────
static void test_output() {
  Form f("Contract", 42, 12);

  // Every attribute the subject asks for must appear.
  std::ostringstream os;
  os << f;
  const std::string s = os.str();
  check(s.find("Contract") != std::string::npos, "operator<< prints the name");
  check(s.find("42") != std::string::npos, "operator<< prints the grade to sign");
  check(s.find("12") != std::string::npos, "operator<< prints the grade to execute");
  check(s.find("no") != std::string::npos, "operator<< prints the unsigned state");

  // The three checks above only prove the numbers appear SOMEWHERE — swapping
  // the two grades in the stream operator passes every one of them. Pin the
  // whole sentence down so each value has to be in its own slot. (Found by
  // tests/mutants.sh: "form operator<< swaps its two grades" survived.)
  check(s == "form Contract, signed: no, grade to sign: 42, grade to execute: 12.",
        "operator<< emits the exact subject format, each grade in its own slot");

  // It must reflect the signature changing.
  Bureaucrat boss("Boss", 1);
  f.beSigned(boss);
  std::ostringstream os2;
  os2 << f;
  check(os2.str().find("yes") != std::string::npos, "operator<< reflects the signed state");
  check(os2.str() != s, "operator<< output changes once signed");

  // It chains like any other stream insertion, and adds no newline of its own.
  std::ostringstream os3;
  os3 << f << "|end";
  check(os3.str().find("|end") != std::string::npos, "operator<< chains");
  check(os3.str().find('\n') == std::string::npos, "operator<< emits no newline");

  // It takes a const reference, so a const form prints. If the getters lost
  // their const, this would not compile.
  const Form frozen("Frozen", 7, 3);
  std::ostringstream os4;
  os4 << frozen;
  check(os4.str().find("Frozen") != std::string::npos, "a const Form can be printed");
  check(frozen.getName() == "Frozen" && frozen.getGradeToSign() == 7 && !frozen.isSigned(), "getters are callable on a const object");
}

// ── Orthodox Canonical Form ─────────────────────────────────────────────────
// The traps here are the ones const members create: an assignment that cannot
// move most of the object, and a copy that must still take everything.
static void test_ocf() {
  Form source("Source", 30, 20);
  Bureaucrat boss("Boss", 1);
  source.beSigned(boss);

  // The copy constructor takes every member, signature included.
  Form copy(source);
  check(copy.getName() == "Source", "copy ctor copies the name");
  check(copy.getGradeToSign() == 30 && copy.getGradeToExecute() == 20, "copy ctor copies both grades");
  check(copy.isSigned(), "copy ctor copies the signed flag");

  // An unsigned form copies as unsigned — the flag is really read, not defaulted.
  Form pristine("Pristine", 30, 20);
  Form pristineCopy(pristine);
  check(!pristineCopy.isSigned(), "copy of an unsigned form is unsigned");

  // Assignment moves the only assignable member; the const ones stay put.
  Form target("Target", 90, 80);
  target = source;
  check(target.isSigned(), "assignment copies the signed flag");
  check(target.getName() == "Target", "assignment keeps the const name");
  check(target.getGradeToSign() == 90 && target.getGradeToExecute() == 80, "assignment keeps the const grades");

  // Assignment must not disturb the source.
  check(source.getName() == "Source" && source.isSigned(), "assignment leaves the source untouched");

  // Assigning an unsigned form over a signed one clears the flag: the operator
  // copies the value rather than OR-ing it.
  Form signedForm("S", 5, 5);
  signedForm.beSigned(boss);
  Form unsignedForm("U", 5, 5);
  signedForm = unsignedForm;
  check(!signedForm.isSigned(), "assignment can clear the signed flag");

  // It returns *this by reference, which is what makes chaining work.
  Form a("A", 10, 10), b("B", 20, 20), c("C", 30, 30);
  c.beSigned(boss);
  Form& assigned = (a = b);
  check(&assigned == &a, "assignment returns *this by reference");
  a = b = c;
  check(a.isSigned() && b.isSigned(), "assignment chains (a = b = c)");
  check(a.getName() == "A" && b.getName() == "B", "chained assignment keeps every name");

  // Self-assignment is a no-op, not a corruption. (Aliased through a reference
  // so the compiler doesn't reject the obvious `target = target`.)
  Form& alias = target;
  target = alias;
  check(target.isSigned() && target.getName() == "Target" && target.getGradeToSign() == 90, "self-assignment is harmless");

  // Copies survive their source: the copy holds its own string.
  Form* heap = new Form("Heap", 55, 45);
  Form survivor(*heap);
  delete heap;
  check(survivor.getName() == "Heap" && survivor.getGradeToSign() == 55, "a copy outlives its source");
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
  Form::GradeTooHighException fHigh;
  Form::GradeTooLowException fLow;
  const std::exception& asBase1 = bHigh;
  const std::exception& asBase2 = bLow;
  const std::exception& asBase3 = fHigh;
  const std::exception& asBase4 = fLow;
  check(asBase1.what() != NULL && asBase1.what()[0] != '\0', "Bureaucrat::GradeTooHigh is a std::exception with a message");
  check(asBase2.what() != NULL && asBase2.what()[0] != '\0', "Bureaucrat::GradeTooLow is a std::exception with a message");
  check(asBase3.what() != NULL && asBase3.what()[0] != '\0', "Form::GradeTooHigh is a std::exception with a message");
  check(asBase4.what() != NULL && asBase4.what()[0] != '\0', "Form::GradeTooLow is a std::exception with a message");

  // RUN-TIME: every site that can throw, caught only as std::exception&.
  bool site[7];
  for (int i = 0; i < 7; ++i) site[i] = false;

  try { Bureaucrat b("X", 0); } catch (std::exception&) { site[0] = true; }
  try { Bureaucrat b("X", 151); } catch (std::exception&) { site[1] = true; }
  { Bureaucrat b("X", 1);   try { b.incrementGrade(); } catch (std::exception&) { site[2] = true; } }
  { Bureaucrat b("X", 150); try { b.decrementGrade(); } catch (std::exception&) { site[3] = true; } }
  try { Form f("F", 0, 1); } catch (std::exception&) { site[4] = true; }
  try { Form f("F", 151, 1); } catch (std::exception&) { site[5] = true; }
  { Form f("F", 50, 50); Bureaucrat junior("J", 150);
    try { f.beSigned(junior); } catch (std::exception&) { site[6] = true; } }

  check(site[0], "Bureaucrat grade 0 is catchable as std::exception&");
  check(site[1], "Bureaucrat grade 151 is catchable as std::exception&");
  check(site[2], "incrementGrade() at the ceiling is catchable as std::exception&");
  check(site[3], "decrementGrade() at the floor is catchable as std::exception&");
  check(site[4], "Form sign grade 0 is catchable as std::exception&");
  check(site[5], "Form sign grade 151 is catchable as std::exception&");
  check(site[6], "a refused beSigned is catchable as std::exception&");
}

// ── Bureaucrat's six ways to move a grade ───────────────────────────────────
// ex01 carries its own copy of Bureaucrat, so ex00's coverage does not reach
// it. Without this, an operator with its own divergent bounds check — or a
// postfix that returns the wrong value — passes the whole suite. (Both were
// found as survivors by tests/mutants.sh.)
enum StepForm {
  kIncMethod = 0,
  kPrefixInc = 1,
  kPostfixInc = 2,
  kDecMethod = 3,
  kPrefixDec = 4,
  kPostfixDec = 5
};
static const int kStepCount = 6;

static bool applyStep(Bureaucrat& b, int form) {
  try {
    switch (form) {
      case kIncMethod: b.incrementGrade(); break;
      case kPrefixInc: ++b; break;
      case kPostfixInc: b++; break;
      case kDecMethod: b.decrementGrade(); break;
      case kPrefixDec: --b; break;
      case kPostfixDec: b--; break;
      default: break;
    }
    return (true);
  } catch (std::exception&) {
    return (false);
  }
}

static bool isPromotion(int form) { return (form <= kPostfixInc); }

static void test_bureaucrat_steps() {
  bool valuesExact = true, throwsAtBoundOnly = true, intactOnThrow = true;
  for (int form = 0; form < kStepCount; ++form) {
    for (int g = 1; g <= 150; ++g) {
      Bureaucrat b("Sweep", g);
      const bool promote = isPromotion(form);
      const bool shouldThrow = promote ? (g == 1) : (g == 150);
      const bool survived = applyStep(b, form);

      if (survived == shouldThrow) throwsAtBoundOnly = false;
      if (survived) {
        if (b.getGrade() != (promote ? g - 1 : g + 1)) valuesExact = false;
      } else if (b.getGrade() != g || b.getName() != "Sweep") {
        intactOnThrow = false;
      }
    }
  }
  check(valuesExact, "every step form moves the grade by exactly one, at every grade");
  check(throwsAtBoundOnly, "every step form throws at its bound and nowhere else");
  check(intactOnThrow, "a throwing step leaves name and grade untouched");

  // The three upward forms must be indistinguishable, likewise the downward.
  bool promoAgree = true, demoAgree = true;
  for (int g = 1; g <= 150; ++g) {
    int promoGrade[3], demoGrade[3];
    bool promoThrew[3], demoThrew[3];
    for (int k = 0; k < 3; ++k) {
      Bureaucrat p("P", g);
      promoThrew[k] = !applyStep(p, kIncMethod + k);
      promoGrade[k] = p.getGrade();
      Bureaucrat d("D", g);
      demoThrew[k] = !applyStep(d, kDecMethod + k);
      demoGrade[k] = d.getGrade();
    }
    for (int k = 1; k < 3; ++k) {
      if (promoGrade[k] != promoGrade[0] || promoThrew[k] != promoThrew[0]) promoAgree = false;
      if (demoGrade[k] != demoGrade[0] || demoThrew[k] != demoThrew[0]) demoAgree = false;
    }
  }
  check(promoAgree, "incrementGrade(), ++b and b++ agree at every grade");
  check(demoAgree, "decrementGrade(), --b and b-- agree at every grade");

  // Postfix hands back the value from before; prefix hands back the object.
  bool postfixSnapshot = true;
  for (int g = 2; g <= 149; ++g) {
    Bureaucrat up("Snap", g);
    const Bureaucrat beforeUp = up++;
    if (beforeUp.getGrade() != g || up.getGrade() != g - 1) postfixSnapshot = false;

    Bureaucrat down("Snap", g);
    const Bureaucrat beforeDown = down--;
    if (beforeDown.getGrade() != g || down.getGrade() != g + 1) postfixSnapshot = false;
  }
  check(postfixSnapshot, "postfix ++/-- return the value from before, at every grade");

  Bureaucrat ref("Ref", 50);
  check(&(++ref) == &ref, "prefix ++ returns *this by reference");
  check(&(--ref) == &ref, "prefix -- returns *this by reference");
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
  std::cout << "Form test suite" << std::endl;
  run("construction", test_construction);
  run("bounds sweep (both grades)", test_bounds_sweep);
  run("exception identity", test_exceptions);
  run("every step form, every grade", test_bureaucrat_steps);
  run("exceptions derive from std::exception", test_exception_hierarchy);
  run("beSigned rank rule", test_be_signed);
  run("signForm output", test_sign_form);
  run("operator<< output", test_output);
  run("orthodox canonical form", test_ocf);

  if (g_failures != 0) {
    std::cout << g_failures << " of " << g_checks << " checks failed" << std::endl;
    return (1);
  }
  std::cout << "all " << g_checks << " checks passed" << std::endl;
  return (0);
}
