/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 18:00:25 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/04 20:44:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Fixed.hpp"

#define BOLD    "\033[1m"
#define MAGENTA "\033[35m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"

// ─────────────────────────────────────────────────────────────────────────────
// DEMO SELECTOR — uncomment exactly ONE line to run that demo after main().
// Leave all commented for the clean 42-subject output.
//
//   DEMO 1 — Default ctor      shows Fixed() called and destroyed
//   DEMO 2 — Copy ctor         Fixed b(a)  → copy constructor fires
//   DEMO 3 — Assignment        a = b       → operator= fires
//   DEMO 4 — Self-assign       a = a       → guard skips body safely
//   DEMO 5 — Pass by value     foo(Fixed x) → hidden copy ctor + destructor
//   DEMO 6 — Chain assignment  a = b = c   → operator= returns *this
// ─────────────────────────────────────────────────────────────────────────────
// #define ACTIVE_DEMO 1   // Default ctor lifecycle
// #define ACTIVE_DEMO 2   // Copy constructor
// #define ACTIVE_DEMO 3   // Assignment operator
// #define ACTIVE_DEMO 4   // Self-assign guard
// #define ACTIVE_DEMO 5   // Pass by value (hidden copy)
// #define ACTIVE_DEMO 6   // Chain assignment
// ─────────────────────────────────────────────────────────────────────────────

#if   ACTIVE_DEMO == 1
# define DEMO_DEFAULT_CTOR
#elif ACTIVE_DEMO == 2
# define DEMO_COPY_CTOR
#elif ACTIVE_DEMO == 3
# define DEMO_ASSIGNMENT
#elif ACTIVE_DEMO == 4
# define DEMO_SELF_ASSIGN
#elif ACTIVE_DEMO == 5
# define DEMO_PASS_BY_VALUE
#elif ACTIVE_DEMO == 6
# define DEMO_CHAIN_ASSIGN
#endif

// ─────────────────────────────────────────────────────────────────────────────
// DEMO 1 — Default constructor lifecycle
// ─────────────────────────────────────────────────────────────────────────────
#ifdef DEMO_DEFAULT_CTOR
/**
 * @brief DEMO 1 — What happens when you just write `Fixed a;`
 *
 * @details
 * The four OCF members that fire here:
 *   1) Default constructor  → on entry (a is created)
 *   2) Destructor           → on scope exit (a goes out of scope)
 *
 * rawBits starts at 0 — proven by the explicit initialiser `: _fixedPointValue(0)`.
 * Without it, the int could hold any garbage from the stack.
 *
 * Notice how the destructor fires in REVERSE construction order when
 * multiple objects are created in the same scope.
 */
static void defaultCtorDemo(void) {
  std::cout << BOLD << BLUE
    << "\n╔══════════════════════════════════════════════════╗\n"
    << "║  DEMO 1 — Default constructor lifecycle          ║\n"
    << "╚══════════════════════════════════════════════════╝\n"
    << RESET;

  std::cout << "\n  [creating Fixed a]\n";
  {
    Fixed a;
    std::cout << "  a.getRawBits() = " << a.getRawBits()
              << "  (should be 0 — never garbage)\n";
  }  // ← destructor fires here
  std::cout << "  [a is gone — destructor was called above]\n";

  std::cout << "\n  [creating Fixed x, y, z — watch destruction ORDER]\n";
  {
    Fixed x;
    Fixed y;
    Fixed z;
    std::cout << "  all three created — now leaving scope...\n";
  }  // z destroyed, then y, then x  (LIFO — stack discipline)
  std::cout << "  [z, y, x destroyed in reverse order — LIFO]\n";
}
#endif  // DEMO_DEFAULT_CTOR

// ─────────────────────────────────────────────────────────────────────────────
// DEMO 2 — Copy constructor
// ─────────────────────────────────────────────────────────────────────────────
#ifdef DEMO_COPY_CTOR
/**
 * @brief DEMO 2 — Copy constructor: `Fixed b(a)`
 *
 * @details
 * The copy constructor fires whenever a new object is initialised FROM
 * an existing one.  The compiler calls it in three situations:
 *
 *   Fixed b(a);          // explicit copy construction
 *   Fixed b = a;         // copy-initialisation (same as above)
 *   void foo(Fixed x);   // pass by value → copy construction on entry
 *
 * Here _fixedPointValue is a plain int on the stack.
 * Each object has its own independent copy — changing one does NOT
 * affect the other.  No heap, no pointer aliasing possible.
 *
 * When heap is involved (int*), the copy constructor MUST allocate a new
 * int and copy the value — not copy the pointer — to avoid aliasing.
 */
static void copyCtrDemo(void) {
  std::cout << BOLD << BLUE
    << "\n╔══════════════════════════════════════════════════╗\n"
    << "║  DEMO 2 — Copy constructor  Fixed b(a)          ║\n"
    << "╚══════════════════════════════════════════════════╝\n"
    << RESET;

  std::cout << "\n  [creating Fixed a via default ctor]\n";
  Fixed a;
  a.setRawBits(42);
  std::cout << "  a.getRawBits() = " << a.getRawBits() << "\n";

  std::cout << "\n  [Fixed b(a)  → copy constructor]\n";
  Fixed b(a);
  std::cout << "  b.getRawBits() = " << b.getRawBits()
            << "  (same value, independent copy)\n";

  std::cout << "\n  [b.setRawBits(99) — does NOT touch a]\n";
  b.setRawBits(99);
  std::cout << "  a.getRawBits() = " << a.getRawBits() << "  (unchanged ✓)\n";
  std::cout << "  b.getRawBits() = " << b.getRawBits() << "\n";

  std::cout << "\n  [leaving scope — b destroyed first, then a]\n";
}
#endif  // DEMO_COPY_CTOR

// ─────────────────────────────────────────────────────────────────────────────
// DEMO 3 — Assignment operator
// ─────────────────────────────────────────────────────────────────────────────
#ifdef DEMO_ASSIGNMENT
/**
 * @brief DEMO 3 — `operator=`: overwrites an already-existing object.
 *
 * @details
 * KEY DIFFERENCE from the copy constructor:
 *   - Copy ctor   → destination does NOT yet exist
 *   - operator=   → destination ALREADY EXISTS (may hold resources)
 *
 * When the class owns heap memory, operator= must:
 *   1) Guard against self-assignment  (if this == &other: do nothing)
 *   2) Free the destination's current resource
 *   3) Allocate a new resource and deep-copy the value
 *   4) Return *this (enables chaining: a = b = c)
 *
 * Here we only have a plain int, so steps 2 & 3 reduce to a simple copy.
 * The self-assignment guard and the return *this are still mandatory habits.
 */
static void assignmentDemo(void) {
  std::cout << BOLD << BLUE
    << "\n╔══════════════════════════════════════════════════╗\n"
    << "║  DEMO 3 — Assignment operator  a = b            ║\n"
    << "╚══════════════════════════════════════════════════╝\n"
    << RESET;

  Fixed a;
  Fixed b;
  b.setRawBits(77);

  std::cout << "\n  before: a=" << a.getRawBits()
            << "  b=" << b.getRawBits() << "\n";

  std::cout << "\n  [a = b  → operator= fires]\n";
  a = b;
  std::cout << "  after:  a=" << a.getRawBits()
            << "  b=" << b.getRawBits() << "\n";

  std::cout << "\n  [a.setRawBits(0) — does NOT touch b]\n";
  a.setRawBits(0);
  std::cout << "  a=" << a.getRawBits() << "  b=" << b.getRawBits()
            << "  (independent ✓)\n";
}
#endif  // DEMO_ASSIGNMENT

// ─────────────────────────────────────────────────────────────────────────────
// DEMO 4 — Self-assignment guard
// ─────────────────────────────────────────────────────────────────────────────
#ifdef DEMO_SELF_ASSIGN
/**
 * @brief DEMO 4 — Self-assignment: `a = a` must be a safe no-op.
 *
 * @details
 * The self-assignment guard `if (this != &other)` is NOT optional.
 *
 * With a plain int it seems harmless to skip it — you just overwrite
 * the value with itself.  But with heap memory the sequence without a guard is:
 *
 *   1) delete _raw;            // free own memory
 *   2) _raw = new int(*other._raw);  // other._raw was JUST freed → UB
 *
 * Building the habit now prevents that bug later.
 * The guard checks POINTER equality — same object in memory → skip the body.
 */
static void selfAssignDemo(void) {
  std::cout << BOLD << BLUE
    << "\n╔══════════════════════════════════════════════════╗\n"
    << "║  DEMO 4 — Self-assignment guard  (a = a)        ║\n"
    << "╚══════════════════════════════════════════════════╝\n"
    << RESET;

  Fixed a;
  a.setRawBits(55);
  std::cout << "\n  a = " << a.getRawBits() << "  at address " << &a << "\n";

  std::cout << "\n  [a = a  → operator= fires, guard detects this == &other]\n";
  a = a;  // operator= prints "Copy assignment operator called" then skips body
  std::cout << "  a = " << a.getRawBits()
            << (a.getRawBits() == 55 ? "  (value preserved ✓)" : "  (CORRUPTED ✗)")
            << "\n";
}
#endif  // DEMO_SELF_ASSIGN

// ─────────────────────────────────────────────────────────────────────────────
// DEMO 5 — Pass by value (hidden copy constructor)
// ─────────────────────────────────────────────────────────────────────────────
#ifdef DEMO_PASS_BY_VALUE
/**
 * @brief DEMO 5 — Passing Fixed by value triggers a hidden copy constructor.
 *
 * @details
 * Every function call that takes an argument by value fires the copy
 * constructor on entry and the destructor on exit.  This is invisible in
 * the source but very visible in the constructor/destructor trace.
 *
 * With a properly-implemented copy constructor the parameter `x` is a
 * fully independent object.  Mutations inside the function do NOT affect
 * the caller's object.
 *
 * Contrast with pass-by-reference (`const Fixed& x`) which fires no
 * constructor at all — just passes the address.
 */
static void byValueFunction(Fixed x) {
  std::cout << "    [inside byValueFunction] x.getRawBits()="
            << x.getRawBits() << "\n";
  x.setRawBits(999);  // does NOT affect the caller's object
  std::cout << "    [modified x to 999 — caller won't see this]\n";
  std::cout << "    [returning — destructor fires on x here]\n";
}  // ← destructor fires on x here

static void passByValueDemo(void) {
  std::cout << BOLD << BLUE
    << "\n╔══════════════════════════════════════════════════╗\n"
    << "║  DEMO 5 — Pass by value (hidden copy ctor)      ║\n"
    << "╚══════════════════════════════════════════════════╝\n"
    << RESET;

  Fixed a;
  a.setRawBits(42);
  std::cout << "\n  caller: a.getRawBits()=" << a.getRawBits() << "\n";

  std::cout << "\n  [calling byValueFunction(a)  → copy ctor fires]\n";
  byValueFunction(a);

  std::cout << "  back in caller: a.getRawBits()=" << a.getRawBits()
            << (a.getRawBits() == 42 ? "  (unchanged ✓)" : "  (WRONG ✗)") << "\n";
}
#endif  // DEMO_PASS_BY_VALUE

// ─────────────────────────────────────────────────────────────────────────────
// DEMO 6 — Chain assignment  a = b = c
// ─────────────────────────────────────────────────────────────────────────────
#ifdef DEMO_CHAIN_ASSIGN
/**
 * @brief DEMO 6 — Chain assignment: `a = b = c` works because `operator=`
 *        returns `*this`.
 *
 * @details
 * `a = b = c` is right-associative and parsed as `a = (b = c)`.
 *
 * Step by step:
 *   1) `b = c`  → operator= on b fires, copies c's raw bits, returns b&
 *   2) `a = b`  → operator= on a fires, copies b's raw bits, returns a&
 *
 * If operator= returned `void` instead of `Fixed&`, step 2 would have
 * nothing to assign and the chain would fail to compile.
 *
 * The return type `Fixed&` is not a convenience — it is a contract.
 */
static void chainAssignDemo(void) {
  std::cout << BOLD << BLUE
    << "\n╔══════════════════════════════════════════════════╗\n"
    << "║  DEMO 6 — Chain assignment  a = b = c           ║\n"
    << "╚══════════════════════════════════════════════════╝\n"
    << RESET;

  Fixed a;
  Fixed b;
  Fixed c;
  c.setRawBits(123);

  std::cout << "\n  before: a=" << a.getRawBits()
            << "  b=" << b.getRawBits()
            << "  c=" << c.getRawBits() << "\n";

  std::cout << "\n  [a = b = c  → two operator= calls, right-to-left]\n";
  a = b = c;

  std::cout << "\n  after:  a=" << a.getRawBits()
            << "  b=" << b.getRawBits()
            << "  c=" << c.getRawBits()
            << "  (all 123 ✓)\n";
}
#endif  // DEMO_CHAIN_ASSIGN

// ─────────────────────────────────────────────────────────────────────────────
// NORMAL MAIN — 42 subject expected output
// ─────────────────────────────────────────────────────────────────────────────
int main(void) {
  std::cout << BOLD << MAGENTA << "--- Initialization Phase ---" << RESET << std::endl;
  Fixed a;
  Fixed b(a);
  Fixed c;

  std::cout << BOLD << MAGENTA << "\n--- Assignment Phase ---" << RESET << std::endl;
  c = b;

  std::cout << BOLD << MAGENTA << "\n--- Data Access Phase ---" << RESET << std::endl;
  std::cout << "Value of a: " << a.getRawBits() << std::endl;
  std::cout << "Value of b: " << b.getRawBits() << std::endl;
  std::cout << "Value of c: " << c.getRawBits() << std::endl;

  std::cout << BOLD << MAGENTA << "\n--- Destruction Phase ---" << RESET << std::endl;

#ifdef DEMO_DEFAULT_CTOR
  defaultCtorDemo();
#endif
#ifdef DEMO_COPY_CTOR
  copyCtrDemo();
#endif
#ifdef DEMO_ASSIGNMENT
  assignmentDemo();
#endif
#ifdef DEMO_SELF_ASSIGN
  selfAssignDemo();
#endif
#ifdef DEMO_PASS_BY_VALUE
  passByValueDemo();
#endif
#ifdef DEMO_CHAIN_ASSIGN
  chainAssignDemo();
#endif

  return 0;
}
