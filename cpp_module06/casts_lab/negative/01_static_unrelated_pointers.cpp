// NEGATIVE TEST -- this file MUST NOT COMPILE.
//
// static_cast refuses to convert between unrelated pointer types. The error
// is the feature: it is the compiler telling you these types have nothing to
// do with each other. Reaching for reinterpret_cast here does not fix the
// problem, it only removes the messenger.

int main() {
  int i = 42;
  double* p = static_cast<double*>(&i);
  (void)p;
  return 0;
}
