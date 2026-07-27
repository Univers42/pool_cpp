#include "ShrubberyCreationForm.hpp"

#include <fstream>
#include <stdexcept>

ShrubberyCreationForm::ShrubberyCreationForm()
    : AForm("ShrubberyCreationForm", 145, 137), _target("default") {}

ShrubberyCreationForm::ShrubberyCreationForm(const std::string& target)
    : AForm("ShrubberyCreationForm", 145, 137), _target(target) {}

ShrubberyCreationForm::ShrubberyCreationForm(const ShrubberyCreationForm& other)
    : AForm(other), _target(other._target) {}

ShrubberyCreationForm& ShrubberyCreationForm::operator=(
    const ShrubberyCreationForm& other) {
  if (this != &other) {
    AForm::operator=(other);
    _target = other._target;
  }
  return *this;
}

ShrubberyCreationForm::~ShrubberyCreationForm() {}

void ShrubberyCreationForm::executeAction(const Bureaucrat&) const {
  std::string filename = _target + "_shrubbery";
  std::ofstream ofs(filename.c_str());
  if (!ofs)
    throw std::runtime_error("Failed to open output file: " + filename);

  const char* tree =
      "       _-_\n"
      "    /~~   ~~\\\n"
      " /~~         ~~\\\n"
      "{               }\n"
      " \\  _-     -_  /\n"
      "   ~  \\\\ //  ~\n"
      "_- -   | | _- _\n"
      "  _ -  | |   -_\n"
      "      // \\\\\n";

  for (int i = 0; i < 3; ++i) ofs << tree << std::endl;
}
