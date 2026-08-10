(The file `/home/syzygy/projects42/picine_cpp/cpp_module01/ex02/README.md` exists, but is empty)
# Pointers and References in C++ — A Compact Lesson

This document collects the common syntax, idioms, and "tricks" you will use when working with pointers and references in C++.

It covers:
- basic syntax
- const rules and qualifiers
- pointer arithmetic and arrays
- references (including rvalue references)
- function parameters and return values
- pointer-to-member and function pointers
- lifetime, ownership, and dangling
- smart pointers and modern best practices
- useful tips, gotchas and small exercises

## Quick summary
- Pointer: a variable that stores the address of an object. Declared with `*`.
- Reference: an alias for an existing object. Declared with `&` after the type (not to be confused with the address-of operator `&`).

Both are ways to access objects indirectly, but they differ in syntax and semantics.

## Syntax cheat-sheet

- Raw pointer declaration and use:

```cpp
int x = 10;
int* p = &x;    // p points to x
*p = 20;        // write through pointer
if (p) { /* check for non-null */ }
p = nullptr;    // reset pointer
```

- Reference declaration and use:

```cpp
int x = 10;
int& r = x;     // r is an alias for x (must initialize)
r = 30;         // changes x
// int& r2;     // error: references must be initialized
```

- Pointer to const vs const pointer vs const pointer to const:

```cpp
const int* p1 = &x;   // pointer to const int (cannot modify *p1)
int* const p2 = &x;   // const pointer to int (pointer can't change)
const int* const p3 = &x; // const pointer to const int
```

- Reference to const (can bind to temporaries):

```cpp
const std::string& s = "temp"; // lifetime of temporary is extended to match s
```

- Rvalue reference (move semantics):

```cpp
std::string&& r = std::string("hello");
// typically used as function parameters: void f(std::string&& s)
```

## Pointer arithmetic & arrays

Pointers are tied to types, and pointer arithmetic advances by sizeof(the pointed type):

```cpp
int arr[3] = {1,2,3};
int* p = arr;        // points to arr[0]
int second = *(p + 1); // 2
p++;                 // now points to arr[1]
```

Don't do arithmetic on pointers that don't point into the same object (undefined behavior).

For arrays prefer std::array or std::vector in modern C++.

## Function parameters and return values

When designing APIs prefer these rules:

- By value: small, cheap-to-copy types (int, small structs) or when you need a copy.
- By const reference (`const T&`): for large objects you don't want to copy and you don't need to modify.
- By non-const reference (`T&`): when you want the function to modify the caller's object; the caller must pass a valid object.
- By pointer (`T*`): when the parameter is optional (nullptr allowed) or you want pointer semantics.

Examples:

```cpp
void setToFive(int& r) { r = 5; }
void maybeSetToFive(int* p) { if (p) *p = 5; }

int& getGlobal();           // returns a reference to an object that must outlive the caller
int* find(int key);         // returns null if not found
```

Never return a reference or pointer to a local automatic variable. That's a dangling reference/pointer.

Bad:

```cpp
int& bad() { int x = 1; return x; } // UB - x goes out of scope
```

Good:

```cpp
int& goodStatic() { static int x = 1; return x; } // static lives for program lifetime
```

## Pointer to pointer, reference to pointer, reference to reference

```cpp
int x = 0;
int* p = &x;      // pointer
int** pp = &p;    // pointer to pointer
int*& rp = p;     // reference to pointer (alias the pointer variable itself)
// A reference to a reference (T&& &&) collapses under reference collapsing rules in templates
```

Reference to pointer example:

```cpp
void resetPtr(int*& p_ref) { p_ref = nullptr; } // can modify the caller's pointer
```

## Pointer to member / function pointers

Pointer to member data:

```cpp
struct S { int x; };
int S::* mptr = &S::x;
S s{42};
int val = s.*mptr;   // 42

// pointer-to-member used with pointer to object:
S* ps = &s;
val = ps->*mptr;
```

Function pointers:

```cpp
int (*fp)(int, int) = [](int a, int b){ return a + b; };
int z = fp(1,2);
```

In modern C++ prefer std::function and templates for flexibility, but function pointers are lightweight.

## Rvalue references and std::move

Rvalue references enable move semantics and perfect forwarding.

```cpp
std::string a = "big string";
std::string b = std::move(a); // move-construct b, a is left in valid unspecified state

template<class T>
void forwarder(T&& arg) {
	some_func(std::forward<T>(arg)); // perfect forwarding
}
```

Don't std::move from something you still need to use in a defined way.

## Lifetime, dangling pointers/references, temporaries

- Dangling pointer/reference: when the object they refer to has been destroyed. Accessing it is undefined behavior.
- Returning reference to a local is UB. Binding a reference-to-const to a temporary extends the temporary's lifetime to match the reference.

Example of lifetime extension:

```cpp
const std::string& s = std::string("temp"); // the temporary lives as long as s
```

But be careful with expressions that produce temporaries indirectly — prefer explicit variables for clarity.

## Const, volatile, and casting

- const_cast<T&/T*>(obj) can remove constness — use only when you really know the object is non-const beneath the const view.
- reinterpret_cast for low-level conversions (dangerous).
- static_cast for well-defined conversions.

Example:

```cpp
const int ci = 42;
int* p = const_cast<int*>(&ci); // undefined behavior if you modify *p because ci is really const
```

## Ownership & smart pointers (modern C++)

Raw pointers don't convey ownership. Prefer smart pointers when ownership is involved:

- std::unique_ptr<T>: exclusive ownership, RAII, cheap to move.
- std::shared_ptr<T>: reference-counted shared ownership.
- std::weak_ptr<T>: non-owning reference to an object managed by shared_ptr.

Examples:

```cpp
auto p = std::make_unique<MyType>(args...);
auto s = std::make_shared<MyType>(args...);

MyType* raw = p.get(); // non-owning raw pointer
```

Use raw pointers or references for non-owning access, and smart pointers for ownership semantics.

## Pointer/Reference idioms and tricks

- Use `const T&` for read-only parameters to avoid copies.
- Use `T&&` to implement move constructors and move assignment.
- Use `int*` or `T*` to represent optional arguments (or better: `std::optional<T>` or `std::unique_ptr<T>`).
- Use `T*&` when you want a function to swap/replace the caller's pointer.
- Use `std::addressof` when you need the address and `operator&` may be overloaded.
- Use `std::reference_wrapper<T>` (std::ref) to store references in containers.

Example: storing references in a vector

```cpp
std::string a = "a", b = "b";
std::vector<std::reference_wrapper<std::string>> refs{std::ref(a), std::ref(b)};
refs[0].get() = "A"; // modifies a
```

## Common pitfalls and gotchas

- Null references: the language doesn't have a standard null reference. Creating a reference that doesn't refer to a valid object is undefined behavior.
- Returning pointer/reference to temporary or local automatic variable — causes UB.
- Mixing ownership: don't delete memory pointed to by a raw pointer if it's owned by a smart pointer elsewhere.
- Pointer arithmetic out-of-bounds is UB.
- Use-after-free or double-delete are common pitfalls with raw pointers.

## Small examples you can try

1) Pointer vs Reference demonstration (`ptr_vs_ref.cpp`):

```cpp
#include <iostream>
#include <string>

void by_ref(std::string& s) { s += " (by ref)"; }
void by_ptr(std::string* s) { if (s) *s += " (by ptr)"; }

int main() {
	std::string a = "start";
	by_ref(a);
	std::cout << a << '\n';
	by_ptr(&a);
	std::cout << a << '\n';
	by_ptr(nullptr); // safe: function checks for null
}
```

Compile and run:

```bash
g++ -std=c++17 ptr_vs_ref.cpp -O2 -Wall -pedantic -o ptr_vs_ref && ./ptr_vs_ref
```

2) Move semantics (`move_demo.cpp`):

```cpp
#include <iostream>
#include <string>
#include <utility>

int main() {
	std::string a = "big string";
	std::string b = std::move(a);
	std::cout << "b=" << b << " a=" << a << "\n"; // a is in valid but unspecified state
}
```

## Exercises

1. Write a function `bool find_and_replace(std::vector<std::string>& v, const std::string& from, const std::string& to);` that uses references/pointers appropriately.
2. Implement a small singly-linked list using raw pointers. Then refactor it to use `std::unique_ptr` for ownership.
3. Create a function taking `T&&` and demonstrate perfect forwarding.

## Best practices (short)

- Prefer references for mandatory parameters and for cleaner syntax.
- Prefer smart pointers for ownership. Use raw pointers or references for non-owning access.
- Avoid returning references/pointers to local variables.
- Use `const T&` for large read-only parameters.
- Use `std::move` only when you really need to transfer resources.

## Further reading

- The C++ Core Guidelines: https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
- Effective Modern C++ (Scott Meyers) — for move semantics and smart pointers

---

If you want, I can add small runnable example files into this repository (e.g. `ptr_vs_ref.cpp`, `move_demo.cpp`) and run them here. Tell me which examples you'd like added and tested.

