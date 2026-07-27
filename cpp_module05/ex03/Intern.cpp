#include "Intern.hpp"

#include <iostream>

#include "AForm.hpp"
#include "PresidentialPardonForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "ShrubberyCreationForm.hpp"

Intern::Intern() {}
Intern::Intern(const Intern&) {}
Intern& Intern::operator=(const Intern&) { return *this; }
Intern::~Intern() {}

static AForm* makeShrubbery(const std::string& target) {
  return new ShrubberyCreationForm(target);
}
static AForm* makeRobotomy(const std::string& target) {
  return new RobotomyRequestForm(target);
}
static AForm* makePardon(const std::string& target) {
  return new PresidentialPardonForm(target);
}

AForm* Intern::makeForm(const std::string& formName,
                        const std::string& target) const {
  // Name -> constructor lookup table; no if/elseif ladder.
  typedef AForm* (*Creator)(const std::string&);
  struct Entry {
    const char* name;
    Creator create;
  };
  static const Entry table[] = {
      {"shrubbery creation", &makeShrubbery},
      {"robotomy request", &makeRobotomy},
      {"presidential pardon", &makePardon},
  };
  const int count = sizeof(table) / sizeof(table[0]);

  for (int i = 0; i < count; ++i) {
    if (formName == table[i].name) {
      std::cout << "Intern creates " << table[i].name << std::endl;
      return table[i].create(target);
    }
  }
  std::cout << "Intern: form \"" << formName << "\" does not exist."
            << std::endl;
  return NULL;
}
