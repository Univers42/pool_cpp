// Standalone C++98 test for Bureaucrat: grade bounds, exception types,
// increment/decrement direction, strong guarantee on failed mutation, and
// operator<< format. Exits non-zero on any failure.

#include <iostream>
#include <sstream>
#include <string>

#include "Bureaucrat.hpp"

static int g_failures = 0;

static void check(bool ok, const char* what) {
  if (!ok) {
    ++g_failures;
    std::cout << "FAIL: " << what << std::endl;
  }
}

int main() {
  // Both bounds construct fine.
  try {
    Bureaucrat top("Top", 1);
    Bureaucrat bottom("Bottom", 150);
    check(top.getGrade() == 1 && bottom.getGrade() == 150,
          "grades 1 and 150 are valid");
    check(top.getName() == "Top", "getName returns the constructed name");
  } catch (std::exception&) {
    check(false, "grades 1/150 must not throw");
  }

  // Out-of-range construction throws the right exception, catchable as
  // std::exception with a non-empty what().
  try {
    Bureaucrat b("Zero", 0);
    check(false, "grade 0 must throw");
  } catch (Bureaucrat::GradeTooHighException& e) {
    check(e.what() && e.what()[0] != '\0', "GradeTooHigh what() non-empty");
  } catch (std::exception&) {
    check(false, "grade 0 must throw GradeTooHighException");
  }
  try {
    Bureaucrat b("Low", 151);
    check(false, "grade 151 must throw");
  } catch (Bureaucrat::GradeTooLowException& e) {
    check(e.what() && e.what()[0] != '\0', "GradeTooLow what() non-empty");
  } catch (std::exception&) {
    check(false, "grade 151 must throw GradeTooLowException");
  }

  // Increment moves toward 1, decrement toward 150 (subject: 3 -> 2).
  Bureaucrat alice("Alice", 3);
  alice.incrementGrade();
  check(alice.getGrade() == 2, "increment 3 -> 2");
  alice.decrementGrade();
  check(alice.getGrade() == 3, "decrement 2 -> 3");

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

  // operator<< follows the subject format exactly.
  std::ostringstream os;
  os << alice;
  check(os.str() == "Alice, bureaucrat grade 3.", "operator<< format");

  // Orthodox Canonical Form: copy takes everything, assignment copies the
  // grade (the name is const and stays).
  Bureaucrat copy(alice);
  check(copy.getName() == "Alice" && copy.getGrade() == 3, "copy constructor");
  Bureaucrat other("Other", 42);
  other = alice;
  check(other.getGrade() == 3, "assignment copies grade");
  check(other.getName() == "Other", "assignment keeps const name");

  if (g_failures != 0) {
    std::cout << g_failures << " check(s) failed" << std::endl;
    return 1;
  }
  std::cout << "test.cpp: all checks passed" << std::endl;
  return 0;
}
