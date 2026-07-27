# ex02 — PmergeMe

## The concept

This exercise teaches the **Ford-Johnson algorithm** (merge-insertion sort,
Knuth TAOCP Vol. 3, p. 184) and how **container choice affects performance**
for the exact same algorithm. Ford-Johnson minimizes the number of
comparisons by combining three ideas:

1. **Pairing**: group the input into pairs, put the larger element of each
   pair first. An odd leftover element is set aside (the *straggler*).
2. **Sort the pairs** recursively by their larger element. The larger
   elements now form a sorted *main chain*; each smaller partner (*pend*
   element) is known to be `<=` some element already in the chain.
3. **Jacobsthal insertion**: insert the pend elements into the main chain by
   binary search, in the order given by the Jacobsthal numbers
   (1, 3, 5, 11, 21, ...). Inserting between consecutive Jacobsthal indices
   in *descending* order keeps every binary search inside a range of
   `2^k - 1` elements, which is the comparison-optimal window.

## How this code demonstrates it

- `PmergeMe.{hpp,cpp}` — an Orthodox-Canonical-Form class with the algorithm
  implemented **twice**, once for `std::vector<int>` and once for
  `std::deque<int>`, as the subject strongly advises (no generic function).
  `sortVector`/`sortDeque` do pairing + straggler handling,
  `sortPairs*`/`mergePairs*` merge-sort the pairs by larger element, and
  `generateJacobsthal*` produces the insertion order.
- `main.cpp` — parses the arguments (`strtol`, rejecting anything that is
  not an integer in `[1, INT_MAX]` — zero is not positive; any bad token
  prints `Error` to stderr),
  prints the `Before:`/`After:` lines, and times each container. Per the
  subject, the timed section includes **data management**: filling the
  container from the parsed input *and* sorting it.
- Container choice per subject rule "one container per exercise, never
  reused": ex00 took `std::map`, ex01 took `std::stack` over `std::list`,
  leaving `vector` + `deque` for the two containers required here. `vector`
  wins in practice: contiguous memory makes its `insert` memmove cheaper
  than deque's paged bookkeeping.

Deliberate shortcuts are marked with `// ponytail:` comments in the sources
(full-range `lower_bound` instead of a partner-bounded search; display
truncated at 10 elements).

## Build / run / test

```sh
make                # builds build/bin/PmergeMe
./build/bin/PmergeMe 3 5 9 7 4
./build/bin/PmergeMe `shuf -i 1-100000 -n 3000 | tr "\n" " "`
./build/bin/PmergeMe "-1" "2"   # -> Error (stderr), exit 1

make test           # builds and runs tests/test.cpp
make fclean         # removes all build artifacts
```

`tests/test.cpp` asserts that both containers match `std::sort` on: empty
and single-element input, the subject example, duplicates, sorted/reversed/
all-equal input, and 3000 (even) + 3001 (odd, straggler path) shuffled
distinct integers.
