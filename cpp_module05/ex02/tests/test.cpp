// Standalone C++98 test for ex02: concrete forms sign/execute rules.
// Exits non-zero on failure.

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Bureaucrat.hpp"
#include "PresidentialPardonForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "ShrubberyCreationForm.hpp"

static int g_failures = 0;

static void check(bool ok, const std::string& label) {
  if (!ok) {
    std::cerr << "FAIL: " << label << std::endl;
    ++g_failures;
  }
}

// Captures std::cout while form.execute(executor) runs; rethrows nothing —
// returns "" and counts a failure if execute throws.
static std::string captureExecute(const AForm& form,
                                  const Bureaucrat& executor,
                                  const std::string& label) {
  std::ostringstream out;
  std::streambuf* old = std::cout.rdbuf(out.rdbuf());
  try {
    form.execute(executor);
  } catch (std::exception& e) {
    std::cout.rdbuf(old);
    std::cerr << "FAIL: " << label << " threw: " << e.what() << std::endl;
    ++g_failures;
    return "";
  }
  std::cout.rdbuf(old);
  return out.str();
}

int main() {
  Bureaucrat boss("boss", 1);

  // Required grades per subject
  {
    ShrubberyCreationForm s("t");
    RobotomyRequestForm r("t");
    PresidentialPardonForm p("t");
    check(s.getGradeToSign() == 145 && s.getGradeToExecute() == 137,
          "shrubbery grades 145/137");
    check(r.getGradeToSign() == 72 && r.getGradeToExecute() == 45,
          "robotomy grades 72/45");
    check(p.getGradeToSign() == 25 && p.getGradeToExecute() == 5,
          "pardon grades 25/5");
    check(!s.isSigned(), "new form starts unsigned");
  }

  // Executing an unsigned form throws NotSignedException
  {
    PresidentialPardonForm p("t");
    bool threw = false;
    try {
      p.execute(boss);
    } catch (AForm::NotSignedException&) {
      threw = true;
    } catch (std::exception&) {
    }
    check(threw, "execute unsigned form throws NotSignedException");
  }

  // Signing below required grade throws; at required grade succeeds
  {
    PresidentialPardonForm p("t");
    Bureaucrat low("low", 26);
    bool threw = false;
    try {
      p.beSigned(low);
    } catch (AForm::GradeTooLowException&) {
      threw = true;
    }
    check(threw && !p.isSigned(), "grade 26 cannot sign pardon (needs 25)");
    Bureaucrat ok("ok", 25);
    p.beSigned(ok);
    check(p.isSigned(), "grade 25 signs pardon");
  }

  // Executing a signed form with too low a grade throws
  {
    ShrubberyCreationForm s("t");
    s.beSigned(boss);
    Bureaucrat weak("weak", 138);
    bool threw = false;
    try {
      s.execute(weak);
    } catch (AForm::GradeTooLowException&) {
      threw = true;
    }
    check(threw, "grade 138 cannot execute shrubbery (needs 137)");
  }

  // Shrubbery creates <target>_shrubbery with tree content
  {
    ShrubberyCreationForm s("test_target");
    s.beSigned(boss);
    s.execute(boss);
    std::ifstream ifs("test_target_shrubbery");
    check(ifs.good(), "shrubbery creates test_target_shrubbery");
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    check(!content.empty(), "shrubbery file is not empty");
    ifs.close();
    std::remove("test_target_shrubbery");  // cleanup
  }

  // Pardon prints the Zaphod message
  {
    PresidentialPardonForm p("Arthur");
    p.beSigned(boss);
    std::string out = captureExecute(p, boss, "pardon execute");
    check(out.find("Arthur has been pardoned by Zaphod Beeblebrox") !=
              std::string::npos,
          "pardon message mentions Zaphod");
  }

  // Robotomy drills, then reports success or failure (either is valid)
  {
    RobotomyRequestForm r("Marvin");
    r.beSigned(boss);
    std::string out = captureExecute(r, boss, "robotomy execute");
    check(out.find("drilling") != std::string::npos, "robotomy drills");
    check(out.find("Marvin has been robotomized successfully") !=
                  std::string::npos ||
              out.find("robotomy of Marvin failed") != std::string::npos,
          "robotomy reports success or failure");
  }

  // Bureaucrat grade bounds still enforced
  {
    bool threw = false;
    try {
      Bureaucrat bad("bad", 0);
    } catch (Bureaucrat::GradeTooHighException&) {
      threw = true;
    }
    check(threw, "grade 0 bureaucrat throws GradeTooHighException");
  }

  if (g_failures) {
    std::cerr << g_failures << " test(s) failed" << std::endl;
    return 1;
  }
  std::cout << "all ex02 tests passed" << std::endl;
  return 0;
}
