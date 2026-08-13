/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:55:59 by marvin            #+#    #+#             */
/*   Updated: 2026/08/11 00:00:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// A guided tour of ex01. The subject asks for two reinterpret_cast calls and a
// Data struct that still works on the way out; every claim below is printed
// with the value that backs it. The assertions live in tests/test.cpp
// (make test) — this file is the readable proof.

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "Data.hpp"
#include "Serializer.hpp"

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
static void show(const std::string& label, const std::string& value) {
  std::cout << "   " << std::left << std::setw(26) << label << value
            << std::endl;
}
static void verdict(bool good, const std::string& text) {
  std::cout << "   " << (good ? GREEN : RED) << (good ? "✔ " : "✘ ") << RESET
            << text << std::endl;
}
template <typename T>
static std::string str(const T& v) {
  std::ostringstream o;
  o << v;
  return o.str();
}
static std::string hex(uintptr_t v) {
  std::ostringstream o;
  o << "0x" << std::hex << v;
  return o.str();
}

// ── 1. The two casts the subject asks for ───────────────────────────────────
static void demo_round_trip() {
  section(1, "THE ROUND TRIP: Data* -> uintptr_t -> Data*");
  note("serialize() is one reinterpret_cast, deserialize() is the other.");

  Data original;
  original.id = 42;
  original.value = 3.14159;
  original.name = "Test Data";

  uintptr_t raw = Serializer::serialize(&original);
  Data* back = Serializer::deserialize(raw);

  show("original address", str(static_cast<void*>(&original)));
  show("as uintptr_t", hex(raw));
  show("deserialized address", str(static_cast<void*>(back)));
  std::cout << std::endl;
  verdict(back == &original, "the pointers compare equal");
  note("Guaranteed by the standard for a large-enough integer type, which is");
  note("what uintptr_t is defined to be. That is why the subject picked it.");
}

// ── 2. The struct still works on the way out ────────────────────────────────
static void demo_still_usable() {
  section(2, "THE RESULTING STRUCT IS USABLE, NOT JUST EQUAL");

  Data original;
  original.id = 42;
  original.value = 3.14159;
  original.name = "Test Data";

  Data* back = Serializer::deserialize(Serializer::serialize(&original));

  show("back->id", str(back->id));
  show("back->value", str(back->value));
  show("back->name", "\"" + back->name + "\"");
  verdict(back->id == original.id && back->value == original.value &&
              back->name == original.name,
          "every field survived the trip");

  std::cout << std::endl;
  note("And it is the SAME object, not a copy — writing through the");
  note("deserialized pointer is visible in the original:");
  back->id = 99;
  back->name = "mutated";
  show("after back->id = 99", "original.id   = " + str(original.id));
  show("after back->name = ...", "original.name = \"" + original.name + "\"");
  verdict(original.id == 99 && original.name == "mutated",
          "the write reached the original object");
}

// ── 3. The edge case worth showing ──────────────────────────────────────────
static void demo_null() {
  section(3, "NULL SURVIVES THE ROUND TRIP TOO");
  note("Nothing special is coded for it; 0 in, 0 out.");

  Data* null = NULL;
  uintptr_t raw = Serializer::serialize(null);
  Data* back = Serializer::deserialize(raw);

  show("serialize(NULL)", hex(raw));
  show("deserialize(0)", back ? "non-null" : "NULL");
  verdict(back == NULL, "NULL round trips to NULL");
}

// ── 4. Why this needs reinterpret_cast at all ───────────────────────────────
static void demo_why_reinterpret() {
  section(4, "WHY reinterpret_cast AND NOT static_cast");
  note("A pointer and an integer are not related types, so static_cast has");
  note("no conversion to perform. These two lines do not compile:");
  std::cout << std::endl;
  std::cout << "     " << DIM << "uintptr_t raw = static_cast<uintptr_t>(ptr);"
            << RESET << std::endl;
  std::cout << "     " << RED
            << "error: static_cast from 'Data *' to 'uintptr_t' is not allowed"
            << RESET << std::endl
            << std::endl;
  std::cout << "     " << DIM << "Data* ptr = static_cast<Data*>(raw);" << RESET
            << std::endl;
  std::cout << "     " << RED
            << "error: static_cast from 'uintptr_t' to 'Data *' is not allowed"
            << RESET << std::endl
            << std::endl;
  note("reinterpret_cast is the one that reinterprets a bit pattern as");
  note("another type. Serializer.cpp keeps both counter-examples behind");
  note("#ifdef, so you can compile them and read the real errors.");
}

int main() {
  std::cout << std::endl
            << BOLD << " ex01 · SERIALIZATION — reinterpret_cast" << RESET
            << std::endl;
  rule(CYAN);

  demo_round_trip();
  demo_still_usable();
  demo_null();
  demo_why_reinterpret();

  std::cout << std::endl;
  rule(CYAN);
  std::cout << " " << GREEN << "✔" << RESET
            << " reinterpret_cast used exactly twice, both directions, and the"
            << std::endl
            << "   Data struct is fully usable after the round trip."
            << std::endl
            << std::endl;
  return 0;
}
