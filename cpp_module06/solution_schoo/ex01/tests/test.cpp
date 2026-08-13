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

// Deterministic + fuzz tests for ex01 Serializer.
//
//   make test              build and run
//   ./build/bin/test       run directly
//   ./build/bin/test 1234  run with a different fuzz seed
//   ./tests/mutants.sh     prove these tests can actually fail
//
// The contract under test is one sentence: for any Data*, deserialize of
// serialize gives back a pointer to the same object. Everything below is a
// way of trying to break that.

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "Data.hpp"
#include "Serializer.hpp"

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

static std::string str(uintptr_t v) {
  std::ostringstream o;
  o << "0x" << std::hex << v;
  return o.str();
}

static std::string str(const void* p) {
  std::ostringstream o;
  o << p;
  return o.str();
}

}  // namespace t

// ─── helpers ────────────────────────────────────────────────────────────────

static Data makeData(int id) {
  Data d;
  d.id = id;
  d.value = id * 1.5;
  d.name = "item";
  return d;
}

static bool sameFields(const Data& a, const Data& b) {
  return a.id == b.id && a.value == b.value && a.name == b.name;
}

// A round trip, written once so every test below reads the same way.
static Data* roundTrip(Data* p) {
  return Serializer::deserialize(Serializer::serialize(p));
}

// ─── 1. the contract ────────────────────────────────────────────────────────

static void test_round_trip_identity() {
  t::section("round trip returns the same pointer");

  Data stack = makeData(1);
  t::ok(roundTrip(&stack) == &stack, "stack object");

  Data* heap = new Data(makeData(2));
  t::ok(roundTrip(heap) == heap, "heap object");
  delete heap;

  Data array[8];
  for (int i = 0; i < 8; ++i) {
    array[i] = makeData(i);
    t::ok(roundTrip(&array[i]) == &array[i], "array element");
  }

  // A heap array rather than a container: same intent (a run of objects at
  // addresses we did not choose), but no STL container.
  Data* heapArray = new Data[16];
  for (int i = 0; i < 16; ++i)
    t::ok(roundTrip(&heapArray[i]) == &heapArray[i], "heap array element");
  delete[] heapArray;
}

static void test_null() {
  t::section("NULL survives the round trip");

  Data* null = NULL;
  t::ok(Serializer::serialize(null) == 0, "serialize(NULL) is 0");
  t::ok(Serializer::deserialize(0) == NULL, "deserialize(0) is NULL");
  t::ok(roundTrip(null) == NULL, "NULL round trips to NULL");
}

// ─── 2. the object is genuinely the same object, not a copy ─────────────────

static void test_same_object() {
  t::section("round trip yields the object, not a copy");

  Data d = makeData(42);
  Data* back = roundTrip(&d);

  t::ok(sameFields(*back, d), "fields match after round trip");

  back->id = 99;
  back->name = "mutated";
  t::ok(d.id == 99, "write through the round-tripped pointer reaches d.id");
  t::ok(d.name == "mutated", "write reaches d.name");

  d.value = 7.25;
  t::ok(back->value == 7.25, "write to d is visible through the pointer");
}

// ─── 3. algebraic properties ────────────────────────────────────────────────

static void test_deterministic() {
  t::section("serialize is deterministic");

  Data d = makeData(3);
  uintptr_t a = Serializer::serialize(&d);
  for (int i = 0; i < 100; ++i)
    t::ok(Serializer::serialize(&d) == a, "same pointer, same value");
}

static void test_injective() {
  t::section("distinct pointers give distinct values");

  const int kN = 64;
  Data pool[kN];
  for (int i = 0; i < kN; ++i)
    for (int j = i + 1; j < kN; ++j)
      t::ok(Serializer::serialize(&pool[i]) != Serializer::serialize(&pool[j]),
            "pool[" + t::str(&pool[i]) + "] vs pool[" + t::str(&pool[j]) + "]");
}

static void test_idempotent() {
  t::section("round tripping repeatedly is stable");

  Data d = makeData(5);
  Data* p = &d;
  for (int i = 0; i < 1000; ++i) {
    p = roundTrip(p);
    t::ok(p == &d, "still the same pointer after repeat #" + t::str(p));
  }
}

static void test_uintptr_is_wide_enough() {
  t::section("uintptr_t can actually hold a pointer");

  t::ok(sizeof(uintptr_t) >= sizeof(void*),
        "sizeof(uintptr_t) >= sizeof(void*)");

  // The reason the subject specifies uintptr_t rather than "some integer":
  // a narrower type would silently drop the high half of a 64-bit address.
  Data d = makeData(6);
  uintptr_t full = Serializer::serialize(&d);
  t::ok(Serializer::deserialize(full) == &d, "full-width value round trips");
}

// ─── 4. fuzz ────────────────────────────────────────────────────────────────

// Addresses vary a lot more when allocation sizes and lifetimes are irregular,
// so this deliberately interleaves allocation and release rather than walking
// one tidy block.
static void fuzz_heap_addresses(int iterations) {
  t::section("fuzz: round trip across scattered heap addresses");

  // Fixed-capacity live set instead of a growable container. Removal swaps
  // the last element into the hole, which is O(1) and keeps the order
  // irregular -- exactly what we want for address scatter.
  static const int kMaxLive = 64;
  Data* live[kMaxLive];
  int liveCount = 0;

  for (int i = 0; i < iterations; ++i) {
    if (liveCount > 40 && std::rand() % 2) {
      int k = std::rand() % liveCount;
      delete live[k];
      live[k] = live[--liveCount];
      continue;
    }
    if (liveCount >= kMaxLive) {
      delete live[--liveCount];
      continue;
    }
    Data* p = new Data;
    p->id = std::rand();
    p->value = std::rand() / 3.0;
    p->name = std::string(static_cast<size_t>(std::rand() % 40), 'x');
    live[liveCount++] = p;

    Data* back = roundTrip(p);
    t::ok(back == p, "pointer identity at " + t::str(p));
    t::ok(sameFields(*back, *p), "fields intact at " + t::str(p));
  }
  for (int i = 0; i < liveCount; ++i) delete live[i];
}

// Every byte of a pointer matters. Walking a large block one offset at a time
// exercises many different low-byte patterns, including ones with zero bytes.
static void fuzz_offsets_within_block(int iterations) {
  t::section("fuzz: every offset inside a large block");

  const size_t kCount = 512;
  Data* block = new Data[kCount];
  for (int i = 0; i < iterations; ++i) {
    size_t k = static_cast<size_t>(std::rand()) % kCount;
    Data* p = &block[k];
    t::ok(roundTrip(p) == p, "offset " + t::str(p));
  }
  delete[] block;
}

// Raw integers that never came from a real pointer must still round trip as
// values -- deserialize is not allowed to inspect or "fix" what it is given.
static void fuzz_arbitrary_values(int iterations) {
  t::section("fuzz: arbitrary integers round trip as values");

  for (int i = 0; i < iterations; ++i) {
    uintptr_t raw = 0;
    for (size_t b = 0; b < sizeof(uintptr_t); ++b)
      raw = (raw << 8) | static_cast<uintptr_t>(std::rand() & 0xff);

    Data* p = Serializer::deserialize(raw);
    t::ok(Serializer::serialize(p) == raw, "value " + t::str(raw));
  }
}

// ─── main ───────────────────────────────────────────────────────────────────

int main(int argc, char** argv) {
  unsigned seed = 20260811u;
  int iterations = 4000;
  if (argc > 1) seed = static_cast<unsigned>(std::strtoul(argv[1], NULL, 10));
  std::srand(seed);

  test_round_trip_identity();
  test_null();
  test_same_object();
  test_deterministic();
  test_injective();
  test_idempotent();
  test_uintptr_is_wide_enough();
  fuzz_heap_addresses(iterations);
  fuzz_offsets_within_block(iterations);
  fuzz_arbitrary_values(iterations);

  std::ostringstream title;
  title << "ex01 Serializer  (seed " << seed << ")";
  return t::report(title.str().c_str());
}
