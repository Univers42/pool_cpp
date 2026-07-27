// Standalone C++98 test for Intern: makeForm returns the right concrete form
// with the target wired through, and NULL on an unknown name. Exits non-zero
// on any failure.

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "AForm.hpp"
#include "Bureaucrat.hpp"
#include "Intern.hpp"

static int g_failures = 0;

static void check(bool ok, const char* what) {
  if (!ok) {
    ++g_failures;
    std::cout << "FAIL: " << what << std::endl;
  }
}

int main() {
  Intern intern;

  // Each known name yields the matching concrete form.
  AForm* shrub = intern.makeForm("shrubbery creation", "test");
  AForm* robo = intern.makeForm("robotomy request", "Bender");
  AForm* pardon = intern.makeForm("presidential pardon", "Arthur");
  check(shrub != NULL && shrub->getName() == "ShrubberyCreationForm",
        "shrubbery creation -> ShrubberyCreationForm");
  check(robo != NULL && robo->getName() == "RobotomyRequestForm",
        "robotomy request -> RobotomyRequestForm");
  check(pardon != NULL && pardon->getName() == "PresidentialPardonForm",
        "presidential pardon -> PresidentialPardonForm");
  check(robo != NULL && robo->getGradeToSign() == 72 &&
            robo->getGradeToExecute() == 45,
        "robotomy form has its own grades (72/45)");

  // Unknown name -> NULL, no crash.
  check(intern.makeForm("coffee making", "kitchen") == NULL,
        "unknown form name returns NULL");

  // Target reaches the form: executing shrubbery writes test_shrubbery.
  if (shrub != NULL) {
    Bureaucrat boss("Boss", 1);
    boss.signForm(*shrub);
    boss.executeForm(*shrub);
    std::ifstream ofs("test_shrubbery");
    check(ofs.good(), "executed shrubbery form created test_shrubbery");
    ofs.close();
    std::remove("test_shrubbery");
  }

  // Unsigned form refuses to execute.
  if (pardon != NULL) {
    Bureaucrat boss("Boss", 1);
    bool threw = false;
    try {
      pardon->execute(boss);
    } catch (std::exception&) {
      threw = true;
    }
    check(threw, "executing an unsigned form throws");
  }

  delete shrub;
  delete robo;
  delete pardon;

  if (g_failures == 0) std::cout << "ex03 Intern tests: all OK" << std::endl;
  return g_failures == 0 ? 0 : 1;
}
