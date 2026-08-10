// Standalone C++98 test suite for Bureaucrat.
//
// Covers: grade bounds (full 1..150 sweep), exception identity, the two
// mutation APIs and their agreement, prefix/postfix return values, the strong
// exception guarantee, operator<< format, and the Orthodox Canonical Form traps
// (forgotten member, no-op assignment, aliasing, const-correctness).
//
// Every check is written so that a *wrong* implementation fails it — see
// tests/ocf_mutants.sh, which breaks the class on purpose and verifies that
// this suite notices. Exits non-zero on any failure.

#include <iostream>
#include <sstream>
#include <string>

#include "Bureaucrat.hpp"

static int g_failures = 0;
static int g_checks = 0;

static void check(bool ok, const char* what) { ++g_checks; if (!ok) { ++g_failures; std::cout << "    FAIL: " << what << std::endl; } }

// Builds a Bureaucrat and reports what happened, so bounds can be swept in a
// loop instead of by hand: -1 = GradeTooHigh, 0 = built, 1 = GradeTooLow,
// 2 = some other exception (which would itself be a bug).
static int classify(int grade) {
  try {
    Bureaucrat probe("Probe", grade);
    return (probe.getGrade() == grade ? 0 : 2);
  } catch (Bureaucrat::GradeTooHighException&) {
    return (-1);
  } catch (Bureaucrat::GradeTooLowException&) {
    return (1);
  } catch (std::exception&) {
    return (2);
  }
}

// ── Construction ────────────────────────────────────────────────────────────
static void test_construction() {
  // Default constructor: the lowest grade, so always valid.
  Bureaucrat anon;
  check(anon.getName() == "Default" && anon.getGrade() == 150, "default ctor is Default/150");

  // Name-only constructor: caller's name, same starting grade.
  Bureaucrat rookie("Rookie");
  check(rookie.getName() == "Rookie" && rookie.getGrade() == 150, "name-only ctor defaults to grade 150");

  // Name+grade constructor, at both bounds.
  Bureaucrat top("Top", 1);
  Bureaucrat bottom("Bottom", 150);
  check(top.getGrade() == 1 && bottom.getGrade() == 150, "grades 1 and 150 are valid");
  check(top.getName() == "Top", "getName returns the constructed name");

  // An empty name is a name like any other; the class has no opinion on it.
  Bureaucrat nameless("", 75);
  check(nameless.getName() == "" && nameless.getGrade() == 75, "empty name is accepted");
}

// ── Bounds, swept exhaustively ──────────────────────────────────────────────
static void test_bounds_sweep() {
  // Every grade in [1, 150] must construct and keep its value.
  bool allValid = true;
  for (int g = 1; g <= 150; ++g)
    if (classify(g) != 0) allValid = false;
  check(allValid, "all 150 valid grades construct and round-trip");

  // Anything below 1 is "too high" — the scale is inverted, which is exactly
  // the place an implementation tends to get it backwards.
  bool allHigh = true;
  for (int g = -50; g <= 0; ++g)
    if (classify(g) != -1) allHigh = false;
  check(allHigh, "grades <= 0 throw GradeTooHighException");

  // Anything above 150 is "too low".
  bool allLow = true;
  for (int g = 151; g <= 250; ++g)
    if (classify(g) != 1) allLow = false;
  check(allLow, "grades > 150 throw GradeTooLowException");

  // The exact edges, spelled out so a failure names the off-by-one.
  check(classify(0) == -1, "grade 0 -> GradeTooHigh");
  check(classify(1) == 0, "grade 1 -> valid");
  check(classify(150) == 0, "grade 150 -> valid");
  check(classify(151) == 1, "grade 151 -> GradeTooLow");
}

// ── Exception identity ──────────────────────────────────────────────────────
static void test_exceptions() {
  // Catchable by the base reference, as the subject's example requires.
  try {
    Bureaucrat b("X", 0);
    check(false, "grade 0 must throw");
  } catch (std::exception& e) {
    check(e.what() && e.what()[0] != '\0', "GradeTooHigh is a std::exception with a message");
  }

  // The two types are distinct: a TooLow handler must not swallow a TooHigh.
  bool wrongHandler = false;
  try {
    Bureaucrat b("X", 0);
  } catch (Bureaucrat::GradeTooLowException&) {
    wrongHandler = true;
  } catch (Bureaucrat::GradeTooHighException&) {
  }
  check(!wrongHandler, "GradeTooHigh is not caught as GradeTooLow");

  bool wrongHandler2 = false;
  try {
    Bureaucrat b("X", 151);
  } catch (Bureaucrat::GradeTooHighException&) {
    wrongHandler2 = true;
  } catch (Bureaucrat::GradeTooLowException&) {
  }
  check(!wrongHandler2, "GradeTooLow is not caught as GradeTooHigh");

  // The two messages must differ, or the diagnostic is useless.
  std::string hi, lo;
  try { Bureaucrat b("X", 0); } catch (std::exception& e) { hi = e.what(); }
  try { Bureaucrat b("X", 151); } catch (std::exception& e) { lo = e.what(); }
  check(!hi.empty() && !lo.empty() && hi != lo, "the two exceptions carry different messages");

  // what() is stable: same object, same answer twice (no dangling buffer).
  try {
    Bureaucrat b("X", 0);
  } catch (std::exception& e) {
    check(std::string(e.what()) == std::string(e.what()), "what() is stable across calls");
  }
}

// ── Named increment / decrement ─────────────────────────────────────────────
static void test_named_mutations() {
  // Increment moves toward 1, decrement toward 150 (subject: 3 -> 2).
  Bureaucrat alice("Alice", 3);
  alice.incrementGrade();
  check(alice.getGrade() == 2, "increment 3 -> 2");
  alice.decrementGrade();
  check(alice.getGrade() == 3, "decrement 2 -> 3");
  check(alice.getName() == "Alice", "mutating the grade leaves the name alone");

  // Mutating past either bound throws and leaves the grade unchanged.
  Bureaucrat top("Top", 1);
  try {
    top.incrementGrade();
    check(false, "increment past 1 must throw");
  } catch (Bureaucrat::GradeTooHighException&) {
  }
  check(top.getGrade() == 1, "grade unchanged after failed increment");

  Bureaucrat bottom("Bottom", 150);
  try {
    bottom.decrementGrade();
    check(false, "decrement past 150 must throw");
  } catch (Bureaucrat::GradeTooLowException&) {
  }
  check(bottom.getGrade() == 150, "grade unchanged after failed decrement");

  // Walking the whole ladder up and back down stays in range the whole way.
  Bureaucrat walker("Walker", 150);
  for (int i = 0; i < 149; ++i) walker.incrementGrade();
  check(walker.getGrade() == 1, "149 increments reach grade 1");
  for (int i = 0; i < 149; ++i) walker.decrementGrade();
  check(walker.getGrade() == 150, "149 decrements return to grade 150");
}

// ── Operators ───────────────────────────────────────────────────────────────
static void test_operators() {
  // Prefix ++ promotes (3 -> 2) and hands back the object itself, not a copy.
  Bureaucrat a("A", 3);
  Bureaucrat& ref = ++a;
  check(a.getGrade() == 2, "++a promotes 3 -> 2");
  check(&ref == &a, "prefix ++ returns *this by reference");

  // Prefix -- demotes (2 -> 3), also by reference.
  Bureaucrat& ref2 = --a;
  check(a.getGrade() == 3, "--a demotes 2 -> 3");
  check(&ref2 == &a, "prefix -- returns *this by reference");

  // Postfix returns the value from *before* the mutation.
  Bureaucrat b("B", 3);
  Bureaucrat old = b++;
  check(old.getGrade() == 3, "b++ returns the old grade");
  check(b.getGrade() == 2, "b++ promotes the object to 2");
  check(old.getName() == "B", "the postfix copy keeps the name");

  Bureaucrat old2 = b--;
  check(old2.getGrade() == 2, "b-- returns the old grade");
  check(b.getGrade() == 3, "b-- demotes the object back to 3");

  // The postfix result is a real copy: mutating it must not move the original.
  Bureaucrat c("C", 50);
  Bureaucrat snapshot = c++;
  ++snapshot;
  check(c.getGrade() == 49 && snapshot.getGrade() == 49, "the postfix copy is independent");

  // Prefix chains because it returns a reference.
  Bureaucrat d("D", 5);
  ++(++d);
  check(d.getGrade() == 3, "++(++d) promotes twice");
  --(--d);
  check(d.getGrade() == 5, "--(--d) demotes twice");

  // The two APIs must agree exactly, on both directions.
  Bureaucrat viaOperator("X", 42);
  Bureaucrat viaMethod("X", 42);
  ++viaOperator;
  viaMethod.incrementGrade();
  check(viaOperator.getGrade() == viaMethod.getGrade(), "++ and incrementGrade() agree");
  --viaOperator;
  viaMethod.decrementGrade();
  check(viaOperator.getGrade() == viaMethod.getGrade(), "-- and decrementGrade() agree");
  viaOperator++;
  viaMethod.incrementGrade();
  check(viaOperator.getGrade() == viaMethod.getGrade(), "postfix ++ and incrementGrade() agree");
}

// ── Operators at the bounds ─────────────────────────────────────────────────
static void test_operator_bounds() {
  Bureaucrat top("Top", 1);
  try {
    ++top;
    check(false, "++ past grade 1 must throw");
  } catch (Bureaucrat::GradeTooHighException&) {
  }
  check(top.getGrade() == 1, "grade unchanged after failed ++top");

  try {
    top++;
    check(false, "postfix ++ past grade 1 must throw");
  } catch (Bureaucrat::GradeTooHighException&) {
  }
  check(top.getGrade() == 1, "grade unchanged after failed top++");

  Bureaucrat bottom("Bottom", 150);
  try {
    --bottom;
    check(false, "-- past grade 150 must throw");
  } catch (Bureaucrat::GradeTooLowException&) {
  }
  check(bottom.getGrade() == 150, "grade unchanged after failed --bottom");

  try {
    bottom--;
    check(false, "postfix -- past grade 150 must throw");
  } catch (Bureaucrat::GradeTooLowException&) {
  }
  check(bottom.getGrade() == 150, "grade unchanged after failed bottom--");

  // A throwing postfix must not leave a half-updated object behind either:
  // the copy is made first, so both it and the original must survive intact.
  Bureaucrat edge("Edge", 1);
  try {
    Bureaucrat unused = edge++;
    check(false, "postfix ++ at the bound must throw before returning");
  } catch (std::exception&) {
  }
  check(edge.getGrade() == 1 && edge.getName() == "Edge", "object intact after a throwing postfix ++");

  // Climbing to the top and stepping once more, the long way round.
  Bureaucrat climber("Climber", 3);
  int steps = 0;
  try {
    while (steps < 10) { ++climber; ++steps; }
    check(false, "the climb must hit the ceiling");
  } catch (Bureaucrat::GradeTooHighException&) {
    check(steps == 2 && climber.getGrade() == 1, "the climb stops exactly at grade 1");
  }
}

// ── The six ways to move a grade ────────────────────────────────────────────
// incrementGrade(), ++b, b++ and their three downward twins are four APIs over
// one rule. Spot-checking a couple of grades cannot tell you they share a
// bounds check — a second, subtly different check hidden in one operator would
// pass every hand-picked value and fail somewhere in the middle of the range.
// So the sweeps below apply every form at every grade in [1, 150].
enum StepForm {
  kIncMethod = 0,
  kPrefixInc = 1,
  kPostfixInc = 2,
  kDecMethod = 3,
  kPrefixDec = 4,
  kPostfixDec = 5
};
static const int kStepCount = 6;

// Applies one form; returns false if it threw.
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

static void test_step_sweep() {
  // Exact result, throw only at the bound, and nothing touched when it throws.
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

  // The three upward forms must be indistinguishable from each other, likewise
  // the three downward ones. A divergence means one has its own bounds check.
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

  // Postfix hands back the value from before — at every grade, not just one.
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

  // A step and its inverse must land exactly back where they started.
  bool roundTrip = true;
  for (int g = 2; g <= 149; ++g) {
    Bureaucrat b("RT", g);
    ++b;
    --b;
    if (b.getGrade() != g) roundTrip = false;
    b.incrementGrade();
    b.decrementGrade();
    if (b.getGrade() != g) roundTrip = false;
    b++;
    b--;
    if (b.getGrade() != g) roundTrip = false;
  }
  check(roundTrip, "a step and its inverse return to the starting grade, at every grade");
}

// ── The exceptions really derive from std::exception ────────────────────────
// The subject's own example catches by `std::exception&`. A class that merely
// LOOKS like an exception — right name, own what(), but no inheritance — would
// still satisfy a `catch (Bureaucrat::GradeTooHighException&)` handler and be
// missed by every generic one. These checks pin the relationship down at
// compile time and then prove it at every site that can throw.
static void test_exception_hierarchy() {
  // COMPILE-TIME proof: these references only bind if the derivation is real
  // and public. Drop the `: public std::exception` and this stops compiling
  // rather than quietly failing a runtime check.
  Bureaucrat::GradeTooHighException highObj;
  Bureaucrat::GradeTooLowException lowObj;
  const std::exception& highAsBase = highObj;
  const std::exception& lowAsBase = lowObj;
  check(highAsBase.what() != NULL && highAsBase.what()[0] != '\0', "GradeTooHigh::what() is non-empty through the base");
  check(lowAsBase.what() != NULL && lowAsBase.what()[0] != '\0', "GradeTooLow::what() is non-empty through the base");

  // RUN-TIME: every throw site, caught only as std::exception&. Testing the
  // constructor alone would leave the six mutation paths unproven.
  bool site[8];
  for (int i = 0; i < 8; ++i) site[i] = false;

  try { Bureaucrat b("X", 0); } catch (std::exception&) { site[0] = true; }
  try { Bureaucrat b("X", 151); } catch (std::exception&) { site[1] = true; }
  { Bureaucrat b("X", 1);   try { b.incrementGrade(); } catch (std::exception&) { site[2] = true; } }
  { Bureaucrat b("X", 150); try { b.decrementGrade(); } catch (std::exception&) { site[3] = true; } }
  { Bureaucrat b("X", 1);   try { ++b; }               catch (std::exception&) { site[4] = true; } }
  { Bureaucrat b("X", 1);   try { b++; }               catch (std::exception&) { site[5] = true; } }
  { Bureaucrat b("X", 150); try { --b; }               catch (std::exception&) { site[6] = true; } }
  { Bureaucrat b("X", 150); try { b--; }               catch (std::exception&) { site[7] = true; } }

  check(site[0], "constructor grade 0 is catchable as std::exception&");
  check(site[1], "constructor grade 151 is catchable as std::exception&");
  check(site[2], "incrementGrade() at the ceiling is catchable as std::exception&");
  check(site[3], "decrementGrade() at the floor is catchable as std::exception&");
  check(site[4], "prefix ++ at the ceiling is catchable as std::exception&");
  check(site[5], "postfix ++ at the ceiling is catchable as std::exception&");
  check(site[6], "prefix -- at the floor is catchable as std::exception&");
  check(site[7], "postfix -- at the floor is catchable as std::exception&");

  // One exception type per bound, reused everywhere: the message a failed ++
  // gives must be the message the constructor gives, not a parallel string.
  std::string fromCtor, fromPrefix, fromPostfix, fromMethod;
  try { Bureaucrat b("X", 0); } catch (std::exception& e) { fromCtor = e.what(); }
  { Bureaucrat b("X", 1); try { ++b; }               catch (std::exception& e) { fromPrefix = e.what(); } }
  { Bureaucrat b("X", 1); try { b++; }               catch (std::exception& e) { fromPostfix = e.what(); } }
  { Bureaucrat b("X", 1); try { b.incrementGrade(); } catch (std::exception& e) { fromMethod = e.what(); } }
  check(!fromCtor.empty() && fromCtor == fromPrefix && fromCtor == fromPostfix && fromCtor == fromMethod,
        "the ceiling reports one message however it is hit");

  // And the two bounds must still be told apart through the base reference.
  std::string ceiling, floorMsg;
  { Bureaucrat b("X", 1);   try { ++b; } catch (std::exception& e) { ceiling = e.what(); } }
  { Bureaucrat b("X", 150); try { --b; } catch (std::exception& e) { floorMsg = e.what(); } }
  check(!ceiling.empty() && !floorMsg.empty() && ceiling != floorMsg,
        "the ceiling and the floor stay distinguishable through std::exception&");
}

// ── Printing ────────────────────────────────────────────────────────────────
static void test_output() {
  Bureaucrat alice("Alice", 3);

  // operator<< follows the subject format exactly: "<name>, bureaucrat grade <grade>."
  std::ostringstream os;
  os << alice;
  check(os.str() == "Alice, bureaucrat grade 3.", "operator<< format");

  // It must reflect mutations, and chain like any other stream insertion.
  ++alice;
  std::ostringstream os2;
  os2 << alice << "|" << alice.getGrade();
  check(os2.str() == "Alice, bureaucrat grade 2.|2", "operator<< reflects the new grade and chains");

  // Both bounds print without surprises.
  std::ostringstream os3;
  Bureaucrat bottom("Bottom", 150);
  os3 << bottom;
  check(os3.str() == "Bottom, bureaucrat grade 150.", "operator<< prints grade 150");

  // The operator prints no newline of its own; the caller adds it.
  check(os3.str()[os3.str().size() - 1] == '.', "operator<< ends on '.' with no newline");

  // It takes a const reference, so a const object prints fine. If the getters
  // lost their const, this would not compile.
  const Bureaucrat frozen("Frozen", 7);
  std::ostringstream os4;
  os4 << frozen;
  check(os4.str() == "Frozen, bureaucrat grade 7.", "a const Bureaucrat can be printed");
  check(frozen.getName() == "Frozen" && frozen.getGrade() == 7, "getters are callable on a const object");
}

// ── Orthodox Canonical Form ─────────────────────────────────────────────────
// The traps here are the ones a wrong OCF actually falls into: a copy that
// forgets a member, an assignment that copies nothing, an assignment that
// returns the wrong thing, and aliasing.
static void test_ocf() {
  Bureaucrat source("Source", 42);

  // Copy constructor takes *both* members. Checking only the grade would let a
  // copy that forgets the name pass.
  Bureaucrat copy(source);
  check(copy.getName() == "Source", "copy ctor copies the name");
  check(copy.getGrade() == 42, "copy ctor copies the grade");

  // The copy is a separate object: mutating one must not move the other.
  ++copy;
  check(copy.getGrade() == 41 && source.getGrade() == 42, "the copy is independent of its source");

  // A copy of a copy still carries the original values.
  Bureaucrat copy2(copy);
  check(copy2.getName() == "Source" && copy2.getGrade() == 41, "copy of a copy is faithful");

  // Assignment moves the grade; the name is const and stays put.
  Bureaucrat target("Target", 100);
  target = source;
  check(target.getGrade() == 42, "assignment copies the grade");
  check(target.getName() == "Target", "assignment keeps the const name");

  // Assignment must not disturb the source.
  check(source.getGrade() == 42 && source.getName() == "Source", "assignment leaves the source untouched");

  // It returns *this by reference, which is what makes chaining work. Binding
  // the result to a non-const reference also fails to compile if the operator
  // ever returns by value.
  Bureaucrat x("X", 10), y("Y", 20), z("Z", 30);
  Bureaucrat& assigned = (x = y);
  check(&assigned == &x, "assignment returns *this by reference");
  x = y = z;
  check(x.getGrade() == 30 && y.getGrade() == 30 && z.getGrade() == 30, "assignment chains (x = y = z)");
  check(x.getName() == "X" && y.getName() == "Y", "chained assignment keeps every name");

  // Self-assignment is a no-op, not a corruption. (Aliased through a reference
  // so the compiler doesn't reject the obvious `target = target`.)
  Bureaucrat& alias = target;
  target = alias;
  check(target.getGrade() == 42 && target.getName() == "Target", "self-assignment is harmless");

  // Assigning from a const source compiles and works: operator= takes a const
  // reference, as OCF requires.
  const Bureaucrat frozen("Frozen", 3);
  Bureaucrat thawed("Thawed", 99);
  thawed = frozen;
  check(thawed.getGrade() == 3, "assignment accepts a const source");

  // A copy made from a const source works too.
  Bureaucrat fromConst(frozen);
  check(fromConst.getName() == "Frozen" && fromConst.getGrade() == 3, "copy ctor accepts a const source");

  // Copies survive their source: the copy holds its own string, so there is
  // nothing dangling once the original is gone.
  Bureaucrat* heap = new Bureaucrat("Heap", 55);
  Bureaucrat survivor(*heap);
  delete heap;
  check(survivor.getName() == "Heap" && survivor.getGrade() == 55, "a copy outlives its source");
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
  std::cout << "Bureaucrat test suite" << std::endl;
  run("construction", test_construction);
  run("bounds sweep", test_bounds_sweep);
  run("exception identity", test_exceptions);
  run("named increment/decrement", test_named_mutations);
  run("operators ++ / --", test_operators);
  run("operators at the bounds", test_operator_bounds);
  run("every step form, every grade", test_step_sweep);
  run("exceptions derive from std::exception", test_exception_hierarchy);
  run("operator<< output", test_output);
  run("orthodox canonical form", test_ocf);

  if (g_failures != 0) {
    std::cout << g_failures << " of " << g_checks << " checks failed" << std::endl;
    return (1);
  }
  std::cout << "all " << g_checks << " checks passed" << std::endl;
  return (0);
}
