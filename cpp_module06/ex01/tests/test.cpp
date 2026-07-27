// C++98 self-check for Serializer: exits non-zero on any failure.
#include <cstdlib>
#include <iostream>

#include "Data.hpp"
#include "Serializer.hpp"

static int g_fails = 0;

static void check(bool ok, const char* what) {
  if (!ok) {
    std::cout << "FAIL: " << what << std::endl;
    ++g_fails;
  } else {
    std::cout << "OK:   " << what << std::endl;
  }
}

int main() {
  Data d = {42, 3.14, "roundtrip"};

  // Core subject requirement: serialize/deserialize round-trips the pointer.
  uintptr_t raw = Serializer::serialize(&d);
  Data* back = Serializer::deserialize(raw);
  check(back == &d, "deserialize(serialize(p)) == p");

  // Data is intact when read through the round-tripped pointer.
  check(back->id == 42 && back->value == 3.14 && back->name == "roundtrip",
        "members readable through deserialized pointer");

  // A second, distinct object serializes to a distinct value.
  Data d2;
  check(Serializer::serialize(&d2) != raw, "distinct objects -> distinct raws");

  // NULL round-trips too.
  check(Serializer::deserialize(Serializer::serialize(NULL)) == NULL,
        "NULL round-trips");

  return g_fails == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
