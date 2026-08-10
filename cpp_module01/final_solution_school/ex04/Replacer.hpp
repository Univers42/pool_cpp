/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replacer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:30:18 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:04:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX04_REPLACER_HPP_
#define CPP_MODULE01_EX04_REPLACER_HPP_

#include <string>

/**
 * @brief Reads a file, replaces every non-overlapping occurrence of a search
 *        string with a replacement string, and writes the result to a new
 *        file named `<filename>.replace`.
 *
 * @details
 * ### Algorithm — Knuth-Morris-Pratt (KMP)
 * A naïve search would compare every position in the text against the full
 * pattern, giving **O(N × M)** in the worst case.  KMP achieves **O(N + M)**
 * by pre-processing the pattern once to build an LPS (Longest Proper Prefix
 * which is also Suffix) array.  That array lets the search loop skip ahead
 * in the pattern instead of restarting from the beginning after a partial
 * match, so no character in the text is ever compared more than twice.
 *
 * #### LPS array in plain English
 * For each position `i` in the pattern, `lps[i]` stores the length of the
 * longest proper prefix of `pattern[0..i]` that is also a suffix of that
 * same substring.  Example for pattern `"ABACABA"`:
 * ```
 * index :  0  1  2  3  4  5  6
 * char  :  A  B  A  C  A  B  A
 * lps   :  0  0  1  0  1  2  3
 * ```
 * When a mismatch occurs at pattern position `j`, instead of resetting `j`
 * to 0, we jump to `lps[j-1]`, reusing the characters we already matched.
 *
 * #### Replacement strategy
 * Rather than rebuilding a new string in memory (which would require
 * `std::string::replace` and repeated copies), the output is streamed
 * directly to the output file:
 * 1. Each time a match ends at text position `i`, write
 *    `text[last_match_end .. i-M)` verbatim.
 * 2. Write `s2` (the replacement).
 * 3. Advance `last_match_end` to `i` and reset the pattern index to `0`,
 *    preventing overlapping matches.
 * 4. After the loop, flush whatever text remains after the last match.
 *
 * ### File naming convention
 * | Role        | Path                   |
 * |-------------|------------------------|
 * | Input       | `<filename>`           |
 * | Output      | `<filename>.replace`   |
 *
 * ### Constraints respected
 * - `std::string::replace` is **not** used (forbidden by the subject).
 * - `std::vector` is **not** used; the LPS array is a raw `int[]` allocated
 *   with `new[]` and freed with `delete[]`.
 * - Files are opened in binary mode to preserve line endings exactly.
 */
class Replacer {
 private:
  std::string inFile;  ///< Path to the source file to read.
  std::string outFile; ///< Path to the output file (`inFile + ".replace"`).
  std::string s1;      ///< The search pattern — must not be empty.
  std::string s2;      ///< The replacement string — may be empty.

  /**
   * @brief Builds the KMP Longest-Proper-Prefix-Suffix (LPS) array for @c s1.
   *
   * @details
   * Runs in **O(M)** time and **O(M)** space where M = `s1.length()`.
   * The array is used by @ref process to skip unnecessary comparisons during
   * the text search phase.
   *
   * Algorithm (informal):
   * - Maintain a `len` cursor that tracks the current longest matching prefix.
   * - Walk index `i` from 1 to M-1:
   *   - If `s1[i] == s1[len]`, extend the match: `lps[i] = ++len`.
   *   - If they differ and `len > 0`, fall back: `len = lps[len-1]` (do NOT
   *     increment `i` — we retry the comparison with the shorter prefix).
   *   - If they differ and `len == 0`, no prefix matches: `lps[i] = 0`.
   *
   * @param lps Caller-allocated array of size `s1.length()`.  On return,
   *            `lps[i]` holds the length of the longest proper prefix of
   *            `s1[0..i]` that is also a suffix of that substring.
   */
  void computeLPSArray(int* lps) const;

 public:
  /**
   * @brief Constructs a Replacer and configures the file paths and strings.
   *
   * The output file path is derived automatically as `filename + ".replace"`.
   * No file I/O occurs at construction time.
   *
   * @param filename Path to the file whose content will be searched.
   * @param s1       The string to search for.  Must not be empty; @ref process
   *                 will return @c false and print an error if it is.
   * @param s2       The string to substitute in place of every occurrence of
   *                 @p s1.  May be empty (effectively deletes all occurrences).
   */
  Replacer(const std::string& filename, const std::string& s1,
           const std::string& s2);

  /** @brief Destructs the Replacer. No open handles are held after construction. */
  ~Replacer();

  /**
   * @brief Performs the search-and-replace operation.
   *
   * @details
   * Steps performed:
   * 1. Validates that @c s1 is not empty.
   * 2. Opens @c inFile for reading (binary mode).
   * 3. Opens @c outFile for writing (binary mode), creating it if absent.
   * 4. Bulk-loads the entire input into a `std::string` via `rdbuf()`.
   * 5. Builds the KMP LPS array by calling @ref computeLPSArray.
   * 6. Runs the KMP search loop, streaming unmodified segments and
   *    replacements directly to the output file.
   * 7. Flushes any trailing text after the last match.
   * 8. Frees the LPS array and closes both file streams.
   *
   * @return @c true on success; @c false if @c s1 is empty, the input file
   *         cannot be opened, or the output file cannot be created.
   */
  bool process();
};

#endif  // CPP_MODULE01_EX04_REPLACER_HPP_
