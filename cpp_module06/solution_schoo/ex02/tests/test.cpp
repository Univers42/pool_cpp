/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 00:00:00 by marvin            #+#    #+#             */
/*   Updated: 2026/08/11 00:00:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Deterministic + fuzz tests for ex02 identify()/generate().
//
//   make test              build and run
//   ./build/bin/test       run directly
//   ./build/bin/test 1234  run with a different fuzz seed
//   ./tests/mutants.sh     prove these tests can actually fail
//
// The contract: given a Base* or Base& whose real type is A, B or C, identify
// prints that letter and nothing else. The interesting part is that it must be
// right when the STATIC type says nothing — which is what dynamic_cast is for.

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "A.hpp"
#include "B.hpp"
#include "C.hpp"
#include "Functions.hpp"

// ─── harness ────────────────────────────────────────────────────────────────

namespace t {

struct Section {
  const char* name;
  int checks;
  int failed;
};

// Fixed-size storage: the subject forbids STL containers before Module 08,
// so the harness uses plain arrays with an explicit count.
static const int kMaxSections = 16;
static const int kMaxErrors = 10;

static Section g_sections[kMaxSections];
static int g_sectionCount = 0;
static std::string g_errors[kMaxErrors];
static int g_errorCount = 0;

static void section(const char* name) {
  if (g_sectionCount >= kMaxSections) return;
  Section& s = g_sections[g_sectionCount++];
  s.name = name;
  s.checks = 0;
  s.failed = 0;
}

static void ok(bool cond, const std::string& detail) {
  Section& s = g_sections[g_sectionCount - 1];
  ++s.checks;
  if (cond) return;
  ++s.failed;
  if (g_errorCount < kMaxErrors)
    g_errors[g_errorCount++] = std::string(s.name) + " — " + detail;
}

static int report(const char* title) {
  int checks = 0, failed = 0;
  std::cout << "\n" << title << "\n" << std::endl;
  for (int i = 0; i < g_sectionCount; ++i) {
    const Section& s = g_sections[i];
    // Clamped: an unsigned 48 - len would wrap for a long name and throw.
    size_t len = std::string(s.name).length();
    std::string dots(len < 48 ? 48 - len : 1, '.');
    std::cout << "  " << s.name << " " << dots << " " << s.checks << " "
              << (s.failed ? "FAILED" : "ok") << std::endl;
    checks += s.checks;
    failed += s.failed;
  }
  std::cout << std::endl;
  if (!failed) {
    std::cout << "  " << g_sectionCount << " sections, " << checks
              << " checks, all passed" << std::endl;
    return 0;
  }
  std::cout << "  " << failed << " of " << checks << " checks FAILED"
            << std::endl
            << std::endl;
  for (int i = 0; i < g_errorCount; ++i)
    std::cout << "    " << g_errors[i] << std::endl;
  return 1;
}

}  // namespace t

// ─── capture ────────────────────────────────────────────────────────────────

// identify() reports through std::cout, so every check goes through one of
// these two. Keeping the redirection in one place is what lets the tests below
// read as plain assertions.

static std::string viaPointer(Base* p) {
  std::ostringstream out;
  std::streambuf* old = std::cout.rdbuf(out.rdbuf());
  identify(p);
  std::cout.rdbuf(old);
  return out.str();
}

static std::string viaReference(Base& p) {
  std::ostringstream out;
  std::streambuf* old = std::cout.rdbuf(out.rdbuf());
  identify(p);
  std::cout.rdbuf(old);
  return out.str();
}

// generate() announces itself on cout too; this swallows that chatter so the
// test output stays clean.
static Base* quietGenerate() {
  std::ostringstream sink;
  std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
  Base* p = generate();
  std::cout.rdbuf(old);
  return p;
}

// ─── 1. the contract, both overloads ────────────────────────────────────────

static void test_identifies_each_type() {
  t::section("each concrete type is identified");

  A a;
  B b;
  C c;

  t::ok(viaPointer(&a) == "A\n", "pointer, A");
  t::ok(viaPointer(&b) == "B\n", "pointer, B");
  t::ok(viaPointer(&c) == "C\n", "pointer, C");

  t::ok(viaReference(a) == "A\n", "reference, A");
  t::ok(viaReference(b) == "B\n", "reference, B");
  t::ok(viaReference(c) == "C\n", "reference, C");
}

static void test_static_type_is_irrelevant() {
  t::section("a Base* handle loses nothing");

  A a;
  B b;
  C c;
  Base* handles[3] = {&a, &b, &c};
  const char* want[3] = {"A\n", "B\n", "C\n"};

  for (int i = 0; i < 3; ++i) {
    t::ok(viaPointer(handles[i]) == want[i], "through Base*, case " + std::string(want[i], 1));
    t::ok(viaReference(*handles[i]) == want[i], "through Base&, case " + std::string(want[i], 1));
  }
}

static void test_heap_objects() {
  t::section("heap objects identify and destroy cleanly");

  Base* objs[3];
  objs[0] = new A();
  objs[1] = new B();
  objs[2] = new C();
  const char* want[3] = {"A\n", "B\n", "C\n"};

  for (int i = 0; i < 3; ++i) {
    t::ok(viaPointer(objs[i]) == want[i], "heap pointer");
    t::ok(viaReference(*objs[i]) == want[i], "heap reference");
  }
  // Base has a virtual destructor; deleting through Base* is what makes this
  // legal. Run under valgrind to see it actually hold.
  for (int i = 0; i < 3; ++i) delete objs[i];
}

// ─── 2. the negative cases ──────────────────────────────────────────────────

static void test_unknown() {
  t::section("something that is none of A, B, C is Unknown");

  Base plain;
  t::ok(viaPointer(&plain) == "Unknown\n", "plain Base, pointer");
  t::ok(viaReference(plain) == "Unknown\n", "plain Base, reference");
}

static void test_null_pointer() {
  t::section("NULL is Unknown, not a crash");

  t::ok(viaPointer(NULL) == "Unknown\n", "identify(NULL)");
}

// ─── 3. output shape ────────────────────────────────────────────────────────

static void test_output_shape() {
  t::section("exactly one line, no decoration");

  A a;
  B b;
  C c;
  Base plain;
  Base* all[4] = {&a, &b, &c, &plain};

  for (int i = 0; i < 4; ++i) {
    std::string s = viaPointer(all[i]);
    t::ok(!s.empty() && s[s.length() - 1] == '\n', "ends with a newline");
    int newlines = 0;
    for (size_t k = 0; k < s.length(); ++k)
      if (s[k] == '\n') ++newlines;
    t::ok(newlines == 1, "exactly one newline");
    t::ok(s == "A\n" || s == "B\n" || s == "C\n" || s == "Unknown\n",
          "output is one of the four legal strings, got [" + s + "]");
  }
}

// ─── 4. generate() ──────────────────────────────────────────────────────────

static void test_generate(int iterations) {
  t::section("generate produces only A, B or C");

  int seen[3] = {0, 0, 0};
  for (int i = 0; i < iterations; ++i) {
    Base* p = quietGenerate();
    std::string got = viaPointer(p);

    t::ok(got == "A\n" || got == "B\n" || got == "C\n",
          "generated something unidentifiable: [" + got + "]");
    if (got == "A\n") ++seen[0];
    if (got == "B\n") ++seen[1];
    if (got == "C\n") ++seen[2];

    t::ok(viaReference(*p) == got, "overloads disagree on a generated object");
    delete p;
  }

  // Not a distribution test -- just proof that no branch of generate() is
  // dead, which a fixed `return new A()` would otherwise sail past.
  t::ok(seen[0] > 0, "generate never produced an A");
  t::ok(seen[1] > 0, "generate never produced a B");
  t::ok(seen[2] > 0, "generate never produced a C");
}

// ─── 5. fuzz ────────────────────────────────────────────────────────────────

// Build a shuffled heap of objects whose real types we recorded, hold them all
// as Base*, and identify them in an order unrelated to their creation order.
// Any implementation that leans on allocation order or on the last-seen type
// falls over here.
static void fuzz_shuffled_population(int iterations) {
  t::section("fuzz: shuffled population held as Base*");

  // Heap arrays rather than a container: the size is known up front, so a
  // container buys nothing here beyond a forbidden dependency.
  Base** objs = new Base*[iterations];
  char* truth = new char[iterations];

  for (int i = 0; i < iterations; ++i) {
    int kind = std::rand() % 3;
    if (kind == 0) {
      objs[i] = new A();
      truth[i] = 'A';
    } else if (kind == 1) {
      objs[i] = new B();
      truth[i] = 'B';
    } else {
      objs[i] = new C();
      truth[i] = 'C';
    }
  }

  for (int round = 0; round < 2; ++round) {
    for (int i = 0; i < iterations; ++i) {
      int k = std::rand() % iterations;
      std::string want = std::string(1, truth[k]) + "\n";
      t::ok(viaPointer(objs[k]) == want, "pointer overload on a shuffled pick");
      t::ok(viaReference(*objs[k]) == want, "reference overload agrees");
    }
  }

  for (int i = 0; i < iterations; ++i) delete objs[i];
  delete[] objs;
  delete[] truth;
}

// Identifying an object must not change it: the same handle asked twice gives
// the same answer, and asking about a neighbour in between changes nothing.
static void fuzz_repeat_stability(int iterations) {
  t::section("fuzz: identification is stable and side-effect free");

  A a;
  B b;
  C c;
  Base* pool[3] = {&a, &b, &c};
  const char* want[3] = {"A\n", "B\n", "C\n"};

  for (int i = 0; i < iterations; ++i) {
    int x = std::rand() % 3;
    int y = std::rand() % 3;
    std::string first = viaPointer(pool[x]);
    viaPointer(pool[y]);                       // interleave a different query
    std::string again = viaPointer(pool[x]);
    t::ok(first == want[x], "first answer correct");
    t::ok(again == first, "answer unchanged after an unrelated query");
  }
}

// ─── main ───────────────────────────────────────────────────────────────────

int main(int argc, char** argv) {
  unsigned seed = 20260811u;
  int iterations = 2000;
  if (argc > 1) seed = static_cast<unsigned>(std::strtoul(argv[1], NULL, 10));
  std::srand(seed);

  test_identifies_each_type();
  test_static_type_is_irrelevant();
  test_heap_objects();
  test_unknown();
  test_null_pointer();
  test_output_shape();
  test_generate(iterations);
  fuzz_shuffled_population(iterations);
  fuzz_repeat_stability(iterations);

  std::ostringstream title;
  title << "ex02 identify / generate  (seed " << seed << ")";
  return t::report(title.str().c_str());
}
