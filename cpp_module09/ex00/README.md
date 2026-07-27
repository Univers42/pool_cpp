# ex00 — Bitcoin Exchange

## The concept: `std::map` as an ordered associative container

This exercise is about picking the right STL container for a lookup problem.
The price database maps dates to exchange rates, and the required query is not
just "find this date" but **"find this date, or the closest earlier one"**.

`std::map` fits exactly because it is *ordered*: keys are kept sorted
(here `YYYY-MM-DD` strings, whose lexicographic order equals chronological
order), so it offers logarithmic bound queries that an unordered container
cannot:

```cpp
std::map<std::string, double>::const_iterator it = _rates.upper_bound(date);
if (it == _rates.begin()) return false;  // date precedes the whole DB
--it;                                    // closest date <= requested date
```

`upper_bound(date)` returns the first entry *strictly after* `date`; stepping
back one lands on the closest entry `<= date` — which is the exact-match case
and the closest-lower case handled by the same two lines. That single idiom is
the heart of the exercise.

## Design decisions

- **One class, `BitcoinExchange`**, in Orthodox Canonical Form, owning the
  `std::map<std::string, double>` price DB. No other containers used (module
  rule: each container may be used in only one exercise — `map` is spent here).
- **Dates stay strings.** Since ISO date order == string order, converting to a
  Date type would add code without adding correctness. Validation (format,
  month/day ranges, leap years) is a small static helper instead.
- **Parsing with `std::getline` + `find`/`substr` + `strtod`**, all C++98
  stdlib. Values pass a strict lexical check first (optional sign, digits,
  at most one dot), so `strtod` never sees hex (`0x5`), exponents (`1e2`),
  `nan` or `infinity`; a leading `-` (even `-0`) is "not a positive
  number.". The DB loader uses `strtod`'s end pointer to skip unparsable
  rows.
- **Errors go to `std::cerr`, results to `std::cout`**, with the subject's
  verbatim messages (`Error: bad input => ...`, `Error: not a positive
  number.`, `Error: too large a number.`). A value is valid iff it is a number
  in `[0, 1000]`.
- The program expects `data.csv` (the subject-provided price DB) in the
  current working directory.

## Build / run / test

```sh
make                        # builds build/bin/btc
./build/bin/btc input.txt  # prints "date => value = result" per valid line
./build/bin/btc            # "Error: could not open file."

make test                   # builds & runs tests/test.cpp (exits non-zero on failure)
make fclean                 # removes all build artifacts
```

`tests/test.cpp` checks, via captured `cout`/`cerr` transcripts: DB and input
parsing, exact/closest-lower/too-early lookups, leap-year date validation, the
0–1000 value range, every subject error message, and the subject's example
session against the real `data.csv` — byte for byte.
