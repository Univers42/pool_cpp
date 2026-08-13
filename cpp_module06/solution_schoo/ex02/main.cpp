/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 22:00:39 by marvin            #+#    #+#             */
/*   Updated: 2026/08/11 00:00:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// A guided tour of ex02. generate() is random, so a plain loop can easily run
// five times without ever producing a C, and never shows the Unknown path at
// all. This walks the deterministic cases first — every type, both overloads,
// and both failure modes — then lets generate() run. The assertions live in
// tests/test.cpp (make test); this file is the readable proof.

#include <cstdlib>
#include <ctime>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "A.hpp"
#include "B.hpp"
#include "C.hpp"
#include "Functions.hpp"

// A fourth type that identify() has never been told about. It derives from
// Base, so it is a perfectly legal thing to hand to identify(), but it is not
// A, B or C — which is what makes it useful for showing the failure paths.
//
// Defined here rather than in the exercise: the subject asks for exactly three
// derived classes, so Mystery is a test fixture, not part of the deliverable.
class Mystery : public Base {};

// Catching std::exception& (from <exception>) is NOT the same as naming
// std::bad_cast (which would need the forbidden <typeinfo>). bad_cast derives
// from std::exception, so the base handler catches it and what() still reports
// what happened. This is how the throw can be shown without touching the
// banned header.
static std::string thrownBy(Base& r, int which) {
  try {
    if (which == 0) {
      A& x = dynamic_cast<A&>(r);
      (void)x;
    } else if (which == 1) {
      B& x = dynamic_cast<B&>(r);
      (void)x;
    } else {
      C& x = dynamic_cast<C&>(r);
      (void)x;
    }
    return "";
  } catch (std::exception& e) {
    return e.what();
  }
}

static bool castsTo(Base* p, int which) {
  if (which == 0) return dynamic_cast<A*>(p) != 0;
  if (which == 1) return dynamic_cast<B*>(p) != 0;
  return dynamic_cast<C*>(p) != 0;
}

static const char* RESET = "\033[0m";
static const char* BOLD = "\033[1m";
static const char* DIM = "\033[2m";
static const char* CYAN = "\033[96m";
static const char* GREEN = "\033[92m";
static const char* RED = "\033[91m";

// ── Tiny presentation helpers ───────────────────────────────────────────────
static void rule(const char* color) {
  std::cout << color;
  for (int i = 0; i < 72; ++i) std::cout << "─";
  std::cout << RESET << std::endl;
}
static void section(int n, const std::string& title) {
  std::cout << std::endl
            << BOLD << CYAN << " " << n << " · " << title << RESET << std::endl;
  rule(DIM);
}
static void note(const std::string& text) {
  std::cout << DIM << "   " << text << RESET << std::endl;
}

// identify() reports through cout, so capturing it is what lets the rows below
// line up in a table instead of scrolling past one line at a time.
static std::string byPointer(Base* p) {
  std::ostringstream out;
  std::streambuf* old = std::cout.rdbuf(out.rdbuf());
  identify(p);
  std::cout.rdbuf(old);
  std::string s = out.str();
  if (!s.empty() && s[s.length() - 1] == '\n') s.erase(s.length() - 1);
  return s;
}
static std::string byReference(Base& p) {
  std::ostringstream out;
  // instead of sending std::cout output to the terminal,  send it into out
  std::streambuf* old = std::cout.rdbuf(out.rdbuf());
  identify(p);
  //restores normal std::cout output
  std::cout.rdbuf(old);
  std::string s = out.str();
  // remove th final newlline
  if (!s.empty() && s[s.length() - 1] == '\n') s.erase(s.length() - 1);
  return s;
}

// One table row. `expected` is the answer identify() SHOULD give, which is not
// always the type name: for a plain Base the right answer is "Unknown".
static void row(const std::string& label, const std::string& expected,
                const std::string& ptr, const std::string& ref) {
  bool good = (ptr == expected) && (ref == expected);
  std::cout << "   " << (good ? GREEN : RED) << (good ? "✔ " : "✘ ") << RESET
            << std::left << std::setw(22) << label << std::setw(20)
            << ("pointer: " + ptr) << ("reference: " + ref) << std::endl;
}

// ── 1. Every concrete type, both overloads ──────────────────────────────────
static void demo_each_type() {
  section(1, "EVERY TYPE, THROUGH BOTH OVERLOADS");
  note("Held as Base*, so the static type says nothing. dynamic_cast asks the");
  note("object what it actually is.");

  A a;
  B b;
  C c;
  row("really an A", "A", byPointer(&a), byReference(a));
  row("really a B", "B", byPointer(&b), byReference(b));
  row("really a C", "C", byPointer(&c), byReference(c));
}

// ── 2. The two ways it is allowed to fail ───────────────────────────────────
static void demo_failure_paths() {
  section(2, "WHEN THE OBJECT IS NONE OF THEM");
  note("Pointer form: dynamic_cast returns NULL, so identify() falls through.");
  note("Reference form: there is no null reference, so the cast THROWS and");
  note("identify() catches it. Two different mechanisms, one answer.");
  std::cout << std::endl;

  Base plain;
  row("a plain Base", "Unknown", byPointer(&plain), byReference(plain));

  std::string nullAnswer = byPointer(NULL);
  std::cout << "   " << (nullAnswer == "Unknown" ? GREEN : RED)
            << (nullAnswer == "Unknown" ? "✔ " : "✘ ") << RESET << std::left
            << std::setw(22) << "a NULL pointer" << std::setw(20)
            << ("pointer: " + nullAnswer) << DIM
            << "(no reference form — cannot bind a null reference)" << RESET
            << std::endl;
}

// ── 3. The failure mechanisms, uncovered ────────────────────────────────────
static void demo_raw_mechanisms() {
  section(3, "THE TWO FAILURE MECHANISMS, SHOWN RAW");
  note("identify() collapses both of these into the word \"Unknown\". Here is");
  note("what actually happens underneath when a cast does not match.");
  std::cout << std::endl;

  B b;  // really a B — so every cast to A below must fail
  Base* p = &b;
  Base& r = b;

  A* asPtr = dynamic_cast<A*>(p);
  std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(30)
            << "dynamic_cast<A*>(ptr)"
            << (asPtr ? "returned non-NULL" : "returned NULL")
            << DIM << "   -> test it with if()" << RESET << std::endl;

  std::string what = thrownBy(r, 0);
  std::cout << "   " << GREEN << "✔ " << RESET << std::left << std::setw(30)
            << "dynamic_cast<A&>(ref)"
            << (what.empty() ? "did not throw" : "threw: " + what)
            << DIM << "   -> catch it" << RESET << std::endl;

  std::cout << std::endl;
  note("There is no null reference, so the reference form has no way to");
  note("report failure by returning. Throwing is the only option left, and");
  note("that is why the two overloads are written differently.");

  std::cout << std::endl;
  A* forced = static_cast<A*>(p);
  A* a = static_cast<A*>(p);
  if (a)
    std::cout << "It is an A" << std::endl;
  else
    std::cout << "It is not an A" << std::endl;
  std::cout << "   " << RED << "✘ " << RESET << std::left << std::setw(30)
            << "static_cast<A*>(ptr)" << "returned "
            << static_cast<void*>(forced) << RED << "  <- NOT NULL" << RESET
            << std::endl;
  note("static_cast performs no check at all: it hands back a non-NULL A*");
  note("aimed at a B. No NULL to test, no exception to catch, and calling an");
  note("A member through it reads whatever happens to be there. This is the");
  note("bug dynamic_cast exists to prevent.");
}

// ── 4. A type identify() has never heard of ─────────────────────────────────
static void demo_unknown_type() {
  section(4, "A TYPE identify() HAS NEVER HEARD OF");
  note("Mystery derives from Base but is not A, B or C. identify() still has");
  note("to answer, and answer honestly rather than guess.");
  std::cout << std::endl;

  Mystery m;
  row("a Mystery object", "Unknown", byPointer(&m), byReference(m));

  std::cout << std::endl;
  note("Underneath, identify() tried all three candidates and all three");
  note("failed — the pointer form by returning NULL, the reference form by");
  note("throwing:");
  std::cout << std::endl;

  Base* p = &m;
  Base& r = m;
  const char* names[3] = {"A", "B", "C"};
  for (int i = 0; i < 3; ++i) {
    std::string what = thrownBy(r, i);
    std::cout << "     " << std::left << std::setw(10)
              << (std::string("as ") + names[i]) << std::setw(24)
              << (std::string("pointer: ") +
                  (castsTo(p, i) ? "non-NULL" : "NULL"))
              << "reference: "
              << (what.empty() ? "no throw" : "threw " + what) << std::endl;
  }

  std::cout << std::endl;
  note("Three NULLs and three throws later, identify() reports Unknown. That");
  note("is the honest answer, and it is only available because dynamic_cast");
  note("is allowed to fail.");
}

// ── 5. generate() feeding identify() ────────────────────────────────────────
static void demo_generate() {
  section(5, "generate() PICKS AT RANDOM, identify() RECOVERS THE TYPE");
  note("generate() announces what it built; identify() is told nothing and");
  note("has to work it out from the object itself.");
  std::cout << std::endl;

  int seen[3] = {0, 0, 0};
  for (int i = 0; i < 90; ++i) {
    std::ostringstream chatter;  // swallow generate()'s own announcement
    std::streambuf* old = std::cout.rdbuf(chatter.rdbuf());
    Base* p = generate();
    std::cout.rdbuf(old);

    std::string built = chatter.str();  // "[generated X]\n"
    std::string truth = built.substr(11, 1);
    row("generated " + truth, truth, byPointer(p), byReference(*p));

    if (truth == "A") ++seen[0];
    if (truth == "B") ++seen[1];
    if (truth == "C") ++seen[2];
    delete p;  // safe: Base has a virtual destructor
  }

  std::cout << std::endl;
  std::cout << DIM << "   this run produced  A×" << seen[0] << "  B×" << seen[1]
            << "  C×" << seen[2] << RESET << std::endl;
}

int main() {
  std::srand(static_cast<unsigned int>(std::time(NULL)));

  std::cout << std::endl
            << BOLD << " ex02 · IDENTIFY REAL TYPE — dynamic_cast" << RESET
            << std::endl;
  rule(CYAN);

  demo_each_type();
  demo_failure_paths();
  demo_raw_mechanisms();
  demo_unknown_type();
  demo_generate();

  std::cout << std::endl;
  rule(CYAN);
  std::cout << " " << GREEN << "✔" << RESET
            << " identify(Base*) tests the cast against NULL; identify(Base&)"
            << std::endl
            << "   uses try/catch. No <typeinfo>, no typeid, no pointer in the"
            << std::endl
            << "   reference version." << std::endl
            << std::endl;
  return 0;
}
