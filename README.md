# picine_cpp

My complete journey through the 42 School C++ modules — ten modules, from
`cout << "hello"` to a Ford-Johnson merge-insertion sort — with the code,
the tests that prove it works, and honest notes about everything that went
wrong along the way.

This README is the map. Each `cpp_module0N/README.md` is the territory: a
per-module deep dive into what the exercises teach, how my solutions work,
what felt hard, and which trade-offs I took with open eyes. If you're
walking the piscine yourself, start there — the module notes were written
to be read, not just indexed.

## The route

| Module | Theme | The one thing it hammered in |
|--------|-------|------------------------------|
| 00 | Namespaces, classes, statics | A class is state plus discipline, not a struct with feelings |
| 01 | Memory, references, pointers to members | A reference is a promise; a pointer is a question |
| 02 | Orthodox Canonical Form, operator overloading | If you write one of copy-ctor/assign/dtor, you owe all of them |
| 03 | Inheritance | Constructors chain up, destructors chain down — always |
| 04 | Polymorphism, abstract classes, interfaces | `virtual` is a contract; forgetting it on a destructor is a leak |
| 05 | Exceptions | Validate before you mutate, and grade 1 is the *highest* |
| 06 | Casts | Each cast answers a different question; `dynamic_cast` answers at runtime |
| 07 | Templates | The compiler writes the code; you write the constraints |
| 08 | STL containers | Stop reinventing; start composing |
| 09 | STL applied | The right container *is* the algorithm |

## Lessons that outlived the exercises

**Measure before you optimize — then believe the measurement.** Module 01's
file replacer originally shipped a hand-written Knuth-Morris-Pratt search,
the textbook O(N+M) answer. Benchmarked against a plain
`std::string::find` loop it lost on *every* workload — 62 ms versus 735 ms
on 16 MB of prose, and it lost even on the pathological input constructed
to favor KMP, because libstdc++'s `find` rides SIMD `memchr`/`memcmp`
while my loop compared one byte per branch. Big-O counts comparisons;
nanoseconds pay for them. The KMP now lives in the test suite as an
independent oracle, which is the job it was actually good at.

**Half-constructed objects never run their destructors.** The leak that
taught me exception safety was 16 bytes: a copy constructor that threw
halfway through its element copies simply abandoned the buffer it had just
allocated. The pattern that fixes it — and now runs through `Array<T>`,
`Dog`, `Cat`, `Character` and `MateriaSource` — is *clone first, delete
second, assign last*. If the clone throws, you still own your old state.

**A test that cannot fail is decoration.** Mutation testing found suite
holes everywhere confidence lived: an `operator=` that silently ignored
empty arrays passed the whole old suite; an over-iteration bug was caught
only because stack garbage happened to be nonzero. Every suite in modules
04–09 has since been fed deliberately broken code to prove it bites.

**Leaks hide on the error paths.** Valgrind on the happy path is a warm-up.
The real finds came from hostile inputs, forced `bad_alloc` under
`ulimit`, throwing copy constructors, double-equipping the same pointer,
and checking that a program which *prints* an error also *exits* with one.

**Parsing leniency is a bug surface.** `strtod` happily accepts `0x5`,
`1e2` and `infinity`. If the subject says "decimal number", the validator
has to say no before the parser gets a vote.

**Constraints breed idioms.** C++98 has no `= delete`, no `noexcept`, no
lambdas — so you learn the private-declared-never-defined idiom, `throw()`
specifications, and function-pointer dispatch tables. None of it feels
antique once you understand what problem each idiom solves.

**Deleting is engineering too.** The BitcoinExchange exercise once hauled
around ~7000 lines of vendored CSV/REPL/table machinery. What the subject
asks for takes ~130. Every deliberate shortcut that remains is marked with
a `// ponytail:` comment naming its ceiling and the trigger to revisit it.

## Building and testing anything

```sh
cd cpp_moduleNN/exNN
make          # → build/bin/<name>   (module09: btc, RPN, PmergeMe)
make run      # build + execute
make test     # modules 04–09 (and 01/ex04–05): builds AND runs the suite
make fclean   # leave no trace
```

Everything compiles with `c++ -std=c++98 -Wall -Wextra -Werror`, zero
warnings. Makefiles are generated from `autotools/Makefile.in` by
`autotools/gen_make.py`. The machine-oriented map of this repository —
conventions, generated-Makefile details, known quirks — lives in
`CLAUDE.md`.

## State of the repo, honestly

Modules 04–09 plus module01's ex04/ex05 are fully repaired, tested,
valgrind-clean on success *and* error paths, and checked line-by-line
against their subject PDFs. Modules 00–03 are the originals from the
piscine days, warts documented in `CLAUDE.md` — they build, but they
haven't had the same torture treatment. The difference is visible in the
git history, which tells the whole story one commit at a time.
