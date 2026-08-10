// ─────────────────────────────────────────────────────────────────────────────
//  all.cpp — CPP Module 05, exercises 00 → 02 in a single C++17 file.
//
//  Build:  c++ -std=c++17 -Wall -Wextra -Werror all.cpp -o all && ./all
//
//  This is NOT the graded submission — the exercises must be handed in as
//  separate C++98 files (see ex00/ … ex03/). This file is the same problem
//  written the way you would write it if the standard were not pinned to 1998,
//  so the two can be diffed. Every place the modern version differs, the
//  comment says WHAT C++98 forced and WHY the newer form is better.
//
//  Covered:
//    ex00  Bureaucrat — a grade in [1, 150] that cannot leave its range
//    ex01  Form       — signing, gated by grade
//    ex02  AForm      — abstract base + three concrete forms, gated execution
//
//  ex03 (Intern) is deliberately out of scope, as asked.
// ─────────────────────────────────────────────────────────────────────────────

// This file is C++17 and says so up front. Without the guard, a plain
// `c++ all.cpp` compiles as gnu++14 (the default for clang 12 and g++ 10) and
// buries you in twenty errors about std::string_view and std::filesystem "not
// existing", none of which name the actual problem.
#if __cplusplus < 201703L
#error "all.cpp requires C++17 — build with: c++ -std=c++17 -Wall -Wextra -Werror all.cpp -o all"
#endif

#include <array>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace bureaucracy {

// ─── Grade bounds ────────────────────────────────────────────────────────────
// C++98: `static const int` in a class, or worse, a #define or a bare 150
// repeated at every comparison. C++17: `inline constexpr` gives a real constant
// with external linkage and no ODR problem, usable in constant expressions.
inline constexpr int kHighestGrade = 1;
inline constexpr int kLowestGrade = 150;

// ─── Exceptions ──────────────────────────────────────────────────────────────
// C++98 (and the subject) nests these inside each class, so Bureaucrat and Form
// each declare their OWN GradeTooHighException. Two names for one concept means
// a caller that wants to handle "grade out of range" from either has to write
// two catch clauses, or fall back to catching std::exception and lose the
// distinction. Here they are hoisted to namespace scope and shared.
//
// `what()` is `noexcept override` — C++98 could only write `const throw()`,
// which is deprecated in C++11 and removed in C++20.
class GradeTooHighException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override { return "grade is too high (1 is the highest)"; }
};

class GradeTooLowException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override { return "grade is too low (150 is the lowest)"; }
};

// A DIFFERENT failure from the two above, and the C++98 version conflates them.
// There, `GradeTooLowException` is thrown both when a grade falls outside
// [1, 150] and when a perfectly legal grade simply does not outrank a form —
// so a grade-20 bureaucrat refused by a form requiring 5 is reported with a
// message about 150 being the lowest, which is not what went wrong. Splitting
// the range violation from the rank comparison makes each message true.
class InsufficientGradeException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override { return "this bureaucrat does not outrank the form"; }
};

class FormNotSignedException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override { return "the form is not signed"; }
};

// ─── Grade: a strong type ────────────────────────────────────────────────────
// The single biggest structural change. In the C++98 version the grade is a raw
// `int` and the [1, 150] check is written out in Bureaucrat's constructor, in
// its increment, in its decrement, and again in Form's constructor — four
// copies of one rule. Here the rule lives in ONE place: a value of type Grade
// cannot exist outside the range, because the only way to make one runs the
// check.
//
// It also kills the direction bug. "1 is the highest" means a *smaller* number
// outranks a larger one, so the comparison is `<=` — written backwards it
// compiles and silently inverts the whole hierarchy. `outranks()` names the
// intent once so no call site has to remember which way the operator goes.
class Grade {
 public:
  // `explicit` so an int never silently becomes a Grade.
  //
  // `constexpr` moves the check to compile time WHEN the result is required to
  // be constant: `constexpr Grade g{0};` is a compile error, and `Grade{42}` in
  // that same position is usable in a static_assert. Note what this does NOT
  // cover — the fixed grades below (`Grade{145}` in a constructor's init list)
  // are ordinary runtime contexts, so those still validate at run time. The
  // constexpr buys compile-time checking only where you ask for it.
  explicit constexpr Grade(int value) : value_{checked(value)} {}

  [[nodiscard]] constexpr int value() const noexcept { return value_; }

  // 1 is the highest grade, so "at least as senior as" is `<=`.
  [[nodiscard]] constexpr bool outranks(const Grade& other) const noexcept { return value_ <= other.value_; }

  // Return new values instead of mutating. The caller assigns only if no throw
  // happened, which is what gives Bureaucrat::promote() its strong exception
  // guarantee for free — see below.
  [[nodiscard]] constexpr Grade promoted() const { return Grade{value_ - 1}; }
  [[nodiscard]] constexpr Grade demoted() const { return Grade{value_ + 1}; }

  // Hidden friends: found only by ADL, so they never pollute overload
  // resolution elsewhere. The C++98 idiom is a free function declared after the
  // class, which is easier to forget to declare `const`.
  friend constexpr bool operator==(const Grade& a, const Grade& b) noexcept { return a.value_ == b.value_; }
  friend std::ostream& operator<<(std::ostream& os, const Grade& g) { return os << g.value_; }

 private:
  // constexpr *and* throwing: legal since C++14. Used in a constant expression
  // the throw path makes it a compile error; used at runtime it throws.
  static constexpr int checked(int value) {
    if (value < kHighestGrade) throw GradeTooHighException{};
    if (value > kLowestGrade) throw GradeTooLowException{};
    return value;
  }

  int value_;
};

class AForm;

// ─── ex00 · Bureaucrat ───────────────────────────────────────────────────────
class Bureaucrat {
 public:
  // Take the string by value and move it: one allocation whether the caller
  // passes an lvalue (copy, then move) or a temporary (move, then move). The
  // C++98 `const std::string&` signature always copies.
  Bureaucrat(std::string name, Grade grade) : name_{std::move(name)}, grade_{grade} {}

  // RULE OF ZERO. Every special member the compiler generates is already
  // correct, because both members are value types that own themselves. The
  // C++98 version has to hand-write four of them, and its `const std::string
  // _name` silently deletes copy assignment as a side effect. Here the name is
  // non-const and simply has no setter — the invariant is enforced by the
  // interface, not by a qualifier that breaks assignment.

  [[nodiscard]] const std::string& name() const noexcept { return name_; }
  [[nodiscard]] Grade grade() const noexcept { return grade_; }

  // Strong exception guarantee: promoted() either returns a valid Grade or
  // throws, and grade_ is only overwritten after it has returned. A bureaucrat
  // at grade 1 that refuses promotion is left exactly as it was.
  void promote() { grade_ = grade_.promoted(); }
  void demote() { grade_ = grade_.demoted(); }

  // `const` on the Bureaucrat: signing changes the FORM, not the signatory.
  void signForm(AForm& form) const;
  void executeForm(const AForm& form) const;

  friend std::ostream& operator<<(std::ostream& os, const Bureaucrat& b) {
    return os << b.name_ << ", bureaucrat grade " << b.grade_ << '.';
  }

 private:
  std::string name_;
  Grade grade_;
};

// ─── ex01 + ex02 · AForm ─────────────────────────────────────────────────────
// ex01's Form and ex02's AForm are the same class one step apart, so the modern
// version writes it once, abstract from the start.
//
// The execute()/executeAction() split is unchanged from the C++98 version and
// is still the point: execute() is public and NON-virtual, so the two
// preconditions are checked in exactly one place; executeAction() is protected
// and pure virtual, so a concrete form supplies only its own behaviour and
// outside code cannot reach it without passing the gate.
class AForm {
 public:
  AForm(std::string name, Grade toSign, Grade toExecute)
      : name_{std::move(name)}, toSign_{toSign}, toExecute_{toExecute} {}

  // A polymorphic base needs a virtual destructor; `= default` says so without
  // writing an empty body. Declaring it re-suppresses the implicit moves, so
  // they are asked for back explicitly — the rule of five, stated in five
  // lines instead of implemented in fifty.
  virtual ~AForm() = default;
  AForm(const AForm&) = default;
  AForm& operator=(const AForm&) = default;
  AForm(AForm&&) noexcept = default;
  AForm& operator=(AForm&&) noexcept = default;

  // NOTE: copy assignment actually WORKS here. In the C++98 version `_name`,
  // `_gradeToSign` and `_gradeToExecute` are `const` members, so operator= can
  // only copy the signed flag — a crippled assignment that satisfies OCF on
  // paper and lies in practice. Dropping const from the members and removing
  // the setters keeps the invariant and gets real value semantics back.

  [[nodiscard]] const std::string& name() const noexcept { return name_; }
  [[nodiscard]] bool isSigned() const noexcept { return signed_; }
  [[nodiscard]] Grade gradeToSign() const noexcept { return toSign_; }
  [[nodiscard]] Grade gradeToExecute() const noexcept { return toExecute_; }

  // Signing twice is a harmless no-op. The throw happens before the mutation,
  // so a refused signature leaves the form untouched.
  void beSigned(const Bureaucrat& bureaucrat) {
    if (!bureaucrat.grade().outranks(toSign_)) throw InsufficientGradeException{};
    signed_ = true;
  }

  // The gate. Order matters for the diagnostic: an unsigned form reports that
  // it is unsigned even when the executor is also too junior, because "not
  // signed" is the more actionable of the two.
  void execute(const Bureaucrat& executor) const {
    if (!signed_) throw FormNotSignedException{};
    if (!executor.grade().outranks(toExecute_)) throw InsufficientGradeException{};
    executeAction(executor);
  }

  friend std::ostream& operator<<(std::ostream& os, const AForm& f) {
    return os << "form " << f.name_ << ", signed: " << (f.signed_ ? "yes" : "no")
              << ", grade to sign: " << f.toSign_ << ", grade to execute: " << f.toExecute_ << '.';
  }

 protected:
  virtual void executeAction(const Bureaucrat& executor) const = 0;

 private:
  std::string name_;
  bool signed_ = false;  // default member initialiser — C++11, no ctor needed
  Grade toSign_;
  Grade toExecute_;
};

// Defined out of line: they need AForm to be complete.
inline void Bureaucrat::signForm(AForm& form) const {
  try {
    form.beSigned(*this);
    std::cout << name_ << " signed " << form.name() << '\n';
  } catch (const std::exception& e) {
    std::cout << name_ << " couldn't sign " << form.name() << " because " << e.what() << ".\n";
  }
}

inline void Bureaucrat::executeForm(const AForm& form) const {
  try {
    form.execute(*this);
    std::cout << name_ << " executed " << form.name() << '\n';
  } catch (const std::exception& e) {
    std::cout << name_ << " couldn't execute " << form.name() << " because " << e.what() << ".\n";
  }
}

// ─── ex02 · The three concrete forms ─────────────────────────────────────────
// `final` on each: nothing is meant to derive further, and it lets the compiler
// devirtualise. `override` on every hook — C++98 had no such keyword, so a typo
// in the signature silently left the class abstract instead of overriding.
//
// None of them has a default constructor. The target is the form's whole
// identity, so a targetless form is meaningless; the target-taking constructor
// suppresses the implicit default one and `ShrubberyCreationForm f;` is a
// compile error, which is the correct answer to that line.

class ShrubberyCreationForm final : public AForm {
 public:
  explicit ShrubberyCreationForm(std::string target)
      : AForm{"ShrubberyCreationForm", Grade{145}, Grade{137}}, target_{std::move(target)} {}

 protected:
  void executeAction(const Bureaucrat&) const override {
    // std::filesystem is C++17. The C++98 version concatenates strings and
    // hopes; a path is a type that knows it is one.
    const std::filesystem::path file = target_ + "_shrubbery";
    std::ofstream out{file};
    if (!out) throw std::runtime_error{"could not open " + file.string()};
    for (int i = 0; i < 3; ++i) out << kTree << '\n';
  }

 private:
  // A raw string literal, so the ASCII tree contains real backslashes instead
  // of doubled ones. `inline constexpr std::string_view` needs no out-of-line
  // definition and costs nothing at runtime.
  static constexpr std::string_view kTree = R"(       _-_
    /~~   ~~\
 /~~         ~~\
{               }
 \  _-     -_  /
   ~  \\ //  ~
_- -   | | _- _
  _ -  | |   -_
      // \\
)";

  std::string target_;
};

class RobotomyRequestForm final : public AForm {
 public:
  explicit RobotomyRequestForm(std::string target)
      : AForm{"RobotomyRequestForm", Grade{72}, Grade{45}}, target_{std::move(target)} {}

 protected:
  void executeAction(const Bureaucrat&) const override {
    std::cout << "* drilling noises *\n";
    // <random> instead of rand() % 2. rand() is the only RNG C++98 offers, and
    // `% 2` on it samples the low bit, which is the worst-distributed bit of
    // most implementations. A bernoulli_distribution says "a fair coin" and
    // means it. thread_local so the engine is seeded once, not per call.
    static thread_local std::mt19937 engine{std::random_device{}()};
    static thread_local std::bernoulli_distribution coin{0.5};
    if (coin(engine))
      std::cout << target_ << " has been robotomized successfully\n";
    else
      std::cout << "the robotomy of " << target_ << " failed\n";
  }

 private:
  std::string target_;
};

class PresidentialPardonForm final : public AForm {
 public:
  explicit PresidentialPardonForm(std::string target)
      : AForm{"PresidentialPardonForm", Grade{25}, Grade{5}}, target_{std::move(target)} {}

 protected:
  void executeAction(const Bureaucrat&) const override {
    std::cout << target_ << " has been pardoned by Zaphod Beeblebrox\n";
  }

 private:
  std::string target_;
};

}  // namespace bureaucracy

// ─────────────────────────────────────────────────────────────────────────────
//  Demo
// ─────────────────────────────────────────────────────────────────────────────

namespace {

constexpr std::string_view kReset = "\033[0m";
constexpr std::string_view kBold = "\033[1m";
constexpr std::string_view kDim = "\033[2m";
constexpr std::string_view kCyan = "\033[96m";
constexpr std::string_view kGreen = "\033[92m";

void section(int n, std::string_view title) {
  std::cout << '\n' << kBold << kCyan << ' ' << n << " · " << title << kReset << '\n' << kDim;
  for (int i = 0; i < 72; ++i) std::cout << "─";
  std::cout << kReset << '\n';
}

void note(std::string_view text) { std::cout << kDim << "   " << text << kReset << '\n'; }
void ok(std::string_view text) { std::cout << "   " << kGreen << "✔ " << kReset << text << '\n'; }

// Runs `fn` and reports the exception it was supposed to raise.
template <typename Fn>
void expectThrow(std::string_view label, Fn&& fn) {
  try {
    fn();
    std::cout << "   " << label << ": " << kBold << "no throw — unexpected" << kReset << '\n';
  } catch (const std::exception& e) {
    std::cout << "   " << kGreen << "✔ " << kReset << label << ": " << kDim << e.what() << kReset << '\n';
  }
}

}  // namespace

int main() {
  using namespace bureaucracy;

  std::cout << kCyan;
  for (int i = 0; i < 72; ++i) std::cout << "─";
  std::cout << kReset << '\n'
            << kBold << " CPP MODULE 05 · ex00–ex02 · one file, C++17 semantics" << kReset << '\n'
            << kCyan;
  for (int i = 0; i < 72; ++i) std::cout << "─";
  std::cout << kReset << '\n';

  // ── ex00 ──────────────────────────────────────────────────────────────────
  section(0, "BUREAUCRAT — A GRADE THAT CANNOT LEAVE [1, 150]");
  note("The range check lives in Grade, so it is written once and cannot drift.");

  const Bureaucrat boss{"Boss", Grade{1}};
  const Bureaucrat junior{"Junior", Grade{150}};
  std::cout << "   " << boss << '\n' << "   " << junior << '\n';

  expectThrow("grade 0 rejected", [] { return Bureaucrat{"TooHigh", Grade{0}}; });
  expectThrow("grade 151 rejected", [] { return Bureaucrat{"TooLow", Grade{151}}; });

  note("Promotion past 1 and demotion past 150 throw, and leave the object intact:");
  Bureaucrat climber{"Climber", Grade{2}};
  climber.promote();
  ok("promoted to " + std::to_string(climber.grade().value()));
  expectThrow("promoting grade 1", [&climber] { climber.promote(); });
  ok("still grade " + std::to_string(climber.grade().value()) + " after the failed promotion");

  // ── ex01 ──────────────────────────────────────────────────────────────────
  section(1, "FORM — SIGNING IS GATED BY GRADE");
  note("A form needs grade 145 to sign; grade 150 is too junior, grade 1 is not.");

  ShrubberyCreationForm shrub{"home"};
  std::cout << "   " << shrub << '\n';
  junior.signForm(shrub);
  boss.signForm(shrub);
  ok(shrub.isSigned() ? "the form is now signed" : "the form is still unsigned");

  // ── ex02 ──────────────────────────────────────────────────────────────────
  section(2, "AFORM — ABSTRACT BASE, GATED EXECUTION");
  note("`AForm f;` does not compile: executeAction() is pure virtual.");
  note("execute() checks signed-then-grade in ONE place, then calls the hook.");
  std::cout << '\n';

  PresidentialPardonForm pardon{"Ford Prefect"};
  note("unsigned — refused even for grade 1:");
  boss.executeForm(pardon);
  note("signed, but grade 20 does not reach the execution grade of 5:");
  boss.signForm(pardon);
  const Bureaucrat middle{"Middle", Grade{20}};
  middle.executeForm(pardon);
  note("grade 1 clears it:");
  boss.executeForm(pardon);

  std::cout << '\n';
  note("One loop, three behaviours — unique_ptr owns them, no delete in sight:");
  std::array<std::unique_ptr<AForm>, 3> forms{
      std::make_unique<ShrubberyCreationForm>("office"),
      std::make_unique<RobotomyRequestForm>("Bender"),
      std::make_unique<PresidentialPardonForm>("Trillian"),
  };

  // Structured bindings would need a pair; a range-for over unique_ptr& is the
  // point here — the loop never names a concrete type.
  for (const auto& form : forms) {
    boss.signForm(*form);
    boss.executeForm(*form);
  }
  ok("every form destroyed through AForm* by unique_ptr — virtual dtor, no leak");

  std::cout << '\n';
  note("Assignment actually works, unlike the C++98 version's const members:");
  RobotomyRequestForm source{"Original"};
  boss.signForm(source);
  RobotomyRequestForm copy{"Copy"};
  copy = source;
  ok(copy.isSigned() ? "the signature survived assignment" : "assignment lost the signature");

  // Only the executed shrubbery form writes anything — the "home" one above was
  // signed but never executed, so ./home_shrubbery does not exist.
  std::cout << '\n' << kDim << " ./office_shrubbery was written by the one shrubbery form that ran." << kReset << '\n';
  return 0;
}
