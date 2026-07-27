#ifndef CPP_MODULE06_EX01_SERIALIZER_HPP_
#define CPP_MODULE06_EX01_SERIALIZER_HPP_

#include <stdint.h>

#include "Data.hpp"

// Not initializable by the user: everything OCF is private and never defined
// (C++98 idiom replacing C++11 `= delete`).
class Serializer {
 private:
  Serializer();
  Serializer(const Serializer&);
  Serializer& operator=(const Serializer&);
  ~Serializer();

 public:
  static uintptr_t serialize(Data* ptr);
  static Data* deserialize(uintptr_t raw);
};

#endif  // CPP_MODULE06_EX01_SERIALIZER_HPP_
