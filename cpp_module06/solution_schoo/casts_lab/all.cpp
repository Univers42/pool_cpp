/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   all.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 00:00:00 by marvin            #+#    #+#             */
/*   Updated: 2026/08/11 00:00:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// WHERE static_cast IS REQUIRED, AND WHERE IT IS NOT
//
//   c++ -std=c++98 -Wall -Wextra -Werror all.cpp -o all && ./all
//
// Every "allowed" case below is real code that compiles and runs. Every
// "refused" case is quoted with the exact error clang produces, verified by
// compiling it. Nothing here is asserted from memory.
//
// The usual mental model is one question:
//
//     "Does C++ already know how to convert these two types?"
//        yes -> implicit works        no -> you must write a cast
//
// That question is right, but it is only HALF the picture, and the missing
// half is the one the 42 evaluation sheet is actually about. There are two
// independent axes:
//
//     axis 1  does the compiler DEMAND a cast?      (permission)
//     axis 2  can the VALUE change in transit?      (safety)
//
//                     | value is preserved      | value can be lost
//     ----------------+-------------------------+------------------------
//     implicit is OK  | char -> int             | double -> int      <-- !!
//                     | float -> double         | int -> char
//                     | Derived* -> Base*       | int -> float
//     ----------------+-------------------------+------------------------
//     cast required   | void* -> T* (right T)   | Base* -> Derived*
//                     | int -> enum             |
//
// The top-right box is the trap. C++ lets `int i = someDouble;` through
// without a word, and the value silently changes. So static_cast is NOT only
// "the thing you write when the compiler complains" -- in that box the
// compiler never complains, and writing static_cast is how you show the loss
// was deliberate. That is precisely why the sheet says:
//
//     "Accept the use of implicit casts for PROMOTION casts only."
//
// A promotion (char->int, float->double) is the value-preserving subset. Left
// implicit, nothing can go wrong. Everything else in the "implicit is OK"
// column can lose data, so it gets an explicit static_cast.

#include <cstddef>   // std::size_t
#include <iostream>
#include <stdint.h>  // uintptr_t (C++98: not <cstdint>, that is C++11)
#include <string>

// ─── types used throughout ──────────────────────────────────────────────────

class Base {
 public:
  Base() : tag(1) {}
  virtual ~Base() {}
  virtual const char* who() const { return "Base"; }
  int tag;
};

class Derived : public Base {
 public:
  Derived() : extra(42) {}
  virtual const char* who() const { return "Derived"; }
  int extra;
};

// Unrelated to Base. Used for the cross-cast case at the end.
class Other {
 public:
  virtual ~Other() {}
  int mark;
};

class Both : public Base, public Other {};

// Animal hierarchy for the dynamic_cast section. These live at file scope on
// purpose: declaring them inside each { } block, as a first draft naturally
// does, makes them invisible to the next block and the code stops compiling.
class Animal {
 public:
  virtual ~Animal() {}
  virtual const char* speak() const { return "..."; }
};

class Dog : public Animal {
 public:
  virtual const char* speak() const { return "Woof!"; }
  void bark() const { std::cout << "    Woof!" << std::endl; }
};

class Cat : public Animal {
 public:
  virtual const char* speak() const { return "Meow!"; }
  void meow() const { std::cout << "    Meow!" << std::endl; }
};

enum Color { RED, GREEN, BLUE };

// Two nearly identical classes: the only difference is `explicit`.
class Fahrenheit {
 public:
  Fahrenheit(double v) : t(v) {}  // converting ctor, NOT explicit
  double t;
};

class Celsius {
 public:
  explicit Celsius(double v) : t(v) {}  // explicit blocks implicit conversion
  double t;
};

static void title(const char* s) {
  std::cout << "\n\033[1m" << s << "\033[0m" << std::endl;
}

// ═══ 1. C++ ALREADY KNOWS — implicit works, static_cast is optional ═════════

static void allowedImplicitly() {
  title("1. Implicit works. static_cast here is documentation, not necessity.");

  // --- 1a. promotions: exact, so implicit is genuinely fine ---------------
  char c = 'A';
  int fromChar = c;  // integral promotion (4.5)
  std::cout << "  char 'A'   -> int    : " << fromChar
            << "     exact, this is a PROMOTION" << std::endl;

  float f = 1.5f;
  double fromFloat = f;  // floating point promotion (4.6)
  std::cout << "  float 1.5f -> double : " << fromFloat
            << "    exact, this is a PROMOTION" << std::endl;

  int i = 7;
  double widened = i;  // widening, exact for every int
  std::cout << "  int 7      -> double : " << widened << "      exact"
            << std::endl;

  // --- 1b. implicit is ALLOWED but LOSSY: the trap ------------------------
  double d = 42.9;
  int truncated = d;  // compiles silently, and the value changed
  std::cout << "\n  double 42.9 -> int   : " << truncated
            << "     <- implicit was ALLOWED, value CHANGED" << std::endl;
  std::cout << "  Write it as static_cast<int>(d) so a reader can see you\n"
               "  meant to drop the .9. The compiler will not tell them."
            << std::endl;

  // --- 1c. pointers: going UP a hierarchy is implicit ---------------------
  Derived derived;
  Base* up = &derived;  // Derived IS-A Base, so no cast is needed
  Base& upRef = derived;
  std::cout << "\n  Derived* -> Base*    : implicit, who() = " << up->who()
            << std::endl;
  std::cout << "  Derived& -> Base&    : implicit, who() = " << upRef.who()
            << std::endl;

  void* anyPtr = &derived;  // any object pointer converts to void*
  bool isNotNull = up;      // pointer -> bool
  std::cout << "  Derived* -> void*    : implicit  (" << anyPtr << ")"
            << std::endl;
  std::cout << "  Base*    -> bool     : implicit  (" << isNotNull << ")"
            << std::endl;

  // --- 1d. enum -> int is implicit, but NOT the other way round ----------
  Color col = GREEN;
  int colorIndex = col;
  std::cout << "\n  Color GREEN -> int   : " << colorIndex << "      implicit"
            << std::endl;

  // --- 1e. a non-explicit constructor is an implicit conversion ----------
  Fahrenheit temp = 98.6;  // calls Fahrenheit(double) implicitly
  std::cout << "  double -> Fahrenheit : " << temp.t
            << "   implicit, because the ctor is not `explicit`" << std::endl;
}

// ═══ 2. C++ REFUSES — static_cast is the right tool ════════════════════════

static void needsStaticCast() {
  title("2. Implicit is refused. static_cast is exactly what you reach for.");

  // --- 2a. going DOWN a hierarchy ----------------------------------------
  Derived real;
  Base* handle = &real;
  //  Derived* wrong = handle;
  //  error: cannot initialize a variable of type 'Derived *' with an lvalue
  //         of type 'Base *'
  //
  // The compiler refuses because it cannot know that `handle` really points
  // at a Derived. Here we happen to know it does, so we assert it:
  Derived* down = static_cast<Derived*>(handle);
  std::cout << "  Base* -> Derived*    : static_cast, extra = " << down->extra
            << std::endl;
  std::cout << "  ^ static_cast BELIEVES you. If the object were a plain Base\n"
               "    this line would still compile and quietly read garbage.\n"
               "    When you do not already know the type, use dynamic_cast."
            << std::endl;

  // --- 2b. void* back to a real type -------------------------------------
  int value = 1234;
  void* erased = &value;
  //  int* restored = erased;
  //  error: cannot initialize a variable of type 'int *' with an lvalue of
  //         type 'void *'
  //
  // (This one is legal in C and NOT in C++, which surprises people.)
  int* restored = static_cast<int*>(erased);
  std::cout << "\n  void* -> int*        : static_cast, *p = " << *restored
            << std::endl;

  // --- 2c. int -> enum ---------------------------------------------------
  //  Color c = 2;
  //  error: cannot initialize a variable of type 'Color' with an rvalue of
  //         type 'int'
  Color picked = static_cast<Color>(2);
  std::cout << "  int 2 -> Color       : static_cast, value = " << picked
            << std::endl;

  // --- 2d. a constructor marked `explicit` --------------------------------
  //  Celsius body = 37.0;
  //  error: no viable conversion from 'double' to 'Celsius'
  //
  // `explicit` exists precisely to force the caller to say what they mean.
  Celsius body = static_cast<Celsius>(37.0);
  std::cout << "  double -> Celsius    : static_cast, t = " << body.t
            << "   (ctor is `explicit`)" << std::endl;
}

// ═══ 3. static_cast REFUSES TOO — a different cast is required ═════════════

static void beyondStaticCast() {
  title("3. static_cast refuses. This is where the other three begin.");

  std::cout << "  These four do NOT compile with static_cast. That error is\n"
               "  the compiler telling you that you picked the wrong tool:\n"
            << std::endl;

  //  const int frozen = 42;
  //  const int* cp = &frozen;
  //  int* mp = static_cast<int*>(cp);
  //  error: static_cast from 'const int *' to 'int *' is not allowed
  std::cout << "    const int* -> int*        needs const_cast" << std::endl;

  //  int n = 0;
  //  uintptr_t raw = static_cast<uintptr_t>(&n);
  //  error: static_cast from 'int *' to 'uintptr_t' (aka 'unsigned long')
  //         is not allowed
  std::cout << "    T* -> uintptr_t           needs reinterpret_cast  (ex01)"
            << std::endl;

  //  int n = 0;
  //  double* dp = static_cast<double*>(&n);
  //  error: static_cast from 'int *' to 'double *' is not allowed
  std::cout << "    int* -> double*           needs reinterpret_cast (and is\n"
               "                              almost always a mistake)"
            << std::endl;

  //  Both object;
  //  Base* b = &object;
  //  Other* o = static_cast<Other*>(b);
  //  error: static_cast from 'Base *' to 'Other *', which are not related by
  //         inheritance, is not allowed
  //
  // Base and Other are siblings: both are bases of Both, neither derives from
  // the other. static_cast only travels up and down one hierarchy. Going
  // SIDEWAYS needs runtime knowledge of the complete object:
  Both object;
  Base* b = &object;
  Other* o = dynamic_cast<Other*>(b);
  std::cout << "    Base* -> Other* (sibling) needs dynamic_cast  -> "
            << (o ? "found it" : "NULL") << std::endl;
}


static void use_of_reinterpret() {
  title("4. reinterpret_cast — relabels bits, converts nothing");

  // --- 1. pointer -> integer -> pointer  (this is exactly ex01) ----------
  {
    int value = 42;
    uintptr_t raw = reinterpret_cast<uintptr_t>(&value);
    int* back = reinterpret_cast<int*>(raw);

    std::cout << "  &value as uintptr_t : " << raw << std::endl;
    std::cout << "  round trip back     : *back = " << *back
              << "   same object? " << (back == &value ? "yes" : "no")
              << std::endl;
    std::cout << "  ^ the ONE portable use: out and back to the SAME type."
              << std::endl;
  }

  // --- 2. pointer -> a different pointer type ----------------------------
  {
    // The address does not change. We are only telling the compiler to read
    // the bytes at that address as a different type:
    //
    //     int*  ──┐
    //             ├── same address, different interpretation
    //     char* ──┘
    //
    // static_cast<char*>(intPtr) does NOT compile: int* and char* are not
    // related by inheritance, so static_cast refuses. reinterpret_cast is the
    // only cast that will do it.
    int value = 42;
    int* intPtr = &value;
    char* charPtr = reinterpret_cast<char*>(intPtr);

    std::cout << "\n  int 42 read through char* : "
              << static_cast<int>(*charPtr) << std::endl;
    std::cout << "  ^ 42 on a little-endian machine, because the low byte\n"
                 "    comes first. On a big-endian one this prints 0."
              << std::endl;
  }

  // --- 3. object -> raw bytes --------------------------------------------
  {
    // Inspecting an object's representation through unsigned char* is one of
    // the few things the standard explicitly blesses: char types are allowed
    // to alias anything. Useful for binary formats, serialisation, hardware.
    double value = 42.42;
    unsigned char* bytes = reinterpret_cast<unsigned char*>(&value);

    std::cout << "\n  double 42.42 as raw bytes : ";
    for (std::size_t i = 0; i < sizeof(value); ++i)
      std::cout << static_cast<int>(bytes[i]) << ' ';
    std::cout << "\n  ^ " << sizeof(value) << " bytes, IEEE-754, low byte first"
              << std::endl;
  }
}

static void use_of_dynamic() {
  title("5. dynamic_cast — asks the object, and is allowed to answer no");

  // --- 1. Derived -> Base (upcast): legal, but nobody writes it -----------
  {
    Dog dog;
    Animal* viaCast = dynamic_cast<Animal*>(&dog);  // works, and is pointless
    Animal* viaImplicit = &dog;                     // what you actually write

    std::cout << "  Dog* -> Animal* : dynamic_cast and implicit agree? "
              << (viaCast == viaImplicit ? "yes" : "no") << std::endl;
    std::cout << "  ^ Going UP never needs a cast. Every Dog IS-A Animal, and\n"
                 "    the compiler already knows it."
              << std::endl;
  }

  // --- 2. Base -> Derived, and the object really is one -------------------
  {
    Dog dog;
    Animal* animal = &dog;  // static type says Animal, real type is Dog

    Dog* dogPtr = dynamic_cast<Dog*>(animal);
    std::cout << "\n  Animal* -> Dog*  (object is a Dog) : "
              << (dogPtr ? "succeeded" : "NULL") << std::endl;
    if (dogPtr) dogPtr->bark();
    std::cout << "  ^ The pointer was typed Animal*, but dynamic_cast checks\n"
                 "    the RUNTIME type, so it finds the Dog."
              << std::endl;
  }

  // --- 3. Base -> Derived, and the object is NOT one ----------------------
  {
    Dog dog;
    Animal* animal = &dog;

    Cat* catPtr = dynamic_cast<Cat*>(animal);
    std::cout << "\n  Animal* -> Cat*  (object is a Dog) : "
              << (catPtr ? "succeeded" : "NULL") << std::endl;
    if (!catPtr)  // NOTE: !catPtr. NULL is the failure, so the test is negated.
      std::cout << "    This animal is not a Cat." << std::endl;
    std::cout << "  ^ static_cast<Cat*> would have returned a non-NULL pointer\n"
                 "    here and let you call meow() on a Dog."
              << std::endl;
  }

  // --- 4. the reference form: no NULL exists, so it throws ----------------
  {
    Dog dog;
    Animal& animal = dog;

    try {
      Cat& cat = dynamic_cast<Cat&>(animal);
      (void)cat;
      std::cout << "\n  (unreachable)" << std::endl;
    } catch (...) {
      std::cout << "\n  Animal& -> Cat&  : threw, because there is no such\n"
                   "                     thing as a null reference"
                << std::endl;
    }
  }
}


// ═══ summary ═══════════════════════════════════════════════════════════════

static void summary() {
  title("Summary — verified against the compiler, not from memory");
  std::cout <<
      "  conversion                     implicit   static_cast\n"
      "  ---------------------------------------------------------\n"
      "  char -> int      (promotion)     yes         yes\n"
      "  float -> double  (promotion)     yes         yes\n"
      "  int -> double                    yes         yes\n"
      "  double -> int    (LOSSY)         yes         yes   <- use the cast\n"
      "  Derived* -> Base*  (upcast)      yes         yes\n"
      "  T* -> void*                      yes         yes\n"
      "  enum -> int                      yes         yes\n"
      "  double -> class (plain ctor)     yes         yes\n"
      "  ---------------------------------------------------------\n"
      "  Base* -> Derived* (downcast)     NO          yes\n"
      "  void* -> T*                      NO          yes\n"
      "  int -> enum                      NO          yes\n"
      "  double -> class (explicit ctor)  NO          yes\n"
      "  ---------------------------------------------------------\n"
      "  const T* -> T*                   NO          NO  -> const_cast\n"
      "  T* -> uintptr_t                  NO          NO  -> reinterpret_cast\n"
      "  int* -> double*                  NO          NO  -> reinterpret_cast\n"
      "  Base* -> sibling Other*          NO          NO  -> dynamic_cast\n"
            << std::endl;

  std::cout <<
      "  Two rules worth carrying out of this file:\n\n"
      "  1. \"It compiled\" does not mean \"the value survived\".\n"
      "     double -> int is the case that proves it. Reserve implicit for\n"
      "     PROMOTIONS -- char->int, float->double -- where the value cannot\n"
      "     change. Write static_cast everywhere else, so the loss is visible\n"
      "     to whoever reads the line next.\n\n"
      "  2. A static_cast that refuses to compile is not an obstacle, it is\n"
      "     an answer. Swapping in reinterpret_cast to make the error go away\n"
      "     removes the only thing that was checking you.\n"
            << std::endl;
}

int main() {
  std::cout << "\033[1mstatic_cast: where it is required and where it is not"
            << "\033[0m" << std::endl;
  allowedImplicitly();
  needsStaticCast();
  beyondStaticCast();
  use_of_reinterpret();
  use_of_dynamic();
  summary();
  return 0;
}
