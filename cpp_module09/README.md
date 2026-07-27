# Module 09 — STL

The final module flips the question: after building containers and
algorithms, now I have to *choose* them. Each exercise is a small real
program, and the module adds a rule with teeth — a container used in one
exercise may not be reused in another. That budget shaped every design here:
`std::map` is spent on ex00, ex01 takes `std::stack` deliberately backed by
`std::list`, and that is precisely what keeps `std::vector` and `std::deque`
free for ex02, which needs two containers by itself. The choice in ex01
looks eccentric until you read it as bookkeeping; there is a ponytail
comment in `RPN.hpp` saying exactly that.

## ex00 — Bitcoin Exchange

The program (`btc`) loads a price database (`data.csv`) into a
`std::map<std::string, double>` and evaluates `date | value` lines,
printing `date => value = result` or the subject's exact error messages.
The container earns its slot because the query is not "this date" but "this
date or the closest *earlier* one", and an ordered map answers that in two
lines: `upper_bound(date)` returns the first entry strictly after the date;
unless that is `begin()`, stepping back one iterator lands on the closest
entry at or before it. Exact match and closest-lower are the same code
path. Keys stay ISO strings — `YYYY-MM-DD` sorts chronologically as text,
so a Date class would add code without adding correctness.

Two lessons cost me here. First, `strtod` is far more liberal than the
subject: it happily eats `0x5`, `1e2`, even `infinity`, and a bare `-0`
would sail past a `< 0` check. So `isValidValue` runs a strict decimal
lexer first — optional sign, digits, at most one dot — and treats any
leading minus as "not a positive number", which is how `-0` gets caught.
The test transcript feeds all three impostors on purpose. Second, the
exit-code bug: an early version printed `Error: could not open file.` and
then exited 0, and a `&& echo` chain in my shell cheerfully believed it.
Programs talk to shells, not just to humans; `main` now returns 1 whenever
the input file cannot be opened, and the suite has a check literally
annotated "(main would exit 0)" so the bug cannot come back.

The confession: this exercise once dragged around roughly 7000 vendored
lines — a CSV framework, a REPL, a table renderer — of which the subject
required none. The Makefile still carries a fossil `sqlite:` target from
that era. What survives is `BitcoinExchange.{hpp,cpp}` plus a 16-line
`main.cpp`, about 190 lines total, doing exactly what the subject asks. The
trade-off I kept: the database loader trusts the subject's CSV and skips
unparsable rows via `strtod`'s end pointer; all the strictness lives on the
input file, where the subject defines the error contract.

## ex01 — RPN

The program evaluates a Reverse Polish expression in one O(n) pass: push
digit tokens, and on `+ - * /` pop two operands, compute, push the result;
exactly one value may remain. The container lesson is that `std::stack` is
an adaptor, so mine is declared `std::stack<int, std::list<int> >` — the
second template argument swaps the default `deque` backing for `list`,
satisfying the budget above.

The classic burn is operand order: the first pop is the *right* operand
(there is a comment shouting this in `applyOperation`), so `3 4 -` is -1,
not 1 — the tests pin that, along with `8 3 /` truncating to 2. Everything
invalid — brackets, multi-digit or decimal tokens, leftover or missing
operands, division by zero — throws `std::runtime_error("Error")`, and
`main` prints that verbatim `Error` to stderr and exits 1, exactly as the
subject shows. `evaluate` drains the stack on entry so one `RPN` object can
be reused, and `isdigit` gets an `unsigned char` cast because the suite
feeds it a high-bit `\xe9`. The conscious ceiling, documented in a ponytail
comment in `RPN.cpp`: plain `int` arithmetic. The subject caps operands
below 10 and says nothing about intermediate overflow, so a long chain of
multiplied 9s wraps; checked operations wait until an evaluator asks.

## ex02 — PmergeMe

The program runs Ford-Johnson merge-insertion sort on the same input twice
— once on `std::vector<int>`, once on `std::deque<int>` — and prints
`Before:`/`After:` plus one timing line per container. In honest terms the
algorithm is: pair the elements (larger first, an odd straggler set aside),
sort the pairs recursively by their larger element so those form a sorted
main chain, then insert each smaller partner by binary search
(`std::lower_bound`) in the order given by a Jacobsthal-ish sequence —
`generateJacobsthalVector` emits 1, 3, 5, 11, 21, ... via
`next = prev + 2 * prev2` — walking each gap in descending order so every
search stays inside its comparison-optimal window. The first partner is
prepended for free; the straggler is inserted last.

`sortVector` and `sortDeque` are near-identical on purpose: the subject
explicitly warns against a single generic function, so the duplication is
sanctioned, and it keeps each container's timing untainted. The timing
itself includes data management, per the subject — the clock starts before
the container is even filled from the parsed input, `gettimeofday` at
microsecond grain. Parsing is `strtol` with full `errno`/end-pointer
checks, and `val <= 0` is rejected with a comment spelling out the reason:
the subject wants *positive* integers, and 0 is not positive. Display
truncates after 10 elements with `[...]`, a ponytail shortcut, as is the
full-chain `lower_bound` instead of a partner-bounded search — correct, a
few extra comparisons. Tests check both containers against `std::sort` on
empty, single, duplicate, sorted, reversed and all-equal inputs, then 3000
shuffled integers (even) and 3001 (odd, the straggler path at scale).

## What stuck with me

Choosing a container is a design act with consequences two exercises away —
the `list` under ex01's stack only makes sense because ex02 exists. The
`upper_bound`-then-step-back idiom replaced a page of date arithmetic with
two lines I can prove correct. I stopped trusting lenient parsers and
started writing the small strict lexer in front of them. And the deepest
one: exit codes are part of a program's interface, and deleting 7000 lines
I was proud of made the program better.

## Building and testing

Each exercise builds the same way; the binaries carry the subject's names:

```sh
cd ex00        # or ex01, ex02
make           # builds build/bin/btc, build/bin/RPN, build/bin/PmergeMe
make run       # builds and runs the binary (no arguments)
make test      # builds and runs tests/test.cpp (exits non-zero on failure)
make fclean    # removes all build artifacts
```

All three suites currently pass, including the byte-for-byte subject
session for `btc`. Real runs take their input directly:

```sh
./build/bin/btc input.txt                        # needs data.csv in the cwd
./build/bin/RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"      # -> 42
./build/bin/PmergeMe `shuf -i 1-100000 -n 3000 | tr "\n" " "`
```

Each exercise keeps its own `README.md` with the design decisions in depth.
