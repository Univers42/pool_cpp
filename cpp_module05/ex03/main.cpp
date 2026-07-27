#include <iostream>

#include "AForm.hpp"
#include "Bureaucrat.hpp"
#include "Intern.hpp"

int main() {
  Intern someRandomIntern;
  Bureaucrat boss("Boss", 1);

  std::cout << "--- subject example ---" << std::endl;
  AForm* rrf = someRandomIntern.makeForm("robotomy request", "Bender");
  if (rrf) {
    std::cout << *rrf << std::endl;
    boss.signForm(*rrf);
    boss.executeForm(*rrf);
    delete rrf;
  }

  std::cout << "\n--- other known forms ---" << std::endl;
  AForm* shrub = someRandomIntern.makeForm("shrubbery creation", "home");
  AForm* pardon = someRandomIntern.makeForm("presidential pardon", "Arthur");
  delete shrub;
  delete pardon;

  std::cout << "\n--- unknown form ---" << std::endl;
  AForm* coffee = someRandomIntern.makeForm("coffee making", "kitchen");
  if (!coffee) std::cout << "makeForm returned NULL as expected" << std::endl;

  return 0;
}
