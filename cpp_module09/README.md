# C++ Module 09 — STL

The final module: instead of writing containers and algorithms, you *choose*
them. Each exercise is a small real program whose difficulty lies in picking
the right standard container for the job — with the module rule that **a
container used in one exercise may not be reused in another**. The arc:

- **ex00** — ordered lookup: `std::map` and the `upper_bound` idiom.
- **ex01** — LIFO evaluation: `std::stack` as a container *adaptor* (backed
  here by `std::list`).
- **ex02** — the same algorithm on two sequence containers
  (`std::vector` vs `std::deque`) to see how container layout affects speed.

That budgeting is deliberate: `map` is spent in ex00, `stack`-over-`list` in
ex01, leaving `vector` and `deque` free for ex02, which requires two.

## ex00 — Bitcoin Exchange

Reads a price database (`data.csv`, date → rate) into a
`std::map<std::string, double>` and evaluates an input file of
`date | value` lines. The point is that `map` keeps keys sorted (ISO dates
sort chronologically as strings), so "this date or the closest earlier one"
is two lines: `upper_bound(date)` then step back one iterator. The demo
prints `date => value = result` per valid line; errors (bad dates, values
outside `[0, 1000]`, malformed lines) go to stderr with the subject's exact
messages. Tests cover exact/closest-lower/too-early lookups, leap-year
validation, every error message, and the subject's example session byte for
byte.

## ex01 — RPN

Evaluates a Reverse Polish (postfix) expression in one O(n) pass: push
digits, and on `+ - * /` pop two operands, compute, push the result; exactly
one value must remain. The container lesson is that `std::stack` is an
adaptor — here declared as `std::stack<int, std::list<int> >` to swap its
default `deque` backing for `list`. `RPN::evaluate` returns the result;
any invalid input (non-digit tokens, brackets, leftover/missing operands,
division by zero) throws, and `main` prints `Error` to stderr. Tests cover
the subject examples, operand order (`3 4 -` is −1), integer truncation,
all error classes, and object reuse.

## ex02 — PmergeMe

Implements Ford-Johnson merge-insertion sort (pairing, recursive sort of the
larger elements, then binary insertion of the partners in Jacobsthal order)
**twice** — once on `std::vector<int>`, once on `std::deque<int>` — and times
both on the same input, including the fill from parsed arguments. The demo
prints `Before:`/`After:` plus one timing line per container; try
``./build/bin/PmergeMe `shuf -i 1-100000 -n 3000 | tr "\n" " "` `` to see vector's
contiguous memory beat deque. Tests check both containers against
`std::sort` on edge cases (empty, duplicates, sorted/reversed, odd count for
the straggler path) and 3000/3001 shuffled integers.

## Commands

Each exercise builds the same way:

```sh
cd exNN
make          # builds build/bin/exNN
make run      # builds and runs the binary (no arguments)
make test     # builds and runs tests/test.cpp; exits non-zero on failure
make fclean   # removes all build artifacts
```

All three programs take their real input as command-line arguments or files,
so for meaningful runs invoke the binary directly:

```sh
./build/bin/btc input.txt                    # needs data.csv in the cwd
./build/bin/RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
./build/bin/PmergeMe 3 5 9 7 4
```

See each exercise's own `README.md` for the design decisions in depth.
