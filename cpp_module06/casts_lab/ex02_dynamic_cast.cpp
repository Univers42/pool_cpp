/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ex02_dynamic_cast.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 00:00:00 by marvin            #+#    #+#             */
/*   Updated: 2026/08/11 00:00:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// LAB FOR ex02 -- dynamic_cast
//
// dynamic_cast is the only cast that asks the OBJECT what it is, at run time,
// and can answer "no". That answer is the entire product. It costs a lookup
// through the vtable's type info, and in exchange it is the only downcast you
// can perform without already knowing the answer.
//
// Build + run:  ./run.sh          (from cpp_module06/casts_lab/)

#include <cstdio>
#include <iostream>
#include <string>

// ---------------------------------------------------------------------------
// Lab classes
// ---------------------------------------------------------------------------

class Base {
 public:
  virtual ~Base() {}  // <- the virtual is what makes dynamic_cast possible
};

class A : public Base {
 public:
  A() : aTag(0xAAAA) {}
  void onlyA() const { std::cout << "     A::onlyA() -- real A" << std::endl; }
  int aTag;
};

class B : public Base {
 public:
  B() : bTag(0xBBBB) {}
  double payload[4];
  int bTag;
};

// Non-polymorphic on purpose: no virtual anything.
class Flat {
 public:
  int x;
};
class FlatChild : public Flat {
 public:
  int y;
};

// Multiple inheritance, for the cross-cast demo.
class Engine {
 public:
  Engine() : engineTag(0xEEEE) {}
  virtual ~Engine() {}
  int engineTag;
};
class Radio {
 public:
  Radio() : radioTag(0xDADA) {}
  virtual ~Radio() {}
  int radioTag;
};
class Car : public Engine, public Radio {
 public:
  Car() : carTag(0xCCCC) {}
  int carTag;
};

// ---------------------------------------------------------------------------

static void title(const char* s) {
  std::cout << "\n\033[1m== " << s << "\033[0m\n" << std::endl;
}

static void verdict(bool safe, const char* s) {
  std::cout << (safe ? "  \033[32m[SAFE]   \033[0m" : "  \033[31m[UNSAFE] \033[0m")
            << s << std::endl;
}

// ---------------------------------------------------------------------------
// 1. The core A/B: unchecked vs checked downcast.
// ---------------------------------------------------------------------------
static void scenario_checked_vs_unchecked() {
  title("1. The whole point -- static_cast asserts, dynamic_cast asks");

  Base* p = new B();  // really a B
  std::cout << "  the object is really a B\n" << std::endl;

  std::cout << "  A) static_cast<A*>(p) -- asserts, does not verify"
            << std::endl;
  A* forced = static_cast<A*>(p);
  std::cout << "     pointer = " << (forced ? "non-null" : "NULL")
            << "  <- ALWAYS non-null, whatever the object is" << std::endl;
  std::cout << "     forced->aTag = " << std::hex << forced->aTag << std::dec
            << "  <- B's bytes read as if they were A's" << std::endl;
  verdict(false, "You told the compiler a fact. It believed you. It was\n"
                 "           false. There is no second line of defence.");

  std::cout << "\n  B) dynamic_cast<A*>(p) -- verifies, can say no" << std::endl;
  A* checked = dynamic_cast<A*>(p);
  std::cout << "     pointer = " << (checked ? "non-null" : "NULL")
            << "        <- refused, because the object is a B" << std::endl;
  verdict(true, "A NULL you can branch on beats a wrong answer you cannot\n"
                "           detect.");

  delete p;
}

// ---------------------------------------------------------------------------
// 2. The null check that never fires. My favourite silent bug.
// ---------------------------------------------------------------------------
static void scenario_useless_null_check() {
  title("2. The guard that is always true");

  Base* p = new B();

  std::cout << "  Someone writes what looks like defensive code:\n" << std::endl;
  std::cout << "      A* a = static_cast<A*>(p);\n"
               "      if (a) {                     // looks careful\n"
               "        a->onlyA();\n"
               "      }\n"
            << std::endl;

  A* a = static_cast<A*>(p);
  if (a) {
    std::cout << "  the branch was taken:" << std::endl;
    a->onlyA();
    std::cout << "     ...on an object that is not an A." << std::endl;
  }
  verdict(false,
          "static_cast on a non-null pointer is non-null by\n"
          "           construction. The if() is decoration. Every reader of\n"
          "           this code, including future you, will see a null check\n"
          "           and assume the case is handled.");

  std::cout << "\n  The same shape with dynamic_cast is a real guard:\n"
            << std::endl;
  A* safe = dynamic_cast<A*>(p);
  if (safe) {
    safe->onlyA();
  } else {
    std::cout << "  not an A -- branch correctly skipped" << std::endl;
  }
  verdict(true, "Identical syntax, opposite value. This is the single\n"
                "           highest-yield thing to recognise in code review.");

  delete p;
}

// ---------------------------------------------------------------------------
// 3. Pointer form vs reference form.
// ---------------------------------------------------------------------------
static void scenario_pointer_vs_reference() {
  title("3. Pointer returns NULL, reference throws");

  B b;
  Base& ref = b;
  Base* ptr = &b;

  std::cout << "  A) pointer form -- test the result" << std::endl;
  A* pa = dynamic_cast<A*>(ptr);
  std::cout << "     dynamic_cast<A*>(ptr) = "
            << (pa ? "non-null" : "NULL  -> branch on it") << std::endl;
  verdict(true, "Use when 'not an A' is an expected, normal case.");

  std::cout << "\n  B) reference form -- there is no null reference, so it\n"
               "     throws std::bad_cast instead"
            << std::endl;
  try {
    A& ra = dynamic_cast<A&>(ref);
    (void)ra;
    std::cout << "     (unreachable)" << std::endl;
  } catch (...) {
    std::cout << "     caught the throw -> not an A" << std::endl;
  }
  verdict(true,
          "Use when 'not an A' is a programming error worth an\n"
          "           exception. ex02's identify(Base&) catches it because\n"
          "           the subject bans <typeinfo>, so std::bad_cast cannot\n"
          "           be named -- catch (...) is the way there.");
}

// ---------------------------------------------------------------------------
// 4. Cross-cast: dynamic_cast goes sideways, static_cast cannot.
// ---------------------------------------------------------------------------
static void scenario_cross_cast() {
  title("4. Cross-casting between siblings -- only dynamic_cast can");

  Car car;
  Engine* e = &car;  // we only hold an Engine*

  std::cout << "  We hold an Engine*. The object is really a Car, which is\n"
               "  also a Radio. Engine and Radio are unrelated to each other.\n"
            << std::endl;

  Radio* r = dynamic_cast<Radio*>(e);
  std::printf("  Engine*                  = %p\n", static_cast<void*>(e));
  std::printf("  dynamic_cast<Radio*>(e)  = %p   <- found it, and adjusted\n",
              static_cast<void*>(r));
  std::cout << "  r->radioTag = " << std::hex << (r ? r->radioTag : 0)
            << std::dec << std::endl;
  verdict(r != 0,
          "dynamic_cast walked to the complete object, found the Radio\n"
          "           subobject and returned a correctly adjusted pointer.\n"
          "           static_cast<Radio*>(e) does not even COMPILE here --\n"
          "           see run.sh's negative tests.");

  std::cout << "\n  And it still answers honestly when the sibling is absent:"
            << std::endl;
  Engine lone;
  Radio* none = dynamic_cast<Radio*>(&lone);
  std::cout << "     plain Engine -> dynamic_cast<Radio*> = "
            << (none ? "non-null" : "NULL") << std::endl;
}

// ---------------------------------------------------------------------------
// 5. Requirements and cost.
// ---------------------------------------------------------------------------
static void scenario_requirements() {
  title("5. What dynamic_cast needs, and what it costs");

  std::cout << "  REQUIREMENT: the source type must be polymorphic -- at least\n"
               "  one virtual function. Base has a virtual destructor, which is\n"
               "  why ex02 works at all.\n"
            << std::endl;
  std::cout << "      class Flat { int x; };            // no virtuals\n"
               "      dynamic_cast<FlatChild*>(flatPtr) // COMPILE ERROR\n"
            << std::endl;
  std::cout << "  If you hit that error, the fix is almost never to switch to\n"
               "  static_cast. It is to add the virtual destructor you were\n"
               "  missing -- if you are downcasting, you have a polymorphic\n"
               "  design and deleting through Base* was already UB.\n"
            << std::endl;

  FlatChild fc;
  fc.x = 1;
  fc.y = 2;
  Flat* fp = &fc;
  FlatChild* back = static_cast<FlatChild*>(fp);
  std::cout << "  static_cast on the flat hierarchy: back->y = " << back->y
            << std::endl;
  verdict(false,
          "Correct here ONLY because we happen to know fp really points\n"
          "           at a FlatChild. Point it at a plain Flat and the same\n"
          "           line reads memory that does not exist, with no way to\n"
          "           detect it -- there is no type info to consult.");

  std::cout << "\n  COST: dynamic_cast is a function call into the runtime that\n"
               "  walks type info; static_cast is zero or one add instruction.\n"
               "  Real, but measured in nanoseconds. Do not trade correctness\n"
               "  for it until a profiler tells you to."
            << std::endl;
}

int main() {
  std::cout << "\033[1m### ex02 LAB: dynamic_cast ###\033[0m" << std::endl;
  std::cout << "The only cast that asks the object what it is -- and can answer"
               " no."
            << std::endl;

  scenario_checked_vs_unchecked();
  scenario_useless_null_check();
  scenario_pointer_vs_reference();
  scenario_cross_cast();
  scenario_requirements();

  std::cout << "\n\033[1m== Summary ==\033[0m\n" << std::endl;
  std::cout << "  USE dynamic_cast when: you have a Base* / Base& and need to\n"
               "                         know the real type, and 'not that\n"
               "                         type' is a case you must handle.\n"
               "  Pointer form -> NULL on failure, branch on it.\n"
               "  Reference form -> throws std::bad_cast, catch it.\n"
               "  Needs a polymorphic source type (a virtual destructor counts).\n"
            << std::endl;
  return 0;
}
