/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replacer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:30:16 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:45:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Replacer.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

/// @brief Stores file paths and search/replacement strings; no I/O performed.
/// @param filename Source file path; output will be written to filename+".replace".
/// @param s1 Search pattern (must not be empty).
/// @param s2 Replacement string (may be empty).
Replacer::Replacer(const std::string& filename, const std::string& s1,
                   const std::string& s2)
    : inFile(filename), outFile(filename + ".replace"), s1(s1), s2(s2) {}

/// @brief Destructs the Replacer (no open resources to release).
Replacer::~Replacer() {}

/// @brief Builds the KMP LPS array for s1 in O(M) time.
/// @details
/// `lps[i]` = length of the longest proper prefix of s1[0..i] that is also
/// a suffix.  When a mismatch occurs at pattern index j, the search jumps to
/// lps[j-1] instead of resetting to 0, avoiding redundant comparisons.
/// @param lps Caller-allocated array of size s1.length(). Filled on return.
void Replacer::computeLPSArray(int* lps) const {
  int len = 0;
  lps[0] = 0;
  int i = 1;
  int M = this->s1.length();

  while (i < M) {
    if (this->s1[i] == this->s1[len]) {
      len++;
      lps[i] = len;
      i++;
    } else {
      if (len != 0) {
        len = lps[len - 1];
      } else {
        lps[i] = 0;
        i++;
      }
    }
  }
}

/// @brief Runs the KMP search-and-replace, writing output to inFile+".replace".
/// @details
/// 1. Validates s1 is non-empty.
/// 2. Opens inFile (read, binary) and outFile (write, binary).
/// 3. Bulk-loads the input via rdbuf() — one allocation, no char-by-char I/O.
/// 4. Allocates the LPS array with new[] (std::vector is forbidden).
/// 5. KMP loop: on match, flushes the verbatim segment then writes s2.
///    last_match_end tracks where the next verbatim segment starts so that
///    matched text is never written to the output.
/// 6. Flushes any trailing text after the final match.
/// 7. Frees the LPS array with delete[] and closes both streams.
/// @return true on success; false if s1 is empty or any file cannot be opened.
bool Replacer::process() {
  if (this->s1.empty()) {
    std::cerr << "Error: The search string cannot be empty.\n";
    return false;
  }

  std::ifstream ifs(this->inFile.c_str(), std::ios::in | std::ios::binary);
  if (!ifs.is_open()) {
    std::cerr << "Error: Cannot open input file '" << this->inFile << "'.\n";
    return false;
  }

  std::ofstream ofs(this->outFile.c_str(), std::ios::out | std::ios::binary);
  if (!ofs.is_open()) {
    std::cerr << "Error: Cannot create output file '" << this->outFile
              << "'.\n";
    ifs.close();
    return false;
  }

  // Bulk-load: rdbuf() copies the entire file content into the stream buffer
  // in a single system call, avoiding per-character overhead.
  std::ostringstream oss;
  oss << ifs.rdbuf();
  std::string text = oss.str();

  int N = text.length();
  int M = this->s1.length();

  // new[] instead of std::vector because the subject forbids <vector>.
  // Must be freed with delete[] before every return path.
  int* lps = new int[M];
  computeLPSArray(lps);

  int i = 0;              // Cursor into text[] — never decrements (O(N) guarantee).
  int j = 0;              // Cursor into s1[]  — jumps via lps on mismatch.
  int last_match_end = 0; // Start of the next verbatim segment to stream out.

  // KMP search loop — i advances monotonically, so total iterations <= 2N.
  while (i < N) {
    if (this->s1[j] == text[i]) {
      j++;
      i++;
    }

    if (j == M) {
      // Full match found ending at text[i-1].
      // Stream the verbatim text that preceded this match, then the replacement.
      ofs << text.substr(last_match_end, i - M - last_match_end);
      ofs << this->s2;

      last_match_end = i; // Skip past the matched region in the input.
      j = 0; // Reset pattern index — non-overlapping replacement.
    } else if (i < N && this->s1[j] != text[i]) {
      // Mismatch: use LPS to skip the pattern prefix we already verified,
      // keeping i in place so text[i] is retried against s1[lps[j-1]].
      if (j != 0) {
        j = lps[j - 1];
      } else {
        i++;
      }
    }
  }

  // Flush any text that follows the last match (or the entire text if no match).
  if (last_match_end < N) {
    ofs << text.substr(last_match_end);
  }

  // Free the raw array before closing — avoids a leak on every return path.
  delete[] lps;

  ifs.close();
  ofs.close();

  return true;
}
