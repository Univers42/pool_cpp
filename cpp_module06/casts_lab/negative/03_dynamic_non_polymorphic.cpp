// NEGATIVE TEST -- this file MUST NOT COMPILE.
//
// dynamic_cast needs somewhere to READ the runtime type from, and that place
// is the vtable. A class with no virtual function has no vtable, so there is
// nothing to interrogate and the compiler rejects the cast outright.
//
// The fix is to give Flat a virtual destructor -- not to downgrade to
// static_cast. If you are downcasting through Flat*, you have a polymorphic
// design already, and deleting a FlatChild through a Flat* without a virtual
// destructor is undefined behaviour on its own.

class Flat {
 public:
  int x;
};

class FlatChild : public Flat {
 public:
  int y;
};

int main() {
  FlatChild fc;
  Flat* p = &fc;
  FlatChild* c = dynamic_cast<FlatChild*>(p);  // Flat is not polymorphic
  (void)c;
  return 0;
}
