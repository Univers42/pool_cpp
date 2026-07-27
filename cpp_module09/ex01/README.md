# C++ Module 09 — ex01: Reverse Polish Notation (RPN)

## The concept

Reverse Polish (postfix) notation puts the operator *after* its operands:
infix `(8 * 9) - 9` becomes `8 9 * 9 -`. Because every operator applies to
the two most recently produced values, no parentheses or precedence rules
are needed — the token order alone encodes the evaluation order.

The exercise teaches the canonical stack algorithm and, more subtly, that
`std::stack` is a **container adaptor**: a restricted LIFO interface layered
over a real sequence container that you can choose.

### The algorithm (single left-to-right pass, O(n))

1. Token is a digit → push it.
2. Token is `+ - * /` → pop the right operand, pop the left operand,
   compute `left op right`, push the result.
3. End of input → exactly one value must remain on the stack; that is the
   result. Anything else (leftover operands, missing operands, junk tokens,
   division by zero) is an error.

Trace of `7 7 * 7 -`: push 7, push 7 → `*` pops 7,7 pushes 49 → push 7 →
`-` pops 7,49 pushes 42. One value left: **42**.

## How this code demonstrates it

- `RPN` (Orthodox Canonical Form) owns one container:

  ```cpp
  std::stack<int, std::list<int> > _stack;
  ```

  The second template argument replaces `std::stack`'s default backing
  container (`std::deque`) with `std::list`. This is deliberate: module 09
  forbids reusing a container across exercises, and ex02 (PmergeMe) needs
  `std::vector` and `std::deque`. Adapting `std::list` keeps both free
  while still satisfying "use at least one container" here.

- `RPN::evaluate(const std::string&)` tokenizes with `std::istringstream`,
  runs the algorithm above, and **returns** the result; printing is left to
  `main`. Any invalid input throws `std::runtime_error("Error")`, which
  `main` prints to **stderr** (exit status 1), matching the subject
  verbatim. Operands are single digits by subject guarantee, so any token
  that is not one digit or one operator (brackets, decimals, `10`, `-1`,
  letters) is rejected.

- Intermediate values and the result are ordinary `int`s and may exceed 9
  or go negative — only the *input* operands are restricted.

## Build / run / test

```sh
make                 # builds build/bin/RPN
./build/bin/RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"   # -> 42
./build/bin/RPN "(1 + 1)"                      # -> Error (on stderr)

make test            # builds and runs tests/test.cpp (exits non-zero on failure)
make fclean          # removes build/
```

`tests/test.cpp` checks the three subject examples, operand order
(`3 4 -` is −1), integer truncation, results outside 0–9, all the error
classes (brackets, empty input, missing/leftover operands, division by
zero, multi-char and decimal tokens), and that one `RPN` object can be
reused without stale stack state.
