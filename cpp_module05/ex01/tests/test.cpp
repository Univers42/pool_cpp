// Standalone C++98 test for ex01: Form signing rules.
// Exits non-zero on any failure.

#include <iostream>

#include "Bureaucrat.hpp"
#include "Form.hpp"

static int g_fails = 0;

static void check(bool ok, const char* what) {
  if (!ok) {
    std::cerr << "FAIL: " << what << std::endl;
    ++g_fails;
  }
}

int main() {
  // Form construction bounds
  try {
    Form f("F", 0, 10);
    check(false, "Form grade 0 must throw GradeTooHighException");
  } catch (Form::GradeTooHighException&) {
  }
  try {
    Form f("F", 10, 151);
    check(false, "Form grade 151 must throw GradeTooLowException");
  } catch (Form::GradeTooLowException&) {
  }
  try {
    Form f("F", 1, 150);
    check(!f.isSigned(), "new form must start unsigned");
    check(f.getGradeToSign() == 1 && f.getGradeToExecute() == 150,
          "getters return construction grades");
  } catch (std::exception&) {
    check(false, "Form(1, 150) must not throw");
  }

  // beSigned: too low throws and leaves form unsigned
  {
    Form f("F", 50, 50);
    Bureaucrat low("Low", 51);
    try {
      f.beSigned(low);
      check(false, "beSigned with lower grade must throw");
    } catch (Form::GradeTooLowException&) {
    }
    check(!f.isSigned(), "failed beSigned must leave form unsigned");
  }

  // beSigned: equal grade is enough
  {
    Form f("F", 50, 50);
    Bureaucrat equal("Equal", 50);
    f.beSigned(equal);
    check(f.isSigned(), "equal grade must sign the form");
  }

  // signForm never throws; sets signed only on success
  {
    Form f("F", 10, 10);
    Bureaucrat weak("Weak", 100);
    Bureaucrat boss("Boss", 1);
    try {
      weak.signForm(f);
      check(!f.isSigned(), "signForm by low grade must not sign");
      boss.signForm(f);
      check(f.isSigned(), "signForm by high grade must sign");
    } catch (std::exception&) {
      check(false, "signForm must not propagate exceptions");
    }
  }

  // Copy assignment: only the signed flag transfers (name/grades are const)
  {
    Form a("A", 5, 5);
    Form b("B", 20, 20);
    Bureaucrat boss("Boss", 1);
    boss.signForm(a);
    b = a;
    check(b.isSigned(), "assignment must copy the signed flag");
    check(b.getName() == "B" && b.getGradeToSign() == 20,
          "assignment must not change const attributes");
  }

  if (g_fails == 0) std::cout << "ex01 tests: OK" << std::endl;
  return g_fails == 0 ? 0 : 1;
}
