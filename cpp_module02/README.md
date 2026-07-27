# Module 02 — Operator overloading and the Orthodox Canonical Form

This module is where classes stopped being structs with functions stapled on and became value
types. Its spine is the Orthodox Canonical Form — default constructor, copy constructor, copy
assignment operator, destructor — introduced on `Fixed` in ex00 and never questioned again.
The same class then grows through all four exercises: a raw fixed-point store, then conversions
and printing, then a full operator set, then a real geometric application. By the end, `Fixed`
is a number type I built myself, and I know what every one of its 32 bits is doing.

## ex00 — My First Class in Orthodox Canonical Form

The exercise wants a `Fixed` class in canonical form whose four special functions all announce
themselves. Mine stores an `int _fixedPointValue` next to `static const int _fractionalBits = 8`,
which fixes the resolution of everything that follows: the smallest representable step is
1/256 = 0.00390625. At this stage the class can only `getRawBits` and `setRawBits`; the point is
not the number, it is watching `main.cpp` construct `a`, copy-construct `b(a)`, and assign
`c = b` while the console narrates who gets created, copied, and destroyed, and in which order.

The one deliberate shortcut is that my copy constructor delegates to the assignment operator
with `*this = other`. One code path for copying instead of two — but the trace of a copy
construction now also prints "Copy assignment operator called", and the member gets written
twice. I took that trade knowingly here; in a class with real resources I would not. There is
also an empty `new.cpp` that the Makefile dutifully compiles — scaffolding I never removed.

## ex01 — Towards a more useful fixed-point number class

Here `Fixed` learns to talk to the world: an int constructor (`n << _fractionalBits`), a float
constructor (`roundf(f * (1 << _fractionalBits))`), `toFloat`, `toInt`, and a free
`operator<<` that prints the float view. This is also where quantization introduced itself:
construct `Fixed const c(42.42f)` and it prints 42.4219, because 42.42 is not a multiple of
1/256 and the nearest representable value is 42.421875. The number I put in is not the number
I get out, and that is by design, not by accident.

What burned me was negative values. Left-shifting to encode is fine, but getting back out is
not symmetric, so `toInt` goes through `toFloat` and a cast, which truncates toward zero the
way C intuition expects. I also marked both converting constructors `explicit` — stricter than
the subject needs, and it costs the sugar of writing `Fixed f = 3;`, but I decided I never want
an int silently becoming a `Fixed` behind my back. `tests/test_fixed.cpp` pins the facts with a
small dispatch table: the 42.421875 conversion, the 1/256 epsilon via `setRawBits(1)`, negative
round-trips, and an 8,000,000 store — comfortable, since 8 fractional bits in a 32-bit int
leave integer headroom of (2^31 − 1)/256, about 8.38 million.

## ex02 — Now we're talking

The subject asks for the full operator set: six comparisons, four arithmetic operators, the four
increment/decrement forms, and static `min`/`max` in const and non-const flavors. The header is
exactly that. The implementation is where I went off-script: `Fixed.cpp` is a software ALU.
Nothing inside the arithmetic uses the CPU's `+`, `-`, `*`, or `/` on the stored values.

Addition is a full adder in a loop — XOR for the sum, AND for the carries, shift the carries
left, repeat until they drain (`bit_add`). Subtraction is two's complement, literally
`bit_add(a, bit_add(~b, 1))`. Multiplication strips the signs into 64-bit space, does classic
shift-and-add over the multiplier's bits, then handles the fixed-point scale: the raw product
carries 16 fractional bits, so I sample bit 7 as a rounding bit, shift right by 8, add the
rounding bit back, and reapply the sign. Division pre-scales the dividend by `<< 8` first —
otherwise all fractional information dies — then runs binary long division, trial-subtracting
the divisor shifted from bit 39 down to bit 0 and setting quotient bits as it goes. Even the
loop counter decrements through `bit_sub`. Division by zero prints "Error: Division by zero"
and returns a zero `Fixed`, which is the least bad option in C++98 with no exceptions in play.
The comparisons are bitwise too: `==` is `!(a ^ b)`, and `<` reasons about sign bits directly.

Why do this when `return Fixed::fromRaw(a + b)` would have been one line? Because fixed-point
exists precisely for machines without an FPU, and I wanted to feel what the hardware does
instead of asking the compiler to feel it for me. The trade-off is real and I accepted it with
open eyes: a loop per carry chain instead of one instruction, far more surface for sign bugs
(multiplication's sign handling bit me twice), and purity that has seams — loop conditions
still use native comparisons; I drew the line at the arithmetic itself. What I bought is that
two's complement, carry propagation, and long division are now things I can derive, not recite.
The per-exercise `README.md` in `ex02/` documents the algorithms at length.

The other lesson lives in `ex02/INCONSISTENT RESULT.md`. My fuzzing harness (the checked-in
`main.cpp` runs 100 randomized rounds of each operator against float, inputs in ±50) originally
used a 0.2 absolute tolerance, and division kept "failing": `Fixed(24.1464 / -0.282173)` gave
−85.8438 where float said −85.573. I hunted a bug that was not there. Trace it by hand: the
inputs quantize to 24.14453125 and −0.28125 the moment they are stored, perfect division of
those gives −85.8472…, and truncating that into 8 fractional bits gives exactly −85.8438. The
algorithm was flawless; my oracle was comparing against numbers my format never saw. The error
of 0.2708 is 0.3% of the result — resolution, not defect. Switching the campaign to a relative
tolerance of 1.5% made 1000/1000 randomized tests pass; the current main keeps a 0.91f absolute
band sized for the ±50 input range. Quantization is not a bug, and testing low-precision math
against high-precision math requires modeling the format, not wishing it away.

## ex03 — BSP

The finale: a `Point` class with two `Fixed const` members and a `bsp()` function that decides
whether a point is strictly inside a triangle, with vertices and edges counting as outside. My
`bsp.cpp` uses orientation via the sign of the cross product's z-component: `crossProduct`
computes `(x1 − x3)(y2 − y3) − (x2 − x3)(y1 − y3)`, I evaluate it for the point against each
edge (d1, d2, d3), any exact zero means the point sits on an edge or vertex and returns false,
and otherwise the point is inside if and only if all three signs agree —
`!(has_neg && has_pos)`. Every subtraction and multiplication in there runs on the ex02 ALU.

The trap in this exercise is the Orthodox Canonical Form itself: `_x` and `_y` are `const`, so
a copy assignment operator cannot legally assign them. Mine casts the const away —
`(Fixed&)this->_x = other.getX();`, labeled TRAP BYPASS in `Point.cpp`. I want to be honest
about this trade: it ticks the OCF box the subject demands, but writing through a const member
is formally undefined behavior, and the defensible design is a `Point` that simply is not
assignable. I kept the cast for the exercise and would refuse it at work.

Testing is where module lessons compounded. `tests/fuzzy.cpp` first pins five fixed cases —
strictly inside (2,2), strictly outside (10,10), on the hypotenuse (5,5), on vertex (0,10), and
a triangle with negative coordinates — then throws 10,000 random points at a random triangle
against a float implementation of the same test. Two things make that fair, both learned from
the ex02 episode: the float oracle reads the quantized coordinates (`p.getX().toFloat()`), so
both sides see the same data, and points whose cross products fall below 0.5 in magnitude are
skipped, because at the boundary an 8-bit format and a 32-bit format may legitimately disagree.
The `main.cpp` entry point is declared `__attribute__((weak))` so a test translation unit can
supply its own — belt and braces, since the Makefile already excludes `main.o` from test links.

## What stuck with me

The Orthodox Canonical Form became reflex — I now notice its absence in any class I read. The
deeper mark is numerical: a number system has a resolution, and error within that resolution is
not a defect; absolute tolerances lie and relative tolerances tell the truth. Building the
operators out of gates turned operator overloading from syntax into semantics — an overloaded
`+` is a promise about meaning, and I now know precisely what mine promises, including where it
rounds. And a fuzzer is only as honest as its oracle: feed the reference the same quantized
inputs, or you are testing your expectations instead of your code.

## Building and testing

Each exercise is self-contained:

```
cd ex00   # or ex01, ex02, ex03
make      # builds build/bin/exNN
make run  # builds and runs it
make test # builds test binaries into build/bin/ — but see below
```

A quirk of these older Makefiles: `make test` compiles and links everything under `tests/` but
only echoes "Executing …" without actually executing, so run the binaries manually —
`./build/bin/test_fixed` in ex01 and `./build/bin/fuzzy` in ex03 (the 10,000-point BSP fuzzer).
ex02 has no `tests/` directory; its stress suite is `main.cpp` itself, so `make run` is the
test there. `make fclean && make re` behave as usual.
