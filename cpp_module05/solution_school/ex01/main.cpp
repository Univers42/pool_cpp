/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 19:37:29 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/10 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// A guided tour of the Form class and the Bureaucrat/Form handshake: every
// rule the subject states, shown working and shown failing. The assertions
// live in tests/test.cpp (make test); this file is the readable proof.


/**
show thatif the grade is too lowe, throw a Form::GradeTooowExcetpion if verifying the status ofg
grade of the Bureucrat high enough or small enough..

add a signForm() member function in the Bureaucrat class. This function must call Form::beSigned() to
attempt to sign the form. If the form is signed succesfully, it will print something like:

<bureaucrat> signed <form>
Otherwise, it will print something like:
<bureaucrat> couldn't sign <form> because <reason>.
*/
#include <iomanip>
#include <iostream>
#include <string>

#include "Bureaucrat.hpp"
#include "Form.hpp"

// The `catch (std::exception& e)` handlers below rely on every exception in
// this exercise deriving publicly from std::exception, and on what() being
// virtual. Bureaucrat.cpp explains the mechanism in full.

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
static void show(const std::string& label, const Form& f) { std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(24) << label << f << std::endl; }
static void showB(const std::string& label, const Bureaucrat& b) { std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(24) << label << b << std::endl; }
static void caught(const std::string& label, const std::exception& e) { std::cout << "   " << RED << "✘ " << RESET << std::left << std::setw(24) << label << RED << e.what() << RESET << std::endl; }
// The catch itself is the success here, so it gets a tick, not a cross.
static void handled(const std::string& label, const std::exception& e) { std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(24) << label << DIM << e.what() << RESET << std::endl; }

// Tries one pair of grades and reports which side of the invariant it landed on.
static void probe(int sign, int exec) {
  std::cout << "   sign " << std::right << std::setw(4) << sign << ", exec " << std::setw(4) << exec << "  ";
  try {
    Form f("Probe", sign, exec);
    std::cout << GREEN << "accepted" << RESET << std::endl;
  } catch (Form::GradeTooHighException& e) {
    std::cout << RED << "rejected" << RESET << "  " << DIM << "GradeTooHigh: " << e.what() << RESET << std::endl;
  } catch (Form::GradeTooLowException& e) {
    std::cout << RED << "rejected" << RESET << "  " << DIM << "GradeTooLow:  " << e.what() << RESET << std::endl;
  }
}

// ── 1. Construction and the required attributes ─────────────────────────────
static void demo_construction() {
  section(1, "CONSTRUCTION — A FORM IS BORN UNSIGNED");
  note("A Form owns a const name, a signed flag, and two const grades.");
  Form blank;
  Form contract("ContractOfEmployment", 42, 12);
  show("default ctor", blank);
  show("name + two grades", contract);
  note("The subject requires the signed flag to start false — it does:");
  std::cout << "   isSigned()          -> " << BOLD << (contract.isSigned() ? "true" : "false") << RESET << std::endl;
  std::cout << "   getName()           -> " << BOLD << contract.getName() << RESET << std::endl;
  std::cout << "   getGradeToSign()    -> " << BOLD << contract.getGradeToSign() << RESET << std::endl;
  std::cout << "   getGradeToExecute() -> " << BOLD << contract.getGradeToExecute() << RESET << std::endl;

  // operator<< takes a const reference, so a const form prints fine.
  const Form frozen("Frozen", 7, 3);
  show("const object prints", frozen);
}

// ── 2. Both grades are validated, on both sides ─────────────────────────────
static void demo_bounds() {
  section(2, "GRADE BOUNDS ARE ENFORCED AT CONSTRUCTION");
  note("Same scale as Bureaucrat: below 1 is 'too high', above 150 is 'too low'.");
  note("Both grades are checked, independently — not just the first one.");
  probe(1, 1);
  probe(150, 150);
  probe(0, 10);
  probe(10, 0);
  probe(151, 10);
  probe(10, 151);
  note("Validation runs inside the initializer list, so a rejected form never");
  note("exists at all — there is no half-built object to clean up.");
}

// ── 3. beSigned(): the rank rule ────────────────────────────────────────────
static void demo_be_signed() {
  section(3, "beSigned() — LOWER NUMBER MEANS HIGHER RANK");
  note("A form requiring grade 50 is signable by grades 1..50, refused by 51+.");
  Form f("Requisition", 50, 25);
  show("start", f);

  Bureaucrat tooLow("Clerk", 51);
  try {
    f.beSigned(tooLow);
    show("unreachable", f);
  } catch (std::exception& e) {
    caught("grade 51 vs required 50", e);
  }
  note("The throw happens before the assignment, so nothing moved:");
  show("still unsigned", f);

  note("Equal grade is enough — the subject says 'greater than or equal':");
  Bureaucrat exact("Exact", 50);
  f.beSigned(exact);
  show("after grade 50 signs", f);

  note("Signing an already-signed form is a harmless no-op:");
  Bureaucrat boss("Boss", 1);
  f.beSigned(boss);
  show("after a second sign", f);
}

// ── 4. The boundary, one grade at a time ────────────────────────────────────
static void demo_boundary() {
  section(4, "THE EXACT BOUNDARY AROUND gradeToSign");
  note("Required grade is 100. Watch where 'accepted' flips to 'refused'.");
  for (int g = 98; g <= 102; ++g) {
    Form f("Boundary", 100, 100);
    Bureaucrat b("Grade", g);
    std::cout << "   bureaucrat grade " << std::right << std::setw(4) << g << "  ";
    try {
      f.beSigned(b);
      std::cout << GREEN << "signs" << RESET << std::endl;
    } catch (std::exception&) {
      std::cout << RED << "refused" << RESET << std::endl;
    }
  }
}

// ── 5. signForm(): the two required messages ────────────────────────────────
static void demo_sign_form() {
  section(5, "Bureaucrat::signForm() — REPORTS, NEVER THROWS");
  note("Required output: '<bureaucrat> signed <form>'");
  note("             or: '<bureaucrat> couldn't sign <form> because <reason>.'");
  std::cout << std::endl;

  Bureaucrat alice("Alice", 3);
  Form top("TopSecret", 2, 1);
  showB("the signer", alice);
  show("the form", top);
  std::cout << std::endl;

  note("grade 3 against a form requiring 2 — refused, with a reason:");
  alice.signForm(top);

  note("after a promotion she ranks exactly high enough:");
  alice.incrementGrade();
  showB("promoted", alice);
  alice.signForm(top);
  show("the form is now", top);

  note("signForm() swallows the exception, so no try/catch is needed here.");
}

// ── 6. Orthodox Canonical Form ──────────────────────────────────────────────
static void demo_ocf() {
  section(6, "ORTHODOX CANONICAL FORM");
  Form source("Source", 30, 20);
  Bureaucrat boss("Boss", 1);
  boss.signForm(source);
  show("source (signed)", source);

  Form copy(source);
  show("copy(source)", copy);
  note("The copy carries the signature and every const attribute.");

  note("Assignment can only move the signed flag — name and grades are const:");
  Form target("Target", 90, 80);
  show("target before", target);
  target = source;
  show("target = source", target);
  note("Note the name stayed 'Target' and the grades stayed 90/80 — only the");
  note("signature crossed over. That is what `const` on those members means.");

  note("operator= returns *this, so assignments chain, and self-assignment");
  note("is guarded and harmless:");
  Form& alias = target;
  target = alias;
  show("target = target", target);
}

// ── 7. Exception mechanics ──────────────────────────────────────────────────
static void demo_exceptions() {
  section(7, "EXCEPTIONS ARE CATCHABLE, AND DISTINCT");
  note("Form's exceptions are their own types, separate from Bureaucrat's.");
  try {
    Form f("TooGood", 0, 10);
  } catch (Form::GradeTooHighException& e) {
    handled("caught by exact type", e);
  } catch (std::exception&) {
    std::cout << "   " << RED << "wrong handler!" << RESET << std::endl;
  }

  try {
    Form f("TooBad", 10, 151);
  } catch (Form::GradeTooHighException&) {
    std::cout << "   " << RED << "wrong handler!" << RESET << std::endl;
  } catch (std::exception& e) {
    handled("caught by base reference", e);
  }

  note("A Form exception is not a Bureaucrat exception, even though the two");
  note("classes spell them the same way:");
  try {
    Form f("Nope", 200, 5);
  } catch (Bureaucrat::GradeTooLowException&) {
    std::cout << "   " << RED << "wrong handler!" << RESET << std::endl;
  } catch (Form::GradeTooLowException& e) {
    handled("Form::GradeTooLow wins", e);
  }
}

// ── 8. What ex01 deliberately does not do yet ───────────────────────────────
static void demo_not_yet() {
  section(8, "STORED BUT UNUSED — gradeToExecute");
  note("ex01 stores the execution grade and prints it, but nothing executes a");
  note("form yet. That is ex02's job, where Form becomes the abstract AForm.");
  Form f("Shrubbery", 145, 137);
  show("ready for ex02", f);
}

int main() {
  rule(CYAN);
  std::cout << BOLD << " CPP MODULE 05 · ex01 · Form up, maggots!" << RESET << std::endl;
  rule(CYAN);

  demo_construction();
  demo_bounds();
  demo_be_signed();
  demo_boundary();
  demo_sign_form();
  demo_ocf();
  demo_exceptions();
  demo_not_yet();

  std::cout << std::endl;
  rule(CYAN);
  std::cout << BOLD << " Assertions: " << RESET << "make test" << std::endl;
  rule(CYAN);
  return (0);
}
