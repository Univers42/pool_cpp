/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:41:55 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// A guided tour of ex03. The forms and the Bureaucrat are ex02's, unchanged;
// what is new is the Intern, and specifically how makeForm() turns a runtime
// string into the right concrete type without an if/elseif ladder. The
// assertions live in tests/test.cpp (make test); this file is the readable
// proof.

#include <iomanip>
#include <iostream>
#include <string>

#include "AForm.hpp"
#include "Bureaucrat.hpp"
#include "Intern.hpp"

static const char* RESET = "\033[0m";
static const char* BOLD = "\033[1m";
static const char* DIM = "\033[2m";
static const char* CYAN = "\033[96m";
static const char* GREEN = "\033[92m";
static const char* RED = "\033[91m";

// ── Tiny presentation helpers ───────────────────────────────────────────────
static void rule(const char* color) { std::cout << color; for (int i = 0; i < 72; ++i) std::cout << "─"; std::cout << RESET << std::endl; }
static void section(int n, const std::string& title) { std::cout << std::endl << BOLD << CYAN << " " << n << " · " << title << RESET << std::endl; rule(DIM); }
static void note(const std::string& text) { std::cout << DIM << "   " << text << RESET << std::endl; }
static void show(const std::string& label, const AForm& f) { std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(22) << label << f << std::endl; }

// ── 1. The subject's own example, verbatim ──────────────────────────────────
static void demo_subject_example() {
  section(1, "THE SUBJECT'S EXAMPLE, VERBATIM");
  note("Intern someRandomIntern;");
  note("AForm* rrf;");
  note("rrf = someRandomIntern.makeForm(\"robotomy request\", \"Bender\");");
  std::cout << std::endl;

  Intern someRandomIntern;
  AForm* rrf;
  rrf = someRandomIntern.makeForm("robotomy request", "Bender");
  if (rrf != NULL) {
    show("the form built", *rrf);
    note("The target reached the form: executing it will drill Bender.");
    delete rrf;
  }
}

// ── 2. Every name the intern knows ──────────────────────────────────────────
static void demo_all_three() {
  section(2, "ONE CALL SHAPE, THREE CONCRETE TYPES");
  note("Same function, same two strings — the name alone picks the type,");
  note("and each form arrives with its own fixed grades.");
  std::cout << std::endl;

  Intern intern;
  const char* names[3] = {"shrubbery creation", "robotomy request", "presidential pardon"};
  const char* targets[3] = {"home", "Bender", "Arthur Dent"};

  for (int i = 0; i < 3; ++i) {
    AForm* form = intern.makeForm(names[i], targets[i]);
    if (form != NULL) {
      show(std::string("target \"") + targets[i] + "\"", *form);
      delete form;
    }
  }
}

// ── 3. An unknown name ──────────────────────────────────────────────────────
static void demo_unknown_name() {
  section(3, "A NAME THE INTERN DOES NOT KNOW");
  note("The subject asks for an explicit error message. It also lists what");
  note("the intern CAN make, so a typo is diagnosable from the message alone.");
  std::cout << std::endl;

  Intern intern;
  AForm* coffee = intern.makeForm("coffee making", "kitchen");
  if (coffee == NULL)
    std::cout << "   " << GREEN << "✔ " << RESET << "makeForm returned " << BOLD << "NULL" << RESET << " — nothing was allocated, nothing to leak." << std::endl;
  else
    std::cout << "   " << RED << "✘ unexpected form" << RESET << std::endl;

  note("Near-misses are not guessed at either — the match is exact:");
  std::cout << std::endl;
  AForm* typo = intern.makeForm("Robotomy Request", "Bender");
  if (typo == NULL)
    std::cout << "   " << GREEN << "✔ " << RESET << "capitalisation matters; no silent wrong form." << std::endl;
  delete typo;
}

// ── 4. Intern and Bureaucrat, end to end ────────────────────────────────────
static void demo_end_to_end() {
  section(4, "THE WHOLE MODULE IN ONE FLOW");
  note("Intern makes the form, bureaucrat signs it, bureaucrat executes it.");
  note("makeForm returns a heap AForm*, so the CALLER owns it and deletes it.");
  std::cout << std::endl;

  Intern intern;
  Bureaucrat boss("Boss", 1);
  AForm* pardon = intern.makeForm("presidential pardon", "Ford Prefect");
  if (pardon == NULL) return;

  show("fresh from the intern", *pardon);
  note("unsigned, so execute() refuses even for grade 1:");
  boss.executeForm(*pardon);
  note("sign it, then execute it:");
  boss.signForm(*pardon);
  boss.executeForm(*pardon);
  show("after execution", *pardon);

  // Virtual destructor in AForm: this runs ~PresidentialPardonForm first.
  delete pardon;
  note("delete through AForm* runs the concrete destructor — no leak.");
}

// ── 5. Why a table and not a ladder ─────────────────────────────────────────
static void demo_no_ladder() {
  section(5, "WHY A LOOKUP TABLE, NOT if/elseif/else");
  note("The subject rejects the ladder outright. In Intern.cpp the mapping is");
  note("DATA, not control flow:");
  std::cout << std::endl;
  std::cout << DIM
            << "     const FormEntry kForms[] = {\n"
            << "         {\"shrubbery creation\",  &makeShrubbery},\n"
            << "         {\"robotomy request\",    &makeRobotomy},\n"
            << "         {\"presidential pardon\", &makePardon},\n"
            << "     };" << RESET << std::endl;
  std::cout << std::endl;
  note("Adding a fourth form is one row plus one creator. The search loop");
  note("never changes, so there is no new branch to get wrong — and no way");
  note("for one form to be handled differently from the other three.");
}

// ── 6. Orthodox Canonical Form on a class with no state ─────────────────────
static void demo_ocf() {
  section(6, "OCF WITH NOTHING TO COPY");
  note("No name, no grade, no members at all — so every OCF member is empty");
  note("and any two interns are interchangeable. They still have to exist.");
  std::cout << std::endl;

  Intern a;
  Intern b(a);
  Intern c;
  c = b;
  // Self-assignment, through an alias so the compiler's -Wself-assign-overloaded
  // does not reject the literal `c = c` at compile time. Harmless either way:
  // there is no state to corrupt.
  Intern& alias = c;
  c = alias;

  AForm* fromA = a.makeForm("shrubbery creation", "office");
  AForm* fromC = c.makeForm("shrubbery creation", "office");
  if (fromA != NULL && fromC != NULL) {
    std::cout << "   " << GREEN << "✔ " << RESET << "a copied intern makes exactly the same form: "
              << (fromA->getName() == fromC->getName() ? "yes" : "no") << std::endl;
  }
  delete fromA;
  delete fromC;
  note("(both wrote ./office_shrubbery only if executed — they were not)");
}

int main() {
  rule(CYAN);
  std::cout << BOLD << " CPP MODULE 05 · ex03 · At least this beats coffee-making..." << RESET << std::endl;
  rule(CYAN);

  demo_subject_example();
  demo_all_three();
  demo_unknown_name();
  demo_end_to_end();
  demo_no_ladder();
  demo_ocf();

  std::cout << std::endl;
  rule(CYAN);
  std::cout << BOLD << " Assertions: " << RESET << "make test" << std::endl;
  std::cout << BOLD << " Sabotage drill: " << RESET << "bash tests/mutants.sh" << std::endl;
  rule(CYAN);
  return (0);
}
