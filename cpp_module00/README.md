# Module 00 — Namespaces, classes, member functions, stdio streams

This module is where I stopped writing C that happened to compile with `c++` and started
writing actual C++. The arc is deliberate: ex00 looks like a one-liner and turns into a
lesson about I/O cost, ex01 forces a first honest class design with zero heap allocation,
and ex02 hands you someone else's header from 1989 and makes you meet its contract exactly.
Everything is built with `c++ -std=c++98 -Wall -Wextra -Werror`, so there was nowhere to
hide. The quiet theme running through all three exercises turned out to be the keyword
`static`, which means three unrelated things in this codebase — and I hit all three.

## ex00 — Megaphone

The subject wants a program that uppercases its arguments, or prints
`* LOUD AND UNBEARABLE FEEDBACK NOISE *` when there are none. Five lines would pass.
I wrote two solutions in `megaphone.cpp` and kept both: `other_solution()` is the
idiomatic answer (`std::transform` with `::toupper` per argument), and
`optimized_solution()` is a buffered pipeline I built to understand where stream time
actually goes. It accumulates into an 8192-byte stack buffer (`CHUNK_SIZE`) and drains it
with a single `std::cout.write(buf, buf_idx)` instead of hammering `operator<<(char)` in a
loop. Behavior is driven by a bitflag enum, `TfFlags` (`TF_TRIM`, `TF_COLLAPSE_WS`,
`TF_SKIP_CR` — each a power of two, combined with `|`, tested with `&`), and the character
transform is a first-class value: `typedef int (*char_fn)(int)` lets me pass `std::toupper`
or NULL for a pass-through.

The genuinely tricky part was whitespace across argument boundaries. Trimming and
collapsing runs is easy inside one string; doing it across `argv[1..n]` as one logical
stream needs carried state, so `buffered_transform_chunk()` threads two booleans —
`has_started` and `pending_space` — through every call. The test named
"boundary across args" (`"hello "` + `" world"` must give exactly `HELLO WORLD`) failed
more times than I want to admit before the pending-space logic settled. The other hurdle
was linkage: `tests/test.cpp` defines its own `main`, and since my `main` lives in
`megaphone.o` (which every test links), the fix was `__attribute__((weak)) int main` —
the test's strong symbol wins, the exercise binary still works alone.

The conscious trade-off: this is over-engineering for a shell-shouting toy, and I know it.
The per-exercise README documents a `HAVE_EXAM` compile-time toggle for switching to the
simple solution; the checked-in weak `main` is the debug variant that runs both back to
back so I can eyeball that they agree. `tests/benchmark.cpp` pushes 5 MB single arguments
and 50,000-argument fragmentation runs through both to keep the "optimized" claim honest.

## ex01 — My Awesome PhoneBook

The subject wants a phonebook of at most 8 contacts, no dynamic allocation, with `ADD`,
`SEARCH` and `EXIT`, where the 9th contact silently overwrites the oldest and every
displayed column is exactly 10 characters, truncated with a trailing dot. My `PhoneBook`
holds `Contact contact[MAX_CONTACT]` (a plain array, `MAX_CONTACT` is 8) plus three ints:
`cap`, `count`, `size`. The whole ring buffer is one line — `size = (size + 1) % cap` —
with `count` saturating at `cap`. `Contact` stores its five fields in an enum-indexed
array (`fields[FIELD_COUNT]`), so `setField`/`getField` are bounds-checked table lookups
and adding a field would mean adding one enum value before the `FIELD_COUNT` sentinel.

The bug that taught me the most here was the truncation rule: 10-character columns means
strings *longer* than 10 become 9 characters plus a dot — `formatField()` is
`s.substr(0, 9) + "."` guarded by `s.size() > 10`, and my first version chopped
10-character names like `"Maximilian"` that should pass through untouched. The subtler
lesson was in `Contact::getField()`: returning `""` on a bad index would hand back a
reference to a temporary, so `emptyString()` returns a function-local `static std::string`
that lives forever. That is the second face of `static`, and the first dangling-reference
bug I caught before it caught me.

Trade-offs I chose deliberately: the table (`BOX_TOP`/`BOX_MID`/`BOX_BOT` macros,
`std::setw(10)` with `std::left` for data and `std::right` for the index) is structurally
hardcoded — dynamic rows, fixed columns — because computing column widths in two passes
buys nothing at 8 rows. I also went past the subject with `isValidPhoneNumber()`
(clean-then-classify: strip separators, then match French `0…`/`+33`, Spanish `6-9…`/`+34`,
or an E.164 fallback of 9–15 digits) and a `seed` command that loads six fixture contacts,
because retyping test contacts by hand was how mistakes were slipping through.

## ex02 — The Account class

Here the subject provides `Account.hpp` verbatim — tabs, `#pragma once` *and* an include
guard, a `typedef Account t`, a private default constructor, and a fake 1989 header signed
by Brad "Buddy" McLane — plus `tests.cpp` as the main and a reference log,
`19920104_091532.log`. My only deliverable is `Account.cpp`, and the check is brutal and
fair: strip the timestamps with sed and diff against the log (the exact one-liner is kept
as a comment at the top of my `Account.cpp`). The four static members
(`_nbAccounts`, `_totalAmount`, `_totalNbDeposits`, `_totalNbWithdrawals`) are declared in
the header but defined once in the `.cpp` — forget that and the linker, not the compiler,
tells you. Each account captures `_accountIndex(_nbAccounts)` in the initializer list
*before* the body increments the counter, which is why indices come out 0..7.

What burned me was the withdrawal branch. The line starts identically either way
(`index:…;p_amount:…;withdrawal:`) and only then forks: `refused` with no counter updates
when `withdrawal > _amount`, or the amount plus `nb_withdrawals` on success. I initially
bumped `_totalNbWithdrawals` before the check and the global stats drifted by exactly the
refused operations — the log is an audit trail, and it audits you. I also learned that
`std::setfill('0')` is sticky while `setw` resets after every field, and that the input
arrays in `tests.cpp` ({42, 54, 957, 432, 1234, 0, 754, 16576} and friends) were never
secret: every value is reconstructible from the log's `p_amount`/`amount` pairs.

The trade-off: I left the provided files byte-untouched — mixed tab soup and all — and
put all my understanding into Doxygen comments in `Account.cpp` instead of "cleaning up"
a header I don't own. Matching someone else's contract exactly is the exercise.

## What stuck with me

Three exercises, three meanings of `static`: internal-linkage helpers in `megaphone.cpp`
and `PhoneBook.cpp`, the function-local `static` in `Contact::emptyString()` that makes
returning a reference safe, and the class-level shared counters in `Account`. I also keep
the ex00 lesson that cost lives in call counts, not in big-O — one `cout.write` per 8 KB
versus a million `operator<<` calls — and the ex02 lesson that a good log is a
specification you can diff against. And `% cap` is all a ring buffer ever was.

## Building and testing

Each exercise is self-contained: `cd ex00 && make` (same for `ex01`, `ex02`), binaries
land in `build/bin/exNN`, and `make run` builds then executes. All Makefiles use
`c++ -std=c++98 -Wall -Wextra -Werror`. One honest caveat: these module 00 Makefiles
(like modules 01–03's, apart from the regenerated module 01 ex04/ex05) only *build* the
test binaries — `make test` compiles `tests/*.cpp` into `build/bin/` and prints
"Executing", but you run them yourself, e.g. `./build/bin/test` and `./build/bin/benchmark`
in ex00. For ex02, `tests.cpp` is the main itself, so `make run` plus the sed-and-diff
line against the reference log is the whole verification.
