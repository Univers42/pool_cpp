/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kmp_ref.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX04_TESTS_KMP_REF_HPP_
#define CPP_MODULE01_EX04_TESTS_KMP_REF_HPP_

// Reference replace-all built on Knuth-Morris-Pratt. This was the original
// Replacer implementation; it now serves as an INDEPENDENT algorithm the
// tests compare the shipped std::string::find implementation against
// (two different algorithms agreeing beats one algorithm agreeing with
// itself). Semantics: every occurrence, left to right, non-overlapping.

#include <string>

static std::string kmpReplace(const std::string& text, const std::string& s1,
                              const std::string& s2) {
  typedef std::string::size_type size_type;
  if (s1.empty()) return text;

  const size_type N = text.length();
  const size_type M = s1.length();

  // LPS (Longest Prefix Suffix) table.
  size_type* lps = new size_type[M];
  lps[0] = 0;
  {
    size_type len = 0;
    size_type i = 1;
    while (i < M) {
      if (s1[i] == s1[len]) {
        ++len;
        lps[i] = len;
        ++i;
      } else if (len != 0) {
        len = lps[len - 1];
      } else {
        lps[i] = 0;
        ++i;
      }
    }
  }

  std::string out;
  out.reserve(N);
  size_type i = 0;     // text index
  size_type j = 0;     // pattern index
  size_type last = 0;  // end of last region already copied

  while (i < N) {
    if (s1[j] == text[i]) {
      ++j;
      ++i;
    }
    if (j == M) {
      out.append(text, last, i - M - last);
      out += s2;
      last = i;
      j = 0;  // non-overlapping, sed-style
    } else if (i < N && s1[j] != text[i]) {
      if (j != 0)
        j = lps[j - 1];
      else
        ++i;
    }
  }
  out.append(text, last, N - last);
  delete[] lps;
  return out;
}

#endif  // CPP_MODULE01_EX04_TESTS_KMP_REF_HPP_
