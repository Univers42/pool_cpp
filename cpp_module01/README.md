# Module 01 — Memory allocation, pointers to members, references, switch

If module 00 was about classes, this one is about *addresses*: who owns an object, how
long it lives, and how many names it has. The arc runs from zombies that prove their own
destruction (ex00–ex01), through the pointer-versus-reference question asked three ways
(ex02–ex03), to a file replacer that taught me to benchmark before believing (ex04), and
two Harls that build the same dispatch table twice — once by hand with member function
pointers, once with the `switch` the compiler always had (ex05–ex06).

## ex00 — BraiiiiiiinnnzzzZ (stack vs heap)

The exercise wants a `Zombie` that announces itself, plus two factory functions:
`newZombie()` returning a heap zombie and `randomChump()` making a stack one that
announces and dies on the spot. My destructor prints `Zombie '<name>' turned to dust.` —
that message is the whole proof. In `main.cpp`, `randomChump("StackRunner")` dusts itself
the instant the function returns, while `newZombie("HeapWalker")` lives across scopes
until I `delete` it by hand. The decision rule I took away: stack when the lifetime fits
the scope (cheap, automatic, impossible to leak), heap only when the object must outlive
the function that made it — and then the `delete` is *my* problem. The tricky bit was
believing how little code this needs; the trade-off was adding edge cases the subject
never asked for (empty name, `"Z0mb!e_#42"`, a 48-character name) because the ctor/dtor
log lines made odd inputs free to check.

## ex01 — Moar brainz (zombieHorde)

Here I must allocate N zombies in a single allocation. `ZombieHorde.cpp` is small:
guard `if (N <= 0) return NULL;`, then `new Zombie[N]` once, then a loop calling
`setName(name)` on each. What the exercise is really teaching hides in the constraints:
`new Zombie[N]` can only call the default constructor, so I had to add one (it names the
zombie `"Unnamed"`) plus a setter — two-phase initialization, which I dislike but C++98
array-new forces. The thing that genuinely burned me in testing was the deallocation pair:
an array from `new[]` must die by `delete[]`; plain `delete` on it is undefined behavior
that often *appears* to work — `delete[] horde` printing five destructor messages is the
receipt. Trade-off: "An unnamed Zombie crawled out of the grave." fires N times before
naming; I kept it because seeing construction order beats silence in a learning repo.

## ex02 — HI THIS IS BRAIN

One `main.cpp`, no class: a `std::string greet = "Hi this is brain"`, a `stringPTR`
pointing to it, a `stringREF` bound to it, then print the three addresses and the three
values. The output makes the point better than any book did: `&greet`, `stringPTR` and
`&stringREF` are the *same address*. A reference is another name, not another object —
taking "its" address just takes the address of the thing it names. Nothing here was hard
to write; what was tricky was internalizing the asymmetry (a pointer is an object holding
an address, reseatable and nullable; a reference is neither). My trade-off: I wrote the
when-to-use-which reasoning as a comment block in the file itself, so it cannot drift.

## ex03 — Unnecessary violence (Weapon, HumanA, HumanB)

The setup: a `Weapon` with a type string, `HumanA` who always has one, `HumanB` who might
not. The whole exercise is one design decision, and my headers say it in comments:
`HumanA` holds `Weapon& weapon` ("Reference because HumanA is ALWAYS armed") and `HumanB`
holds `Weapon* weapon` ("Pointer because HumanB can be unarmed (NULL)"). A reference
member cannot exist unbound, so `HumanA`'s constructor *must* initialize it in the
initializer list — `: name(name), weapon(weapon)` — there is no assigning it later.
`HumanB` starts at NULL and `attack()` checks before dereferencing; Pacifist Paul in
`main.cpp` exists precisely because an unchecked `weapon->getType()` there is a segfault.
Both humans see `club.setType("some other type of club")` instantly, since both alias the
caller's `Weapon`. The criterion I keep: *can it be absent or change target? pointer.
Guaranteed present for the owner's whole life? reference.* One honest note: ex03's
README.md is not a write-up but a 302-line field guide on pointers and references (const
rules, dangling, smart pointers) I wrote for myself while sorting this out. Returning
`const std::string&` from `Weapon::getType()` instead of a copy was the small trade-off
that made the aliasing visible at zero cost.

## ex04 — Sed is for losers

Replace every `s1` with `s2` from `<filename>` into `<filename>.replace`, with C file
functions banned and `std::string::replace` banned. `Replacer::process()` slurps the file
through a binary-mode `std::ifstream` and `rdbuf()` (so embedded NUL bytes survive — the
tests feed `{'a','\0','b',...}` on purpose), then runs a `find`-and-advance loop writing
the gap, writing `s2`, and resuming *after* the match — non-overlapping, sed-style, so
`abab` with `s1=ab, s2=cab` yields `cabcab` instead of looping forever. Empty `s1` is
rejected outright, the output file is only opened after a successful read (no stray
`.replace` on error), and a `flush()` + `ofs.good()` check catches short writes.

This exercise handed me the module's best lesson. I originally shipped a hand-rolled
Knuth–Morris–Pratt — the textbook O(N+M) answer — then benchmarked it against the naive
`std::string::find` loop on a 16 MB file. The measured table from the README:

| workload                 | Replacer (`find`) | hand-rolled KMP |
|--------------------------|-------------------|-----------------|
| prose, 372k matches      | ~62 ms            | ~735 ms         |
| prose, pattern absent    | ~84 ms            | ~707 ms         |
| pathological (`aaa…ab`)  | ~40 ms            | ~80 ms          |

The "clever" code lost everywhere — even on the near-miss case built to favor it —
because libstdc++'s `find` rides SIMD `memchr`/`memcmp` while my KMP compares one byte
per branch. Big-O counts comparisons, not nanoseconds. The trade-off I settled on: ship
the ten-line `find` loop, and demote the KMP to `tests/kmp_ref.hpp` as an independent
oracle — every one of the ~320 differential cases (plus a 300-iteration deterministic
fuzzer and 30 kB pathological patterns) checks that two *different* algorithms agree,
which is a far stronger test than one algorithm agreeing with itself.

## ex05 — Harl 2.0

Harl complains at four levels, and the subject forbids an `if/else` forest — the point is
pointers to member functions. `complain()` pairs two parallel arrays,
`void (Harl::*methods[])(void) = {&Harl::debug, &Harl::info, &Harl::warning, &Harl::error}`
and the matching level strings, then walks them and dispatches with `(this->*methods[i])()`.
That syntax was the hurdle: the `&Harl::debug` is mandatory, the pointer is useless
without an instance, and the parentheses around `this->*methods[i]` are not optional —
call precedence bites otherwise. Two deliberate calls the subject leaves open: matching is
case-sensitive (`"debug"` is not a level) and an unknown level is a silent no-op — the
contract ex06 builds on. `tests/test.cpp` swaps `std::cout`'s `rdbuf()` into an
`ostringstream` and byte-compares every message, the silent cases, and three rounds to
prove dispatch is stateless.

## ex06 — Harl filter

Same Harl, new entry point: `./build/bin/ex06 WARNING` must print that level *and
everything above it*. `main.cpp` maps `argv[1]` to an index via a loop over the levels
array, then a `switch (level_index)` where fallthrough is the feature: `case 0` prints the
`[ DEBUG ]` block and deliberately falls through 1, 2, 3 (each marked `/* FALLTHROUGH */`),
with the single `break` after `case 3` and a `default` — also used when `argc != 2` —
printing `[ Probably complaining about insignificant problems ]`. The subject's own hint
clicked here: this `switch` *is* the jump table ex05 built by hand. Here `complain()` is a
plain `if/else` chain printing only message bodies while the switch owns the `[ LEVEL ]`
headers — a split so the cascade lives in exactly one place.

## What stuck with me

Lifetime is the whole question: stack unless the object must outlive its scope, and every
`new`/`new[]` is a promise to a matching `delete`/`delete[]`. A reference is another name
for an object — it can't be null, can't reseat, and as a member must be bound in the
initializer list; a pointer is an object of its own, which is exactly what makes "maybe
absent" representable. Measure before optimizing: the KMP-versus-`find` table is the
proof I keep. And dispatch is one idea wearing three coats — module 00's function
pointers, ex05's member-function-pointer table, ex06's `switch`.

## Building and testing

Every exercise builds with `c++ -std=c++98 -Wall -Wextra -Werror`: `cd exNN && make`,
binary at `build/bin/exNN`, `make run` to build and execute (`ex04` needs real arguments,
`./build/bin/ex04 <file> <s1> <s2>`; `ex06` wants a level). One asymmetry worth knowing:
the regenerated ex04/ex05 Makefiles' `make test` actually *runs* the test binaries and
fails on any mismatch (`./$test_bin || exit 1`), re-measuring ex04's benchmark every run;
the others (ex00–ex03, ex06 — like all of modules 00–03) only build tests into
`build/bin/` without executing them, so run those by hand (ex01's and ex03's test.cpp are
empty placeholders anyway). `make fclean` clears everything, ex04 scratch files included.
