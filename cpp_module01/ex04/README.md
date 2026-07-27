# ex04 — Sed is for losers

Replace every occurrence of `s1` with `s2` in a file, writing the result to
`<filename>.replace`. C file functions are forbidden; every `std::string`
member is allowed **except** `replace`.

```sh
make
./build/bin/ex04 <filename> <s1> <s2>
```

## Concept: C++ file streams + string searching

The program is a pipeline: `std::ifstream` reads the whole file into a
`std::string` (binary mode, via `rdbuf()`, so NUL bytes and any encoding pass
through untouched), the string is scanned for matches, and `std::ofstream`
writes out the stitched result. No `FILE*`, no `fopen` — RAII streams close
themselves, and stream state (`is_open`, `bad`, `good`) is how errors are
detected instead of errno.

Replacement semantics are the standard sed ones: **left to right, greedy,
non-overlapping**. After a match, scanning resumes *after* the replaced
region, so the inserted `s2` is never rescanned (`abab` with `s1=ab`,
`s2=cab` gives `cabcab`, not an infinite loop).

## The algorithm story (and a lesson in measuring)

This exercise originally shipped a hand-written Knuth-Morris-Pratt search —
the textbook O(N+M) answer to substring matching. Benchmarking it against a
plain `std::string::find` + advance loop produced this (16 MB prose file,
g++ 15, `make test` re-measures on every run):

| workload                   | Replacer (`find`) | hand-rolled KMP |
|----------------------------|-------------------|-----------------|
| prose, 372k matches        | ~62 ms            | ~735 ms         |
| prose, pattern absent      | ~84 ms            | ~707 ms         |
| pathological (`aaa…ab`)    | ~40 ms            | ~80 ms          |

The "naive" `find` wins everywhere — including the near-miss-heavy case
constructed to favor KMP — because libstdc++'s `find` sits on `memchr`/
`memcmp`, which are SIMD-vectorized, while a hand-rolled KMP compares one
byte per branch. Big-O describes comparisons, not nanoseconds.

So the shipped `Replacer::process()` is the ~10-line `find` loop, and the KMP
lives on in `tests/kmp_ref.hpp` as an **independent reference algorithm**:
every test compares the two implementations byte-for-byte, which is a much
stronger check than an algorithm agreeing with itself.

## Error handling

- Wrong argument count → usage on stderr, exit 1.
- Missing/unreadable input file → error on stderr, exit 1, and no
  `.replace` file is created (output is only opened after a successful read).
- Empty `s1` → rejected explicitly (a zero-length pattern has no sane
  match semantics; sed errors on it too).
- Short writes (disk full) → detected via `ofs.good()` after `flush()`,
  reported, exit 1.

## Tests

`make test` builds and runs two binaries from `tests/`:

- `test` — ~320 differential cases against the KMP oracle: the classics
  (multiple occurrences, deletion, `s1 == s2`, whole-file match, boundary
  matches), overlap traps (`aaaaa`/`aa`, periodic patterns), embedded NUL
  bytes, 30 kB pathological patterns, error paths, and a 300-case
  deterministic fuzzer. Also asserts the input file is never modified.
- `bench` — the table above; fails if the two implementations ever disagree.

Both exit non-zero on failure. Scratch files live under `build/`, so
`make fclean` removes every artifact.
