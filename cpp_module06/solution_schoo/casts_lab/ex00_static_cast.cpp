/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ex00_static_cast.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 00:00:00 by marvin            #+#    #+#             */
/*   Updated: 2026/08/11 00:00:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// LAB FOR ex00 -- static_cast
//
// static_cast is the "I know the types are related, convert between them"
// cast. It is checked at COMPILE time and never at RUN time. That single
// sentence is the whole lesson: every trap below is a case where the compile
// time check passes and there is no run time check left to save you.
//
// Build + run:  ./run.sh          (from cpp_module06/casts_lab/)
// Or by hand:   c++ -std=c++98 -Wall -Wextra ex00_static_cast.cpp -o /tmp/s
//               c++ -std=c++98 -g -fsanitize=undefined ex00_static_cast.cpp -o /tmp/s_ub

#include <cmath>
#include <cstdio>
#include <iostream>
#include <limits>
#include <string>

// ---------------------------------------------------------------------------
// Lab classes: a normal-looking single-inheritance hierarchy.
// ---------------------------------------------------------------------------

class Shape {
 public:
  Shape() : shapeTag(1111) {}
  virtual ~Shape() {}
  virtual const char* name() const { return "Shape"; }
  int shapeTag;
};

class Circle : public Shape {
 public:
  Circle() : radius(7.0), circleTag(2222) {}
  virtual const char* name() const { return "Circle"; }
  double area() const { return 3.14159265358979 * radius * radius; }
  double radius;
  int circleTag;
};

class Square : public Shape {
 public:
  Square() : side(3.0), squareTag(3333) {}
  virtual const char* name() const { return "Square"; }
  double side;
  int squareTag;
};

// Multiple inheritance: this is where a Derived* and its Base* subobject do
// NOT live at the same address, which is the thing ex01 will abuse.
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
// 1. Numeric narrowing: lossy, but well defined. The benign end of the scale.
// ---------------------------------------------------------------------------
static void scenario_narrowing() {
  title("1. static_cast on numbers -- lossy but DEFINED");

  double d = 3.99;
  std::cout << "  A) static_cast<int>(3.99)      = " << static_cast<int>(d)
            << "   <- truncates toward zero, does NOT round" << std::endl;
  std::cout << "  B) static_cast<int>(-3.99)     = " << static_cast<int>(-d)
            << "  <- toward zero again, so -3 not -4" << std::endl;

  int big = 200;
  std::cout << "  C) static_cast<char>(200)      = "
            << static_cast<int>(static_cast<char>(big))
            << " <- char is signed here, so 200 wraps negative" << std::endl;

  verdict(true,
          "Defined behaviour. You lose data, but the program stays honest.");
  std::cout << "     This is the case ScalarConverter handles with its range\n"
               "     checks before printing 'impossible'."
            << std::endl;
}

// ---------------------------------------------------------------------------
// 2. Out-of-range double -> int. THE bug from ScalarConverter's history.
// ---------------------------------------------------------------------------
static void scenario_out_of_range() {
  title("2. static_cast<int>(huge double) -- UNDEFINED BEHAVIOUR");

  double huge = 1e20;  // far past INT_MAX

  std::cout << "  A) unguarded: static_cast<int>(1e20)" << std::endl;
  // This line is UB. It does not crash; it quietly produces a number.
  int bad = static_cast<int>(huge);
  std::cout << "     -> " << bad
            << "   (looks like an answer, is actually garbage)" << std::endl;
  verdict(false,
          "No crash, no warning at runtime. UBSan is what makes it visible.");

  std::cout << "\n  B) guarded: range-check first, exactly like convert() does"
            << std::endl;
  if (huge < static_cast<double>(std::numeric_limits<int>::min()) ||
      huge > static_cast<double>(std::numeric_limits<int>::max())) {
    std::cout << "     -> int: impossible" << std::endl;
  } else {
    std::cout << "     -> " << static_cast<int>(huge) << std::endl;
  }
  verdict(true, "The check is the whole fix. static_cast will not do it.");

  std::cout << "\n  DEBUG: build with -fsanitize=undefined and rerun. Case A\n"
               "         reports 'runtime error: 1e+20 is outside the range of\n"
               "         representable values of type int'. Case B stays quiet."
            << std::endl;
}

// ---------------------------------------------------------------------------
// 3. Downcasting a class pointer with no runtime check. The real trap.
// ---------------------------------------------------------------------------
static void scenario_bad_downcast() {
  title("3. static_cast downcast on the WRONG type -- silent corruption");

  Shape* s = new Square();  // the object really is a Square
  std::cout << "  the object is really a: " << s->name() << std::endl;

  std::cout << "\n  A) static_cast<Circle*> -- compiles, runs, lies"
            << std::endl;
  Circle* wrong = static_cast<Circle*>(s);
  std::cout << "     wrong->radius      = " << wrong->radius
            << "   <- this is Square::side's bytes reinterpreted" << std::endl;
  std::cout << "     wrong->circleTag   = " << wrong->circleTag
            << "  <- reading past the real object" << std::endl;
  std::cout << "     wrong->area()      = " << wrong->area()
            << "  <- a confident, meaningless number" << std::endl;
  verdict(false,
          "Compiler is happy: Circle IS related to Shape, so the cast is\n"
          "           legal. It just is not TRUE. Nothing checks that.");

  std::cout << "\n  B) dynamic_cast<Circle*> -- asks the object what it is"
            << std::endl;
  Circle* checked = dynamic_cast<Circle*>(s);
  std::cout << "     dynamic_cast<Circle*>(s) = "
            << (checked ? "non-null" : "NULL  <- refused, correctly")
            << std::endl;
  if (!checked) std::cout << "     -> take the other branch, no corruption"
                          << std::endl;
  verdict(true, "Costs a few ns. Buys you the truth.");

  std::cout << "\n  RULE: static_cast DOWN a hierarchy is only safe when you\n"
               "        already know the dynamic type by other means. If you\n"
               "        are asking 'what is it?', that is dynamic_cast's job."
            << std::endl;

  std::cout << "\n  DEBUG: -fsanitize=undefined DOES catch this one:\n"
               "           runtime error: downcast of address 0x... which does\n"
               "           not point to an object of type 'Circle'\n"
               "         UBSan's vptr check consults the same type info that\n"
               "         dynamic_cast would have. Think of it as dynamic_cast's\n"
               "         answer arriving after the crime, and only in builds\n"
               "         you remembered to instrument. Cheaper to just ask."
            << std::endl;

  delete s;
}

// ---------------------------------------------------------------------------
// 4. Multiple inheritance: static_cast does pointer arithmetic for you.
// ---------------------------------------------------------------------------
static void scenario_multiple_inheritance() {
  title("4. Multiple inheritance -- static_cast ADJUSTS the address");

  Car car;
  Car* c = &car;

  Engine* e = static_cast<Engine*>(c);
  Radio* r = static_cast<Radio*>(c);

  std::printf("  Car*    = %p\n", static_cast<void*>(c));
  std::printf("  Engine* = %p   (same address: first base)\n",
              static_cast<void*>(e));
  std::printf("  Radio*  = %p   (SHIFTED by %ld bytes: second base)\n",
              static_cast<void*>(r),
              static_cast<long>(reinterpret_cast<char*>(r) -
                                reinterpret_cast<char*>(c)));

  std::cout << "\n  r->radioTag = " << r->radioTag << "  (correct)"
            << std::endl;
  verdict(true,
          "static_cast knows the layout and applies the offset. This is\n"
          "           the service it provides that reinterpret_cast does not.");

  std::cout << "\n  Remember this number. ex01 shows what happens when you\n"
               "  skip that adjustment with reinterpret_cast."
            << std::endl;
}

// ---------------------------------------------------------------------------
// 5. What static_cast REFUSES -- the compile-time safety net.
// ---------------------------------------------------------------------------
static void scenario_refusals() {
  title("5. What static_cast will NOT let you do (this is a feature)");

  std::cout << "  These do not compile. run.sh compiles them for real so you\n"
               "  can read the actual clang error:\n"
            << std::endl;
  std::cout << "    Shape* s = ...;\n"
               "    Engine* e = static_cast<Engine*>(s);   // unrelated types\n"
            << std::endl;
  std::cout << "    int i = 42;\n"
               "    double* p = static_cast<double*>(&i);  // unrelated ptrs\n"
            << std::endl;
  verdict(true,
          "static_cast refusing to compile is it doing its job. When you\n"
          "           'fix' such an error by switching to reinterpret_cast,\n"
          "           you have not solved anything -- you have silenced the\n"
          "           only thing that was checking you.");
}

int main() {
  std::cout << "\033[1m### ex00 LAB: static_cast ###\033[0m" << std::endl;
  std::cout << "static_cast is checked at COMPILE time, never at RUN time."
            << std::endl;

  scenario_narrowing();
  scenario_out_of_range();
  scenario_bad_downcast();
  scenario_multiple_inheritance();
  scenario_refusals();

  std::cout << "\n\033[1m== Summary ==\033[0m\n" << std::endl;
  std::cout << "  USE static_cast for: numeric conversions, upcasts\n"
               "                       (Derived* -> Base*), void* -> T* that\n"
               "                       you yourself put there, and downcasts\n"
               "                       where the dynamic type is already known.\n"
               "  DO NOT use it for:   'is this object a Derived?' -- that is\n"
               "                       dynamic_cast. And range-check numbers\n"
               "                       yourself; the cast will not.\n"
            << std::endl;
  return 0;
}
