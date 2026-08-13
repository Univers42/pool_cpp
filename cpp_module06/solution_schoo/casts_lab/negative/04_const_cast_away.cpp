// NEGATIVE TEST -- this file MUST NOT COMPILE.
//
// Not one of module 06's three casts, but the same lesson from the fourth
// corner: static_cast and reinterpret_cast cannot remove const. Only
// const_cast can, and the moment you need it you should ask why the const was
// there. Writing through a pointer whose object was actually declared const
// is undefined behaviour even when const_cast makes it compile.

int main() {
  const int value = 42;
  int* p = static_cast<int*>(&value);
  *p = 7;
  return 0;
}
