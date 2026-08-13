/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ex01_reinterpret_cast.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 00:00:00 by marvin            #+#    #+#             */
/*   Updated: 2026/08/11 00:00:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// LAB FOR ex01 -- reinterpret_cast
//
// reinterpret_cast does not convert anything. It relabels the bits and tells
// the compiler to stop asking questions. It performs NO pointer adjustment,
// NO value conversion and NO check of any kind, at compile time or run time.
//
// It has exactly one guaranteed-portable use, and ex01's Serializer is it:
// a round trip T* -> uintptr_t -> T* back to the SAME type. Everything else
// in this file is a way to get hurt.
//
// Build + run:  ./run.sh          (from cpp_module06/casts_lab/)

#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <string>

// ---------------------------------------------------------------------------
// Lab types
// ---------------------------------------------------------------------------

struct Data {
  int id;
  double value;
  std::string name;
};

// A different struct that happens to be roughly the same size. This is the
// dangerous kind of "compatible": nothing about it is compatible.
struct Telemetry {
  double timestamp;
  int sensorId;
  std::string unit;
};

class Engine {
 public:
  Engine() : engineTag(0xEEEE) {}
  virtual ~Engine() {}
  virtual const char* who() const { return "Engine"; }
  int engineTag;
};

class Radio {
 public:
  Radio() : radioTag(0xDADA) {}
  virtual ~Radio() {}
  virtual const char* who() const { return "Radio"; }
  int radioTag;
};

class Car : public Engine, public Radio {
 public:
  Car() : carTag(0xCCCC) {}
  int carTag;
};

// Stand-in for ex01's Serializer::serialize, with Car in place of Data. Byte
// for byte the same function the subject asks you to write -- which is the
// point of scenario 3D below.
static uintptr_t Serializer_serialize(Car* ptr) {
  return reinterpret_cast<uintptr_t>(ptr);
}

// ---------------------------------------------------------------------------

static void title(const char* s) {
  std::cout << "\n\033[1m== " << s << "\033[0m\n" << std::endl;
}

static void verdict(bool safe, const char* s) {
  std::cout << (safe ? "  \033[32m[SAFE]   \033[0m" : "  \033[31m[UNSAFE] \033[0m")
            << s << std::endl;
}

// ---------------------------------------------------------------------------
// 1. The one guaranteed use: same-type round trip. This is ex01's Serializer.
// ---------------------------------------------------------------------------
static void scenario_round_trip() {
  title("1. The ONE portable use -- T* -> uintptr_t -> T*, same type");

  Data d;
  d.id = 42;
  d.value = 3.14159;
  d.name = "original";

  uintptr_t raw = reinterpret_cast<uintptr_t>(&d);
  Data* back = reinterpret_cast<Data*>(raw);

  std::printf("  &d       = %p\n", static_cast<void*>(&d));
  std::printf("  uintptr  = 0x%lx\n", static_cast<unsigned long>(raw));
  std::printf("  back     = %p\n", static_cast<void*>(back));
  std::cout << "  back->name = \"" << back->name << "\", id = " << back->id
            << std::endl;

  verdict(&d == back,
          "Guaranteed by the standard: a pointer converted to a large\n"
          "           enough integer and back compares equal to the original.\n"
          "           uintptr_t is 'large enough' by definition. This is why\n"
          "           the subject picked this exercise.");
}

// ---------------------------------------------------------------------------
// 2. Round trip into the WRONG type.
// ---------------------------------------------------------------------------
static void scenario_wrong_type() {
  title("2. Same bits, wrong type on the way out");

  Data d;
  d.id = 42;
  d.value = 3.14159;
  d.name = "original";

  uintptr_t raw = reinterpret_cast<uintptr_t>(&d);

  std::cout << "  A) deserialize as Data (correct):" << std::endl;
  Data* good = reinterpret_cast<Data*>(raw);
  std::cout << "     id = " << good->id << ", value = " << good->value
            << std::endl;
  verdict(true, "The type you put in is the type you take out.");

  std::cout << "\n  B) deserialize the SAME integer as Telemetry*:" << std::endl;
  Telemetry* wrong = reinterpret_cast<Telemetry*>(raw);
  std::printf("     wrong->timestamp = %g   <- these are Data::id's bytes\n",
              wrong->timestamp);
  std::cout << "     wrong->sensorId  = " << wrong->sensorId
            << "  <- and these are the middle of Data::value" << std::endl;
  verdict(false,
          "Compiles clean, runs, prints numbers. Every field is a lie.\n"
          "           Note we did NOT touch wrong->unit: reading that\n"
          "           std::string would treat a double as a pointer and\n"
          "           segfault or worse.");

  std::cout << "\n  This is why Serializer::deserialize returns Data* and not\n"
               "  a template. The uintptr_t carries no type information at\n"
               "  all -- keeping the type correct is 100% on you."
            << std::endl;
}

// ---------------------------------------------------------------------------
// 3. THE demo: multiple inheritance. reinterpret_cast skips the adjustment.
// ---------------------------------------------------------------------------
static void scenario_multiple_inheritance() {
  title("3. Multiple inheritance -- reinterpret_cast SKIPS the offset");

  Car car;
  Car* c = &car;

  Radio* correct = static_cast<Radio*>(c);       // adjusts
  Radio* broken = reinterpret_cast<Radio*>(c);   // does not adjust

  std::printf("  Car*                        = %p\n", static_cast<void*>(c));
  std::printf("  static_cast<Radio*>(c)      = %p   <- shifted, correct\n",
              static_cast<void*>(correct));
  std::printf("  reinterpret_cast<Radio*>(c) = %p   <- NOT shifted, wrong\n",
              static_cast<void*>(broken));
  std::printf("  offset the cast should have applied: %ld bytes\n",
              static_cast<long>(reinterpret_cast<char*>(correct) -
                                reinterpret_cast<char*>(c)));

  std::cout << "\n  A) through the correct pointer:" << std::endl;
  std::cout << "     correct->radioTag = " << std::hex << correct->radioTag
            << std::dec << "   (0xdada, as constructed)" << std::endl;
  std::cout << "     correct->who()    = " << correct->who() << std::endl;
  verdict(true, "static_cast landed on the real Radio subobject.");

  std::cout << "\n  B) through the reinterpret_cast pointer:" << std::endl;
  std::cout << "     broken->radioTag  = " << std::hex << broken->radioTag
            << std::dec << "   <- that is Engine::engineTag!" << std::endl;
  std::cout << "     broken->who()     = " << broken->who()
            << "   <- dispatched through the ENGINE vtable" << std::endl;
  verdict(false,
          "We asked a Radio* what it is and Engine answered. The object\n"
          "           is fine; the pointer is pointing at the wrong half of\n"
          "           it. No sanitizer trip, no crash -- just permanently\n"
          "           wrong answers.");

  std::cout << "\n     GOOD NEWS: clang caught this one at compile time.\n"
               "     Scroll up to the build line and read it:\n"
               "       warning: 'reinterpret_cast' from class 'Car *' to its\n"
               "       base at non-zero offset 'Radio *' behaves differently\n"
               "       from 'static_cast' [-Wreinterpret-base-class]\n"
               "     That warning is worth more than this whole lab. -Wall is\n"
               "     not decoration."
            << std::endl;

  std::cout << "\n  C) and it corrupts on write:" << std::endl;
  std::cout << "     before: engineTag = " << std::hex << car.engineTag
            << std::dec << std::endl;
  broken->radioTag = 0;  // intends to touch Radio, actually hits Engine
  std::cout << "     broken->radioTag = 0;" << std::endl;
  std::cout << "     after:  engineTag = " << std::hex << car.engineTag
            << std::dec << "        <- the WRITE went to the wrong member"
            << std::endl;
  std::cout << "             radioTag  = " << std::hex << car.radioTag
            << std::dec << "     <- untouched, as if nothing happened"
            << std::endl;
  verdict(false,
          "This is the bug class the exercise is warning you about: a\n"
          "           write through a plausible-looking pointer silently\n"
          "           damages an unrelated member. You will debug the\n"
          "           SYMPTOM (engineTag is 0) hours later and miles away.");

  // -------------------------------------------------------------------------
  // And now the part that should actually scare you.
  // -------------------------------------------------------------------------
  std::cout << "\n  D) the same bug, with the warning laundered away:"
            << std::endl;

  Car car2;
  // Round-trip through uintptr_t -- i.e. exactly what Serializer does. Each
  // step in isolation is the "blessed" ex01 pattern. The compiler now has no
  // Car*-to-Radio* conversion to look at, so -Wreinterpret-base-class has
  // nothing to fire on.
  uintptr_t raw = Serializer_serialize(&car2);
  Radio* laundered = reinterpret_cast<Radio*>(raw);

  std::cout << "     uintptr_t raw   = reinterpret_cast<uintptr_t>(&car2);"
            << std::endl;
  std::cout << "     Radio* r        = reinterpret_cast<Radio*>(raw);"
            << std::endl;
  std::cout << "     r->radioTag     = " << std::hex << laundered->radioTag
            << std::dec << "   <- eeee again: same bug, zero warnings"
            << std::endl;
  verdict(false,
          "THIS is the real lesson of ex01. Serializer's round trip is\n"
          "           only safe because it returns to the SAME type. Split the\n"
          "           cast in two and the compiler loses the thread: it saw a\n"
          "           legal pointer->integer, then a legal integer->pointer,\n"
          "           and never saw the illegal Car->Radio in between.\n"
          "           uintptr_t is a type-system blindfold. No warning, no\n"
          "           sanitizer, no valgrind finding. Only discipline.");
}

// ---------------------------------------------------------------------------
// 4. Type punning and strict aliasing: works at -O0, breaks at -O2.
// ---------------------------------------------------------------------------
static void scenario_type_punning() {
  title("4. Type punning -- the optimiser is allowed to disagree with you");

  int i = 1078530011;  // the bit pattern of 3.14159f
  float* f = reinterpret_cast<float*>(&i);

  std::cout << "  int i = " << i << std::endl;
  std::cout << "  *reinterpret_cast<float*>(&i) = " << *f << std::endl;
  verdict(false,
          "'It printed 3.14159, so it works.' It does not work -- it is\n"
          "           a strict-aliasing violation. The standard says an int\n"
          "           object may not be read through a float lvalue, so the\n"
          "           optimiser may assume i and *f never refer to the same\n"
          "           memory and reorder the two freely.");

  std::cout << "\n  DEBUG: this is the nastiest category because the bug is\n"
               "         invisible at -O0 and appears at -O2. Compile the same\n"
               "         source both ways and diff the output; if they differ,\n"
               "         you have UB. -Wstrict-aliasing and -fsanitize=undefined\n"
               "         help, but neither is complete."
            << std::endl;
  std::cout << "\n  The correct C++98 tool for this job is std::memcpy into a\n"
               "  float, which the compiler recognises and optimises away."
            << std::endl;
}

// ---------------------------------------------------------------------------
// 5. Casting a pointer to a too-small integer.
// ---------------------------------------------------------------------------
static void scenario_truncation() {
  title("5. Why uintptr_t and not int");

  Data d;
  d.id = 7;
  d.value = 1.0;
  d.name = "x";

  uintptr_t full = reinterpret_cast<uintptr_t>(&d);
  unsigned int truncated = static_cast<unsigned int>(full);

  std::printf("  pointer as uintptr_t (%zu bytes) = 0x%lx\n", sizeof(uintptr_t),
              static_cast<unsigned long>(full));
  std::printf("  same value in unsigned int (%zu bytes) = 0x%x\n",
              sizeof(unsigned int), truncated);
  std::printf("  round trip from the truncated value = %p\n",
              reinterpret_cast<void*>(static_cast<uintptr_t>(truncated)));

  verdict(full == static_cast<uintptr_t>(truncated),
          full == static_cast<uintptr_t>(truncated)
              ? "Survived by luck: this address happened to fit in 32 bits."
              : "Top bits gone. The deserialized pointer is now garbage --\n"
                "           on a 64-bit build this is the difference between\n"
                "           uintptr_t and 'some integer that looked big enough'.");
}

int main() {
  std::cout << "\033[1m### ex01 LAB: reinterpret_cast ###\033[0m" << std::endl;
  std::cout << "reinterpret_cast relabels bits. It converts NOTHING and checks"
               " NOTHING."
            << std::endl;

  scenario_round_trip();
  scenario_wrong_type();
  scenario_multiple_inheritance();
  scenario_type_punning();
  scenario_truncation();

  std::cout << "\n\033[1m== Summary ==\033[0m\n" << std::endl;
  std::cout << "  USE reinterpret_cast for: T* <-> uintptr_t round trips back\n"
               "                            to the same type, and inspecting an\n"
               "                            object's bytes through char*.\n"
               "  NEVER use it for:         anything involving a class\n"
               "                            hierarchy. It skips the pointer\n"
               "                            adjustment that makes inheritance\n"
               "                            work. Use static_cast to go up and\n"
               "                            dynamic_cast to come down.\n"
            << std::endl;
  return 0;
}
