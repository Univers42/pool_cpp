# C++ Casts — Purpose and Examples

C++ provides four explicit cast operators:

* `static_cast`
* `dynamic_cast`
* `const_cast`
* `reinterpret_cast`

There are also **implicit conversions and promotions**, which happen automatically in many situations and can be made explicit with `static_cast`.

Each cast exists for a different purpose. They should not be considered interchangeable.

---

## 1. `static_cast`

### Purpose

`static_cast` is used for **well-defined compile-time conversions** between compatible types.

It is commonly used for:

* Numeric conversions
* Numeric promotions
* Conversions between related class types
* Conversions that the compiler can verify without runtime type information

It does **not** perform arbitrary pointer/integer reinterpretations.

### Numeric conversion

```cpp
double value = 42.8;

int number = static_cast<int>(value);

std::cout << number << std::endl; // 42
```

Here, `static_cast` explicitly converts a `double` to an `int`.

The fractional part is discarded.

### Numeric promotion

A smaller integer type can be promoted to a larger integer type:

```cpp
char c = 'A';

int value = static_cast<int>(c);

std::cout << value << std::endl; // 65
```

The `char` is promoted to `int`.

The same kind of promotion can happen implicitly:

```cpp
char c = 'A';

int value = c; // implicit promotion
```

Using `static_cast` makes the programmer's intention explicit.

### Class hierarchy: derived → base

```cpp
class Animal {
public:
    virtual ~Animal() = default;
};

class Dog : public Animal {
};

Dog dog;

Animal* animal = static_cast<Animal*>(&dog);
```

This conversion is safe because every `Dog` is an `Animal`.

### What `static_cast` does NOT do

This is not allowed:

```cpp
uintptr_t raw = static_cast<uintptr_t>(ptr); // ❌
```

where:

```cpp
Data* ptr;
```

A pointer-to-integer conversion is not the purpose of `static_cast`.

For that kind of low-level conversion, `reinterpret_cast` is used.

---

# 2. `dynamic_cast`

### Purpose

`dynamic_cast` is used for **runtime-checked conversions within a polymorphic class hierarchy**.

It is useful when you have a base-class pointer/reference but need to determine whether the object is actually a particular derived type.

The base class must generally be polymorphic, meaning it has at least one virtual function.

### Example

```cpp
class Animal {
public:
    virtual ~Animal() = default;
};

class Dog : public Animal {
public:
    void bark() {
        std::cout << "Woof!" << std::endl;
    }
};

class Cat : public Animal {
};
```

Now suppose we only have an `Animal*`:

```cpp
Dog dog;
Animal* animal = &dog;
```

We can safely check whether the object is actually a `Dog`:

```cpp
Dog* dogPtr = dynamic_cast<Dog*>(animal);

if (dogPtr) {
    dogPtr->bark();
}
```

The cast succeeds because the object really is a `Dog`.

### Failed cast

```cpp
Cat* catPtr = dynamic_cast<Cat*>(animal);

if (catPtr == nullptr) {
    std::cout << "The object is not a Cat" << std::endl;
}
```

The cast returns `nullptr` because the object is a `Dog`, not a `Cat`.

This runtime checking is the key difference from `static_cast`.

### Reference version

With references, a failed `dynamic_cast` throws `std::bad_cast`:

```cpp
Animal& animal = dog;

try {
    Cat& cat = dynamic_cast<Cat&>(animal);
}
catch (const std::bad_cast& e) {
    std::cout << "Not a Cat" << std::endl;
}
```

### Important

`dynamic_cast` is **not** for converting pointers to integers:

```cpp
uintptr_t raw = dynamic_cast<uintptr_t>(ptr); // ❌
```

Nor is it for converting an integer back into a pointer:

```cpp
Data* ptr = dynamic_cast<Data*>(raw); // ❌
```

Its purpose is runtime type checking within a class hierarchy.

---

# 3. `const_cast`

### Purpose

`const_cast` is used to **add or remove `const` or `volatile` qualification** from a pointer or reference.

It does not convert the underlying object into another type.

### Removing `const`

```cpp
const int value = 42;

const int* ptr = &value;

int* mutablePtr = const_cast<int*>(ptr);
```

The type changed from:

```text
const int*
```

to:

```text
int*
```

However, there is an extremely important rule.

### Do NOT modify an originally const object

This is dangerous:

```cpp
const int value = 42;

int* ptr = const_cast<int*>(&value);

*ptr = 100; // ❌ Undefined behavior
```

The original object was actually declared `const`.

Removing the `const` qualifier does not magically make the object mutable.

### Valid use case

Suppose an API requires a non-const pointer even though the function promises not to modify the object:

```cpp
void legacyFunction(char* text) {
    // Does not modify text
}
```

You might have:

```cpp
const char* message = "Hello";

legacyFunction(const_cast<char*>(message));
```

This is only safe if `legacyFunction` truly does not modify the data.

### Key idea

`const_cast` changes **cv-qualification**:

```text
const T*  →  T*
T*        →  const T*
```

It does not change:

```text
T → U
```

and it does not reinterpret memory.

---

# 4. `reinterpret_cast`

### Purpose

`reinterpret_cast` is used for **low-level reinterpretation of a value as another type**.

It is commonly associated with:

* Pointer ↔ integer conversions
* Converting one pointer type to another
* Low-level memory manipulation
* Interfacing with low-level APIs

It should be used carefully because it does not perform the same kind of semantic checking as `static_cast` or `dynamic_cast`.

---

## Pointer → integer

This is the kind of conversion used in a serializer that stores an object's address:

```cpp
Data* ptr = ...;

uintptr_t raw = reinterpret_cast<uintptr_t>(ptr);
```

The pointer's address is represented as an integer.

Then it can be converted back:

```cpp
Data* ptr2 = reinterpret_cast<Data*>(raw);
```

This is the relevant pattern for the common pointer-address serialization exercise:

```cpp
uintptr_t Serializer::serialize(Data* ptr)
{
    return reinterpret_cast<uintptr_t>(ptr);
}

Data* Serializer::deserialize(uintptr_t raw)
{
    return reinterpret_cast<Data*>(raw);
}
```

The important idea is:

```text
Data*
  │
  │ reinterpret_cast
  ▼
uintptr_t
  │
  │ reinterpret_cast
  ▼
Data*
```

### Pointer type reinterpretation

For example:

```cpp
int value = 42;

int* intPtr = &value;

char* charPtr = reinterpret_cast<char*>(intPtr);
```

Now `charPtr` points at the same memory address, but it is interpreted as a `char*`.

This does **not** mean that the integer `42` was converted into a character.

The pointer itself was reinterpreted.

### Important warning

`reinterpret_cast` does not guarantee that every possible conversion is safe to use.

For example:

```cpp
int* ptr = ...;

double* other = reinterpret_cast<double*>(ptr);
```

The conversion may compile, but dereferencing `other` is generally not valid simply because the cast compiled.

`reinterpret_cast` gives you very little safety. The programmer is responsible for ensuring that the resulting use is valid.

---

# 5. Implicit Conversions and Promotions

Not every conversion requires an explicit cast.

C++ performs many conversions automatically.

### Integer promotion

```cpp
char c = 'A';

int value = c;
```

The `char` is automatically promoted to `int`.

Conceptually:

```text
char
  ↓
int
```

### Arithmetic promotion

```cpp
char a = 10;
char b = 20;

int result = a + b;
```

The operands are promoted before the addition.

Conceptually:

```text
char + char
     ↓
int + int
     ↓
int
```

### Floating-point conversion

```cpp
int value = 42;

double result = value;
```

The integer is automatically converted to a `double`.

```text
int
 ↓
double
```

### Explicit version

You can make the conversion explicit:

```cpp
double result = static_cast<double>(value);
```

This is often preferable when you want to make the conversion obvious to the reader.

---

# Summary

| Cast               | Main purpose                                   | Runtime check? |
| ------------------ | ---------------------------------------------- | -------------- |
| `static_cast`      | Well-defined compile-time conversions          | No             |
| `dynamic_cast`     | Runtime-checked class hierarchy conversion     | Yes            |
| `const_cast`       | Add/remove `const`/`volatile`                  | No             |
| `reinterpret_cast` | Low-level type/representation reinterpretation | No             |
| Implicit promotion | Automatic numeric/type conversions             | No             |

A useful way to remember them is:

```text
static_cast
    ↓
"These types have a normal C++ conversion."

dynamic_cast
    ↓
"Check the actual object type at runtime."

const_cast
    ↓
"Change const/volatile qualification."

reinterpret_cast
    ↓
"Interpret this representation as another type."

implicit promotion
    ↓
"The language automatically converts this value
 because the destination/context requires it."
```

## For the Serializer exercise

The important distinction is:

```cpp
// static_cast
static_cast<uintptr_t>(ptr);   // ❌ Not a pointer → integer cast
static_cast<Data*>(raw);       // ❌ Not an integer → pointer cast
```

```cpp
// dynamic_cast
dynamic_cast<uintptr_t>(ptr);  // ❌ Not a class hierarchy conversion
dynamic_cast<Data*>(raw);      // ❌ raw is an integer
```

```cpp
// reinterpret_cast
reinterpret_cast<uintptr_t>(ptr); // ✅ Pointer → integer
reinterpret_cast<Data*>(raw);     // ✅ Integer → pointer
```

So the Serializer example is a particularly good demonstration of **why the four casts are not interchangeable**.



## table 

Situation	Usually use
double → int	static_cast
char → int	static_cast / implicit
int → double	static_cast / implicit
Base* → Derived* with runtime check	dynamic_cast
Derived* → Base*	implicit / static_cast

Data* → uintptr_t	reinterpret_cast
uintptr_t → Data*	reinterpret_cast
char* → unsigned char*	reinterpret_cast can be appropriate
std::string → int	not reinterpret_cast; parsing/conversion
char → std::string	constructor/conversion, not reinterpret_cast