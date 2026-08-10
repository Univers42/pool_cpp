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

// ─────────────────────────────────────────────────────────────────────────────
//  WHICH OF THE TWO execute() TECHNIQUES THIS EXERCISE USES
//
//  The subject accepts either one:
//
//    (A) execute() is PURE VIRTUAL, and every concrete form implements it and
//        repeats the signature/grade checks itself.
//
//    (B) execute() lives in the base class, performs the checks there, then
//        calls another method in the derived class that only executes the
//        action.
//
//  >>> THIS CODE USES TECHNIQUE (B). <<<
//
//  Concretely:
//
//    AForm::execute(Bureaucrat const& executor) const   [AForm.hpp:88]
//        PUBLIC and NON-VIRTUAL. It runs both preconditions, in this order:
//          1. the form must be signed      -> throws AForm::NotSignedException
//          2. executor's grade must reach _gradeToExecute
//                                          -> throws AForm::GradeTooLowException
//        and only if both pass does it call the hook below.
//
//    AForm::executeAction(Bureaucrat const& executor) const   [AForm.hpp:95]
//        PROTECTED and PURE VIRTUAL (= 0). Each concrete form implements only
//        its own behaviour — writing the tree file, making drilling noises,
//        printing the pardon — and contains no checks whatsoever.
//
//  Why (B) rather than (A): the two rules are written once and live in one
//  place, so no concrete form can forget a check, reorder the two, or drift
//  when a fourth form is added later. execute() being non-virtual means a
//  derived class cannot override the gate away; executeAction() being
//  protected means outside code cannot reach the action without passing
//  through it. Under (A) the same two checks would be copy-pasted into three
//  separate files, with three chances to get them wrong.
//
//  Both techniques are valid per the subject; this is simply the one chosen.
// ─────────────────────────────────────────────────────────────────────────────

// A guided tour of ex02: Form becomes the abstract AForm, three concrete forms
// grow real behaviour, and execute() gates them all. Every rule the subject
// states, shown working and shown failing. The assertions live in
// tests/test.cpp (make test); this file is the readable proof.

#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>

#include "AForm.hpp"
#include "Bureaucrat.hpp"
#include "PresidentialPardonForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "ShrubberyCreationForm.hpp"

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
static void show(const std::string& label, const AForm& f) { std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(22) << label << f << std::endl; }
static void showB(const std::string& label, const Bureaucrat& b) { std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(22) << label << b << std::endl; }
static void handled(const std::string& label, const std::exception& e) { std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(22) << label << DIM << e.what() << RESET << std::endl; }

// ── 1. The three concrete forms and their fixed grades ──────────────────────
static void demo_the_three() {
  section(1, "THREE CONCRETE FORMS, GRADES FIXED BY THE SUBJECT");
  note("Each takes only a target; name and both grades are baked in.");
  ShrubberyCreationForm shrub("home");
  RobotomyRequestForm robot("Bender");
  PresidentialPardonForm pardon("Arthur Dent");
  show("sign 145, exec 137", shrub);
  show("sign  72, exec  45", robot);
  show("sign  25, exec   5", pardon);
}

// ── 2. AForm cannot be instantiated ─────────────────────────────────────────
static void demo_abstract() {
  section(2, "AForm IS ABSTRACT");
  note("`AForm f;` does not compile — executeAction() is pure virtual.");
  note("The only way to hold one is through a reference or a pointer:");
  ShrubberyCreationForm shrub("garden");
  AForm& asBase = shrub;
  AForm* asPointer = &shrub;
  show("AForm& to a shrubbery", asBase);
  show("AForm* to the same", *asPointer);
  note("Both print the same object — no slicing, because neither copies it.");
}

// ── 3. execute() gates every form the same way ──────────────────────────────
static void demo_execute_gate() {
  section(3, "execute() CHECKS SIGNED, THEN GRADE — IN THE BASE CLASS");
  note("The two preconditions live in AForm::execute() and nowhere else, so");
  note("no concrete form can forget or skip one.");
  std::cout << std::endl;

  PresidentialPardonForm pardon("Ford Prefect");
  Bureaucrat boss("Boss", 1);
  show("the form", pardon);
  showB("the executor", boss);
  std::cout << std::endl;

  note("unsigned form -> NotSignedException, even for the top bureaucrat:");
  boss.executeForm(pardon);

  note("signed, but the executor must also reach the execution grade (5):");
  boss.signForm(pardon);
  Bureaucrat middle("Middle", 20);
  showB("too junior to execute", middle);
  middle.executeForm(pardon);

  note("grade 1 clears an execution grade of 5:");
  boss.executeForm(pardon);
}

// ── 4. The exact execution boundary ─────────────────────────────────────────
static void demo_boundary() {
  section(4, "THE EXACT BOUNDARY AROUND gradeToExecute");
  note("RobotomyRequestForm executes at grade 45. Watch where it flips.");
  note("(the drilling noise is muted here so the table stays readable)");
  for (int g = 44; g <= 46; ++g) {
    RobotomyRequestForm robot("Marvin");
    Bureaucrat signer("Signer", 1);
    Bureaucrat b("Grade", g);
    robot.beSigned(signer);  // silent, unlike signForm
    std::cout << "   executor grade " << std::right << std::setw(4) << g << "  ";
    // Swallow whatever the action prints; only the gate's verdict matters here.
    std::ostringstream muted;
    std::streambuf* saved = std::cout.rdbuf(muted.rdbuf());
    std::string verdict, reason;
    try {
      robot.execute(b);
      verdict = "executed";
    } catch (std::exception& e) {
      verdict = "refused";
      reason = e.what();
    }
    std::cout.rdbuf(saved);
    if (reason.empty())
      std::cout << GREEN << verdict << RESET << std::endl;
    else
      std::cout << RED << verdict << RESET << "  " << DIM << reason << RESET << std::endl;
  }
  note("Sign and execute grades are independent: a bureaucrat can be allowed");
  note("to sign a form and still be too junior to execute it.");
}

// ── 5. Each form's own behaviour ────────────────────────────────────────────
static void demo_actions() {
  section(5, "WHAT EACH FORM ACTUALLY DOES");
  Bureaucrat boss("Boss", 1);

  note("PresidentialPardonForm — prints the pardon:");
  PresidentialPardonForm pardon("Zaphod");
  boss.signForm(pardon);
  boss.executeForm(pardon);
  std::cout << std::endl;

  note("RobotomyRequestForm — drilling, then a 50/50 outcome:");
  RobotomyRequestForm robot("Bender");
  boss.signForm(robot);
  for (int i = 0; i < 4; ++i) boss.executeForm(robot);
  std::cout << std::endl;

  note("ShrubberyCreationForm — writes ./home_shrubbery, no stdout of its own:");
  ShrubberyCreationForm shrub("home");
  boss.signForm(shrub);
  boss.executeForm(shrub);
  note("(check it with: cat home_shrubbery)");
}

// ── 6. Polymorphism through the base ────────────────────────────────────────
static void demo_polymorphism() {
  section(6, "ONE LOOP, THREE BEHAVIOURS");
  note("The whole point of the abstract base: the loop below knows nothing");
  note("about which form it holds, yet each one does its own thing.");
  std::cout << std::endl;

  ShrubberyCreationForm shrub("office");
  RobotomyRequestForm robot("R2D2");
  PresidentialPardonForm pardon("Trillian");
  AForm* forms[3];
  forms[0] = &shrub;
  forms[1] = &robot;
  forms[2] = &pardon;

  Bureaucrat boss("Boss", 1);
  for (int i = 0; i < 3; ++i) {
    boss.signForm(*forms[i]);
    boss.executeForm(*forms[i]);
  }

  note("And the virtual destructor means `delete` through an AForm* runs the");
  note("concrete destructor first — no leak, nothing sliced:");
  AForm* heap = new RobotomyRequestForm("HeapBot");
  boss.signForm(*heap);
  boss.executeForm(*heap);
  delete heap;
}

// ── 7. Orthodox Canonical Form across the hierarchy ─────────────────────────
static void demo_ocf() {
  section(7, "ORTHODOX CANONICAL FORM, BASE AND DERIVED");
  Bureaucrat boss("Boss", 1);
  RobotomyRequestForm source("Original");
  boss.signForm(source);
  show("source (signed)", source);

  note("The copy ctor forwards to AForm(other), so the base half comes too:");
  RobotomyRequestForm copy(source);
  show("copy(source)", copy);

  note("Assignment carries the signature; name and grades are const and stay:");
  RobotomyRequestForm target("Target");
  show("target before", target);
  target = source;
  show("target = source", target);

  note("Self-assignment is guarded and harmless:");
  RobotomyRequestForm& alias = target;
  target = alias;
  show("target = target", target);
}

// ── 8. Exception mechanics ──────────────────────────────────────────────────
static void demo_exceptions() {
  section(8, "THE THREE WAYS A FORM SAYS NO");
  ShrubberyCreationForm shrub("nowhere");

  try {
    Bureaucrat boss("Boss", 1);
    shrub.execute(boss);
  } catch (AForm::NotSignedException& e) {
    handled("unsigned", e);
  }

  Bureaucrat boss("Boss", 1);
  boss.signForm(shrub);
  try {
    Bureaucrat junior("Junior", 150);
    shrub.execute(junior);
  } catch (AForm::GradeTooLowException& e) {
    handled("executor too junior", e);
  }

  // The third exception, GradeTooHigh, is unreachable through the concrete
  // three: their grades are hard-coded to legal values. It guards the base
  // constructor, so a future form with a bad grade is caught at birth.
  note("The third, GradeTooHigh, guards the base constructor. The concrete");
  note("three hard-code legal grades, so only a new form could trip it.");

  note("All three derive from std::exception, so one handler catches any:");
  ShrubberyCreationForm fresh("unsigned-again");
  try {
    Bureaucrat boss2("Boss", 1);
    fresh.execute(boss2);
  } catch (std::exception& e) {
    handled("caught by base ref", e);
  }
}

int main() {
  rule(CYAN);
  std::cout << BOLD << " CPP MODULE 05 · ex02 · No, you need form 28B, not 28C..." << RESET << std::endl;
  rule(CYAN);

  demo_the_three();
  demo_abstract();
  demo_execute_gate();
  demo_boundary();
  demo_actions();
  demo_polymorphism();
  demo_ocf();
  demo_exceptions();

  std::cout << std::endl;
  rule(CYAN);
  std::cout << BOLD << " Assertions: " << RESET << "make test" << std::endl;
  std::cout << BOLD << " Sabotage drill: " << RESET << "bash tests/mutants.sh" << std::endl;
  rule(CYAN);
  return (0);
}
