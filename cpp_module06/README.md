# Module 06 — C++ casts

A C-style cast like `(int)x` can silently mean any of several unrelated
conversions, and it will happily perform the wrong one. C++ splits that
blunt instrument into named casts that each do one checkable thing, and this
module hands you exactly one per exercise: `static_cast` for value
conversions in ex00, `reinterpret_cast` for the pointer round-trip in ex01,
`dynamic_cast` for runtime type recovery in ex02. A second thread runs
underneath: twice the subject demands a class nobody can instantiate, and in
C++98 — no `= delete` — the idiom is to declare the constructors, copy
assignment, and destructor `private` and never define them. Misuse fails at
the access check, or at link time from inside the class itself. This is also
the module where I learned that undefined behavior doesn't crash; it lies,
politely, until someone hostile asks the right question.

## ex00 — Conversion of scalar types

`ScalarConverter::convert(const std::string&)` takes a C++ literal — `'c'`,
`-42`, `4.2f`, `nan`, `+inff` — detects its type, and prints the value as
`char`, `int`, `float`, and `double`, with `impossible` and `Non
displayable` where honesty requires it. The subject is strict that the class
has "ONLY one static method convert", so `ScalarConverter.hpp` is one public
static method above four private, never-defined special members, and every
helper lives in an anonymous namespace in the `.cpp` — parsing is
implementation detail, not API.

The bug that reshaped this exercise was invisible in every demo I ran: my
first parser used `atoi`, and `atoi` on an out-of-range literal like
`9999999999` is undefined behavior — on my machine it returned garbage that
looked plausible enough to print. The fix was structural, not local: every
numeric literal now funnels through a single `std::strtod` call into a
`double`, so an overflowing int literal becomes a large finite double and
`printInt`'s range check against `std::numeric_limits<int>` reports
`impossible` instead of gambling. The comment in `ScalarConverter.cpp` says
exactly this, because I want the defense question. The cost of that funnel
is a trade-off I took consciously and wrote down: `wantsPointZero()` only
appends `.0` when `v == std::floor(v) && std::fabs(v) < 1e6`, so integral
values at or above a million print in cout's scientific default without the
`.0` suffix. A documented ceiling beat a hand-rolled formatter.

Then I got adversarial with my own program: a battery of 33 hostile inputs,
because the subject's examples are a warm-up, not a test. That session is
where the edges got sharp. The char boundaries live at 31/32 and 127/128 —
`printChar` treats 31 as `Non displayable`, 32 (space) as a printable
`' '`, 127 as non-displayable but *possible*, and 128 as `impossible`, since
the check is `v < 0.0 || v > 127.0` before `isprint`. Hex like `0x5` was a
trap of my own making: `strtod` would cheerfully parse it as 5, so the
validators have to reject it first — `isIntLiteral` refuses anything
non-digit after at most one leading sign, which also kills doubled signs
like `++42`. The survivors of that battery were distilled into
`tests/test.cpp`: 13 byte-for-byte comparisons of captured stdout, from the
subject's three verbatim examples through `9999999999` → `int: impossible`,
`float: 1e+10f`, down to garbage and the empty string.

## ex01 — Serialization

`Serializer` is the short exercise that exists to mark a boundary. Two
static methods: `serialize(Data*)` returns
`reinterpret_cast<uintptr_t>(ptr)`, and `deserialize(uintptr_t)` casts it
back. This is the one *guaranteed* use of `reinterpret_cast`: the standard
promises that pointer → `uintptr_t` → pointer yields the original pointer,
which the demo and test verify by comparing the round-tripped pointer for
equality and reading the members through it. `static_cast` refuses
pointer-integer conversions outright — which is the quiet lesson: the cast
that *won't* compile is telling you which tool the job needs.

Two small decisions carry the rest. `Data` in `Data.hpp` is a deliberately
plain aggregate — `int id; double value; std::string name;` — with no
constructors, because 42's rules forbid function bodies in headers and a
struct this dumb doesn't deserve its own `.cpp`. And `Serializer` repeats
the private, declared-never-defined canonical form from ex00; by the second
use it stopped feeling like an incantation and started feeling like
vocabulary.

## ex02 — Identify real type

Given a `Base*` or `Base&` to something that is really an `A`, `B`, or `C`,
recover the dynamic type — with `<typeinfo>` forbidden. `Base` has only a
public virtual destructor (body in `Base.cpp`, headers stay body-free),
which is what gives the object a vtable for `dynamic_cast` to interrogate.

The exercise's real content is the asymmetry between the two forms, and the
subject's constraints force you to feel both. `identify(Base*)` uses the
pointer form, where failure is a value: three chained casts, first non-NULL
wins. `identify(Base&)` cannot do that — the subject forbids pointers inside
it, and a reference has no null — so it must use `dynamic_cast<A&>` and let
failure arrive as an exception. Here is where the forbidden header bites:
the exception thrown is `std::bad_cast`, but *naming* it in a catch clause
requires `<typeinfo>`. So `Functions.cpp` catches `(...)` around each
attempt, and the comment above the function explains why. I dislike bare
`catch (...)` on principle — it swallows everything — but inside a try block
whose only statement is a single cast, the only possible thrower is that
cast. Narrow scope is what makes the broad handler honest; that trade-off I
can defend. The demo generates five random objects via `std::rand() % 3`
(each announcing `[generated A]` and so on, so the output is eyeball-
checkable), and `tests/test.cpp` asserts both overloads print exactly `A`,
`B`, or `C` for known objects and agree with each other on 20 random ones.

## What stuck with me

Each named cast is a sentence about intent, and choosing one forces you to
finish the sentence: this is a value conversion, this is a bit
reinterpretation I can justify from the standard, this is a runtime question
about what an object really is. The `atoi` episode taught me that UB is
worst when it works — the program that prints plausible garbage is more
dangerous than the one that crashes. And the 33-input battery taught me the
cheapest habit I own now: hostile inputs cost minutes to write and they pay
out every single time, usually at a boundary — 31/32, 127/128, one sign too
many.

## Building and testing

Everything compiles with `c++ -std=c++98 -Wall -Wextra -Werror` and is
valgrind-clean. In any exercise directory:

```sh
cd exNN
make            # build build/bin/exNN
make run        # build and run the demo
make test       # build and run tests/test.cpp — exits non-zero on failure
make fclean
```

One wrinkle: ex00's demo takes its literal on the command line
(`./build/bin/ex00 42.0f`) and prints a usage line otherwise, so `make run`
is most useful in ex01 and ex02 — for ex00, `make test` is the interesting
target anyway, since that is where the battery lives.
