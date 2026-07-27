# ex05 — Harl 2.0

A `Harl` class that complains at four levels (`DEBUG`, `INFO`, `WARNING`,
`ERROR`) through **pointers to member functions** — the whole point of the
exercise. No `if/else if/else` forest.

```sh
make
make run     # demo: all four levels + an unknown one
make test    # exact-output tests
```

## Concept: pointers to member functions

A pointer to a free function stores an address. A pointer to a *member*
function is a different beast: it must be combined with an object instance
before it can be called, and it has its own syntax at every step:

```cpp
void (Harl::*method)(void);       // declare
method = &Harl::debug;            // take (the & is mandatory here)
(this->*method)();                // call through an object pointer
```

`complain()` pairs two parallel arrays — the level names and the member
function pointers — and walks them with a single loop:

```cpp
void (Harl::*methods[])(void) = {&Harl::debug, &Harl::info,
                                 &Harl::warning, &Harl::error};
std::string levels[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
for (int i = 0; i < 4; i++)
  if (levels[i] == level) return (this->*methods[i])();
```

One comparison per candidate, dispatch through the table: this is a jump
table built by hand, the same shape `switch` compiles to (ex06 makes that
literal). Adding a level means adding one entry to each array — no new
control flow.

Design choices worth naming: matching is case-sensitive (`"debug"` is not a
level) and an unknown level is a silent no-op — the subject prescribes
nothing for it, and doing nothing is the behavior the ex06 filter builds on.

## Tests

`tests/test.cpp` redirects `std::cout` into a `std::ostringstream` via
`rdbuf()` swapping — no forked processes, no temp files — and byte-compares
each level's full output against the subject's verbatim messages. It also
checks the three silent cases (unknown, lowercase, empty) and three full
rounds of complaints to prove dispatch is stateless. Exits non-zero on any
mismatch.
