/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serializer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:55:52 by marvin            #+#    #+#             */
/*   Updated: 2025/12/25 21:55:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serializer.hpp"

// ponytail: reinterpret_cast is the subject-mandated conversion here; the
// round-trip Data* -> uintptr_t -> Data* is guaranteed by the standard.
//
// ─── READ THIS BEFORE DEFINING ANY MACRO ────────────────────────────────────
// The default build (no macro defined) takes the FIRST branch below, which is
// the real implementation. The other two branches are deliberate
// COUNTER-EXAMPLES kept for study: they show what static_cast and dynamic_cast
// do when handed a pointer/integer conversion, and they DO NOT COMPILE. That
// is the lesson, not a bug.
//
//   (nothing defined)          -> reinterpret_cast, compiles, correct
//   -DREINTERPRET -DSTATIC_CAST -> static_cast branch,  compile error
//   -DREINTERPRET               -> dynamic_cast branch, compile error
//
// Do not define these when building for evaluation.
// ────────────────────────────────────────────────────────────────────────────

#ifndef REINTERPRET
/**
take this low-levell bit/address representation and interpret it as anothe type

int value = 42;

unsigned char* bytes =
    reinterpret_cast<unsigned char*>(&value);


char c = 'A';

unsigned char x =
    reinterpret_cast<unsigned char&>(c);

unsigned char x = static_cast<unsigned char>(c);


Data data;
Data* ptr = &data;

ptr contains something like:
0x7ffd12345678

uintptr_t raw = Serializer::serialize(ptr);

Data *ptr
\|
 v
 0x7ffd12345678
 \|
 v
 uintptr_t integer

 Data* ptr2 = Serializer::deserialize(raw)
*/
uintptr_t Serializer::serialize(Data* ptr) {
    return reinterpret_cast<uintptr_t>(ptr);
}

Data* Serializer::deserialize(uintptr_t raw) {
    return reinterpret_cast<Data*>(raw);
}
#elif STATIC_CAST 

/**
static_cast is not a general-purpose "convert anything to anything" cast. C++
only allows `static_cast` for certain categories of conversions


In our case, we're trying to convert between two fundamentally different kinds of values

Data*       //pointer
uintptr_t   //integer

## 1. Pointer -> integer
uintptr_t Serializer::serialize(Data* ptr) {
    return static_cast<uintptr_t>(ptr)
};

this fails because `static_cast` does not allow arbitrary pointer-to-integer conversion..
The standard conversion we want is specifically provided by `reinterpret_cast`:

```bash
uintptr_t raw = reinterpret_cast<uintptr_t>(ptr);
```

`reinterpret_cast` is designed for exactly this low-level representation change.

## 2. Integer -> pointer

Same problem in the other direction:

Data* Serializer::deserialize(uintptr_t raw){
    return static_cast<Data*>(raw)
}

Data* ptr = reinterpret_cast<Data*>(raw);

Why doesn't `static_cast` allow it?

Think of `static_cast` as saying:

```bash
I know these two types have a meaningful C++ conversion between them,
and I want the compiler to perform that conversion.

```

For example:

*/
uintptr_t Serializer::serialize(Data* ptr) {
    return static_cast<uintptr_t>(ptr); // ❌
}

Data* Serializer::deserialize(uintptr_t raw) {
    return static_cast<Data*>(raw); // ❌
}
#else  // dynamic cast


/**
    `dynamic_cast` is for runtime-checked conversions between related
    polymorphic class types. Data is a plain struct with no virtual
    functions, and uintptr_t is not a class at all, so neither operand
    qualifies. Both lines below are rejected at compile time.
*/
uintptr_t Serializer::serialize(Data* ptr) {
    return dynamic_cast<uintptr_t>(ptr); // ❌
}

Data* Serializer::deserialize(uintptr_t raw) {
    return dynamic_cast<Data*>(raw); // ❌
}

#endif
