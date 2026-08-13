// NEGATIVE TEST -- this file MUST NOT COMPILE.
//
// Engine and Radio are siblings: both are bases of Car, neither derives from
// the other. static_cast can only travel UP and DOWN a hierarchy, never
// sideways, because the relationship it needs does not exist at compile time.
// dynamic_cast CAN do this (see ex02_dynamic_cast.cpp scenario 4) because it
// inspects the complete object at run time.

class Engine {
 public:
  virtual ~Engine() {}
};

class Radio {
 public:
  virtual ~Radio() {}
};

class Car : public Engine, public Radio {};

int main() {
  Car car;
  Engine* e = &car;
  Radio* r = static_cast<Radio*>(e);  // no path from Engine to Radio
  (void)r;
  return 0;
}
