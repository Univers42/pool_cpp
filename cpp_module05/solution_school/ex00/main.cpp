/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 19:37:29 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// A guided tour of the Bureaucrat class: every rule the subject states, shown
// working and shown failing. The assertions live in tests/test.cpp (make test);
// this file is the readable proof.

#include <iomanip>
#include <iostream>
#include <string>

#include "Bureaucrat.hpp"

// The `catch (std::exception& e)` handlers below rely on every exception in
// this exercise deriving publicly from std::exception, and on what() being
// virtual. Bureaucrat.hpp explains the mechanism in full.

static const char* RESET = "\033[0m";
static const char* BOLD = "\033[1m";
static const char* DIM = "\033[2m";
static const char* CYAN = "\033[96m";
static const char* GREEN = "\033[92m";
static const char* RED = "\033[91m";

// ── Tiny presentation helpers ───────────────────────────────────────────────
static void rule(const char* color) { std::cout << color; for (int i = 0; i < 68; ++i) std::cout << "─"; std::cout << RESET << std::endl; }
static void section(int n, const std::string& title) { std::cout << std::endl << BOLD << CYAN << " " << n << " · " << title << RESET << std::endl; rule(DIM); }
static void note(const std::string& text) { std::cout << DIM << "   " << text << RESET << std::endl; }
// const& binds to temporaries too, so `show("carol++", carol++)` works.
static void show(const std::string& label, const Bureaucrat& b) { std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(26) << label << b << std::endl; }
static void caught(const std::string& label, const std::exception& e) { std::cout << "   " << RED << "✘ " << RESET << std::left << std::setw(26) << label << RED << e.what() << RESET << std::endl; }
// The catch itself is the success here, so it gets a tick, not a cross.
static void handled(const std::string& label, const std::exception& e) { std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(26) << label << DIM << e.what() << RESET << std::endl; }

// Tries one grade and reports which side of the invariant it landed on.
static void probe(int grade) {
  std::cout << "   grade " << std::right << std::setw(4) << grade << "  ";
  try {
    Bureaucrat b("Probe", grade);
    std::cout << GREEN << "accepted" << RESET << "  " << DIM << b << RESET << std::endl;
  } catch (std::exception& e) {
    std::cout << RED << "rejected" << RESET << "  " << DIM << e.what() << RESET << std::endl;
  }
}

// ── 1. The three ways to build one ──────────────────────────────────────────
static void demo_construction() {
  section(1, "CONSTRUCTION");
  note("A bureaucrat has a const name and a grade in [1, 150]; 1 is the highest.");
  Bureaucrat nobody;
  Bureaucrat rookie("Rookie");
  Bureaucrat boss("Boss", 1);
  show("default ctor", nobody);
  show("name only (starts at 150)", rookie);
  show("name + grade", boss);
}

// ── 2. Getters and the required output format ───────────────────────────────
static void demo_output() {
  section(2, "GETTERS AND operator<<");
  note("Required format: <name>, bureaucrat grade <grade>.");
  Bureaucrat clerk("Clerk", 42);
  std::cout << "   getName()  -> " << BOLD << clerk.getName() << RESET << std::endl;
  std::cout << "   getGrade() -> " << BOLD << clerk.getGrade() << RESET << std::endl;
  std::cout << "   << clerk   -> " << BOLD << clerk << RESET << std::endl;

  // operator<< takes a const reference, so a const bureaucrat prints fine.
  const Bureaucrat frozen("Frozen", 7);
  show("const object prints too", frozen);
}

// ── 3. The invariant, probed from both sides ────────────────────────────────
static void demo_bounds() {
  section(3, "GRADE BOUNDS ARE ENFORCED AT CONSTRUCTION");
  note("Below 1 is 'too high', above 150 is 'too low' — the scale is inverted.");
  probe(-42);
  probe(0);
  probe(1);
  probe(75);
  probe(150);
  probe(151);
  probe(9000);
}

// ── 4. incrementGrade / decrementGrade ──────────────────────────────────────
static void demo_named_mutations() {
  section(4, "incrementGrade() / decrementGrade()");
  note("Grade 1 is the highest, so incrementing 3 gives 2 (subject, page 9).");
  try {
    Bureaucrat alice("Alice", 3);
    show("start", alice);
    alice.incrementGrade();
    show("after incrementGrade()", alice);
    alice.incrementGrade();
    show("after incrementGrade()", alice);
    alice.incrementGrade();  // at grade 1: throws
    show("unreachable", alice);
  } catch (std::exception& e) {
    caught("incrementing past 1", e);
  }

  try {
    Bureaucrat bob("Bob", 150);
    show("start", bob);
    bob.decrementGrade();
    show("unreachable", bob);
  } catch (std::exception& e) {
    caught("decrementing past 150", e);
  }
}

// ── 5. The same rules, through operators ────────────────────────────────────
static void demo_operators() {
  section(5, "OPERATORS ++ AND -- (SAME RULES, NICER SYNTAX)");
  note("Prefix returns the object itself; postfix returns its value from before.");
  Bureaucrat carol("Carol", 3);
  show("start", carol);
  note("prefix promotes, then hands back the new value:");
  show("++carol  returns", ++carol);
  show("  carol is now", carol);
  note("postfix promotes, but hands back the value from before:");
  show("carol++  returns", carol++);
  show("  carol is now", carol);
  note("and the same pair, downward:");
  show("--carol  returns", --carol);
  show("  carol is now", carol);
  show("carol--  returns", carol--);
  show("  carol is now", carol);

  note("Prefix returns a reference, so it chains:");
  ++(++carol);
  show("++(++carol)", carol);

  note("Both APIs share one bounds check, so they can never disagree:");
  Bureaucrat viaOperator("Twin", 42);
  Bureaucrat viaMethod("Twin", 42);
  ++viaOperator;
  viaMethod.incrementGrade();
  show("++twin", viaOperator);
  show("twin.incrementGrade()", viaMethod);
}

// ── 6. Operators respect the bounds too ─────────────────────────────────────
static void demo_operator_bounds() {
  section(6, "OPERATORS THROW AT THE BOUNDS, AND CHANGE NOTHING");
  Bureaucrat top("Top", 1);
  try {
    ++top;
    show("unreachable", top);
  } catch (std::exception& e) {
    caught("++top at grade 1", e);
  }
  show("top is untouched", top);

  Bureaucrat bottom("Bottom", 150);
  try {
    bottom--;
    show("unreachable", bottom);
  } catch (std::exception& e) {
    caught("bottom-- at grade 150", e);
  }
  show("bottom is untouched", bottom);
  note("The check runs before the assignment, so a refused move is a no-op.");
}

// ── 7. Orthodox Canonical Form ──────────────────────────────────────────────
static void demo_ocf() {
  section(7, "ORTHODOX CANONICAL FORM");
  Bureaucrat source("Source", 42);
  Bureaucrat copy(source);
  show("source", source);
  show("copy(source)", copy);

  note("The copy is a separate object — promoting it leaves the source alone:");
  ++copy;
  show("++copy", copy);
  show("source, unaffected", source);

  note("Assignment moves the grade; the name is const, so it stays:");
  Bureaucrat target("Target", 100);
  show("target before", target);
  target = source;
  show("target = source", target);

  note("operator= returns *this, so assignments chain:");
  Bureaucrat x("X", 10), y("Y", 20), z("Z", 30);
  x = y = z;
  show("x = y = z  -> x", x);
  show("           -> y", y);

  note("Self-assignment is guarded and harmless:");
  Bureaucrat& alias = target;
  target = alias;
  show("target = target", target);
}

// ── 8. Exception mechanics ──────────────────────────────────────────────────
static void demo_exceptions() {
  section(8, "EXCEPTIONS ARE CATCHABLE AS std::exception");
  note("Both types derive from std::exception, and they are distinct types.");
  try {
    Bureaucrat b("TooGood", 0);
  } catch (Bureaucrat::GradeTooHighException& e) {
    handled("caught by exact type", e);
  } catch (std::exception&) {
    std::cout << "   " << RED << "wrong handler!" << RESET << std::endl;
  }

  try {
    Bureaucrat b("TooBad", 151);
  } catch (Bureaucrat::GradeTooHighException&) {
    std::cout << "   " << RED << "wrong handler!" << RESET << std::endl;
  } catch (std::exception& e) {
    handled("caught by base reference", e);
  }
}

int main() {
  rule(CYAN);
  std::cout << BOLD << " CPP MODULE 05 · ex00 · Bureaucrat" << RESET << std::endl;
  rule(CYAN);

  demo_construction();
  demo_output();
  demo_bounds();
  demo_named_mutations();
  demo_operators();
  demo_operator_bounds();
  demo_ocf();
  demo_exceptions();

  std::cout << std::endl;
  rule(CYAN);
  std::cout << BOLD << " Assertions: " << RESET << "make test" << std::endl;
  std::cout << BOLD << " OCF sabotage drill: " << RESET << "bash tests/ocf_mutants.sh" << std::endl;
  rule(CYAN);
  return (0);
}
