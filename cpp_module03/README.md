# Module 03 — Inheritance

One robot, three generations. `ClapTrap` is born in ex00 and every later exercise derives from
it: `ScavTrap`, then `FragTrap`, then the deliberately monstrous `DiamondTrap` that inherits
from both at once. The real subject of the module is not the robots but what happens between a
derived object's birth and its death — which constructor runs first, who owns which attribute,
and what breaks when a base class exists twice. A convention worth naming up front: each
exercise re-vendors every class it needs (ex03 carries its own copies of `ClapTrap`,
`ScavTrap`, and `FragTrap`), because the subject asks for "files from previous exercises" in
each turn-in. Every snapshot is self-contained; the price is that a fix in one exercise must be
re-applied by hand in the next, and the ex01 `ClapTrap` genuinely differs from ex00's.

This file replaces an old unchecked QA checklist I once kept here. Its useful ideas did not
die: draining energy to zero in a loop, overkill damage, poking a dead robot, self-assignment,
and copy-state fidelity all became executable assertions in the mains and in
`ex01/tests/fuzzy.cpp`, which is where test ideas belong.

## ex00 — Aaaaand... OPEN!

The exercise wants a `ClapTrap` with a name and fixed starting stats — 10 hit points, 10 energy
points, 0 attack damage in `ClapTrap.cpp` — plus `attack`, `takeDamage`, and `beRepaired`, all
narrating themselves. Mine keeps the four stats private, guards every action twice (a robot at
0 HP "cannot attack because it has no hit points left!", one at 0 EP gets the energy variant),
and charges one energy point per attack or repair. A free `operator<<` prints a compact
`ClapTrap[name | HP | EP | AD]` status line so the mains do not drown in getters.

Where I got burned: the stats are `unsigned int`, so `5 − 15` does not go negative, it wraps to
around four billion. `takeDamage` therefore clamps explicitly — `if (amount >= _hitPoints)
_hitPoints = 0;` — and the main proves it by hitting Bravo for 15 with 5 HP left. The subject
also leaves death semantics open, so I chose them consciously: a dead robot refuses everything,
including repair — no self-revival at 0 HP. Keeping the subject's unsigned types means every
method carries edge-case guards; signed ints with clamping would have been easier to reason
about, but I preferred matching the domain ("points" cannot be negative) and paying for it in
explicit checks. The main's attack loop runs 11 times so the 11th visibly fails on an empty
battery, and it ends by poking the corpse to show every guard firing.

## ex01 — Serena, my love!

`ScavTrap` derives publicly from `ClapTrap` with new stats — 100 HP, 50 EP, 20 attack damage —
its own attack message ("aggressively strikes"), and a special `guardGate()` that announces
Gate keeper mode unless the robot is dead. Enabling it meant editing the base: the ex01
`ClapTrap.hpp` flips the attributes from private to protected, and the comment in the header
still marks the spot. That is the module's first real design decision — protected is a
deliberate widening of the contract to children, not a default.

The part the subject really grades is chaining, and my constructors and destructors carry
distinct per-class messages precisely to prove it: constructing `ScavTrap scav("Guardian")`
prints the ClapTrap line first, then the ScavTrap line; destruction runs derived-first, in
exact reverse. The base is built on the way up before the derived, torn down after it on the
way down — visible in the output, not recited from a book. The awkward seam is initialization:
`ClapTrap(name)` runs first and sets 10/10/0, then the `ScavTrap` body overwrites all three
stats. A protected four-argument base constructor would have avoided the double write; I kept
the subject-shaped single-name chain and accepted two redundant assignments as the cost of a
simpler interface. One more thing I noted for later: `attack` here is overridden but not
virtual, so through a `ClapTrap&` you would still get ClapTrap's version — that is shadowing,
not polymorphism, and Module 04 exists to fix it. `tests/fuzzy.cpp` asserts the 100/50/20
start, drains all 50 EP in a loop and verifies the 51st action is refused with no underflow,
stops 500 overkill damage at exactly 0, and checks that a copied robot carries its exact
damaged state (HP 90, EP 49) rather than factory stats.

## ex02 — Repetitive work

`FragTrap` is the same shape again with different numbers — 100 HP, 100 EP, 30 attack damage —
different construction and destruction messages, and a special `highFivesGuys(void)` that asks
everyone for a high five (I gave it the same dead-robot guard as everything else). The title is
the honest part: the value of the exercise is noticing what copy-paste class families do to
you. My `FragTrap.hpp` still contains the comment "Fixed the typo here!" on the inheritance
line, a small scar from exactly the bug this kind of duplication breeds.

One choice I made deliberately: `FragTrap` does not override `attack`, so it uses ClapTrap's
wording — a visible contrast with ScavTrap that the main points out. The main doubles as an
automated fuzzer: after one attack and one repair the energy reads exactly 98, overkill damage
of 1000 pins HP at 0, a dead FragTrap's high five is refused, and copies of a damaged robot
report 80 HP, 99 EP, and the FragTrap-specific 30 attack damage.

## ex03 — Now it's weird!

`DiamondTrap` inherits from both `ScavTrap` and `FragTrap`, and per the subject its identity is
stitched from both sides: hit points from FragTrap (100), energy from ScavTrap (50), attack
damage from FragTrap (30), `attack()` from ScavTrap, its own name from the constructor
parameter, and `ClapTrap::_name` set to that parameter plus `"_clap_name"`.

Two mechanisms make it work. First, in this exercise `ScavTrap` and `FragTrap` inherit
`virtual public ClapTrap` — the headers even shout "NOTICE THE 'virtual' KEYWORD HERE!" —
so the diamond contains one shared `ClapTrap` subobject instead of two ambiguous ones. The
consequence took me a while to internalize: with a virtual base, the most-derived class
initializes it directly, so `DiamondTrap`'s constructor calls `ClapTrap(name + "_clap_name")`
itself, and the `ClapTrap` delegations inside `ScavTrap(name)` and `FragTrap(name)` are simply
ignored. Second, `using ScavTrap::attack;` in the header resolves the two inherited attacks
declaratively, no wrapper needed. The name trick is the subject's mandated bad design:
`DiamondTrap` declares its own private `std::string _name`, shadowing `ClapTrap::_name` (the
`-Wshadow` hint in the subject is a wink), and `whoAmI()` proves I know which is which by
printing both — "I am DiamondTrap Goliath! But my ClapTrap core is known as Goliath_clap_name!"

Where I got burned was the stats. `ScavTrap` and `FragTrap` constructor bodies each overwrite
HP/EP/AD, and they run in base-list order, so the final values depend on whichever body ran
last — correct by coincidence at best. I stopped relying on that and set all three explicitly
in `DiamondTrap`'s own body, commented with which parent each number honors. The main asserts
the whole contract: 100/50/30 at birth, `guardGate()`, `highFivesGuys()`, and `whoAmI()` all
callable on one object, energy dropping to 49 after an attack (proof ScavTrap's version ran,
since FragTrap started at 100), and `getName()` returning `"Goliath_clap_name"`. Virtual
inheritance costs a little indirection and this unusual constructor responsibility; the
alternative — two ClapTraps and ambiguity on every member access — is not an alternative.

## What stuck with me

Construction and destruction chaining stopped being a rule and became something I can watch:
base up first, derived down first, every time, in my own program's output. Unsigned arithmetic
is a loaded gun near subtraction. Overriding without `virtual` is shadowing dressed up as
polymorphism. And the diamond taught me that virtual inheritance does not just deduplicate a
base — it reassigns responsibility for it to the most-derived class, which is why
`DiamondTrap` constructs its own `ClapTrap` even though two parents sit in between.

## Building and testing

Each exercise is self-contained:

```
cd ex00   # or ex01, ex02, ex03
make      # builds build/bin/exNN
make run  # builds and runs it
make test # builds test binaries into build/bin/ — but see below
```

These older Makefiles share a quirk: `make test` compiles and links anything in `tests/` but
only echoes "Executing …" without running it. In ex01, run the ScavTrap fuzzer manually with
`./build/bin/fuzzy` after `make test`. ex00, ex02, and ex03 have no separate `tests/`
directory — their assertion suites are baked into `main.cpp`, so `make run` is the test.
Destructor chains fire at end of scope, so the tail of every run doubles as a leak sanity
check: one destructor line per object, in reverse construction order.
