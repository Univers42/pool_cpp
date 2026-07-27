/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Differential test suite for Replacer: every successful case is compared
// against an independent KMP-based oracle (tests/kmp_ref.hpp). The shipped
// implementation uses std::string::find — two DIFFERENT algorithms agreeing
// on "replace every occurrence, left to right, non-overlapping" is the point.
// Run from the exercise dir (make test). Exits non-zero on any failure.
// Scratch files live under build/ so `make fclean` removes them.

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Replacer.hpp"
#include "kmp_ref.hpp"

static int g_fails = 0;

static bool writeFile(const std::string& path, const std::string& content) {
  std::ofstream f(path.c_str(),
                  std::ios::out | std::ios::binary | std::ios::trunc);
  if (!f.is_open()) return false;
  f.write(content.c_str(), static_cast<std::streamsize>(content.size()));
  f.close();
  return f.good();
}

static bool readFile(const std::string& path, std::string& out) {
  std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
  if (!f.is_open()) return false;
  std::ostringstream oss;
  oss << f.rdbuf();
  out = oss.str();
  return true;
}

// Independent reference: the original KMP implementation.
static std::string oracle(const std::string& text, const std::string& s1,
                          const std::string& s2) {
  return kmpReplace(text, s1, s2);
}

static std::string preview(const std::string& s) {
  std::ostringstream oss;
  std::string::size_type n = s.length() < 60 ? s.length() : 60;
  for (std::string::size_type i = 0; i < n; ++i) {
    unsigned char c = static_cast<unsigned char>(s[i]);
    if (c == '\n')
      oss << "\\n";
    else if (c == '\0')
      oss << "\\0";
    else if (c < 32 || c > 126)
      oss << "?";
    else
      oss << s[i];
  }
  if (s.length() > 60) oss << "...";
  return oss.str();
}

static const char* kIn = "build/t_in.txt";

static void runCase(const char* name, const std::string& text,
                    const std::string& s1, const std::string& s2) {
  if (!writeFile(kIn, text)) {
    std::cerr << "[SETUP-FAIL] " << name << ": cannot write scratch file (run "
              << "via `make test` from the exercise dir)\n";
    ++g_fails;
    return;
  }
  Replacer r(kIn, s1, s2);
  if (!r.process()) {
    std::cerr << "[FAIL] " << name << ": process() returned false\n";
    ++g_fails;
    return;
  }
  std::string got;
  if (!readFile(std::string(kIn) + ".replace", got)) {
    std::cerr << "[FAIL] " << name << ": no .replace file produced\n";
    ++g_fails;
    return;
  }
  const std::string want = oracle(text, s1, s2);
  if (got != want) {
    std::cerr << "[FAIL] " << name << "\n       want(" << want.length()
              << "): '" << preview(want) << "'\n       got (" << got.length()
              << "): '" << preview(got) << "'\n";
    ++g_fails;
    return;
  }
  std::string back;
  if (!readFile(kIn, back) || back != text) {
    std::cerr << "[FAIL] " << name << ": input file was modified\n";
    ++g_fails;
  }
}

static void expectFailure(const char* name, const std::string& file,
                          const std::string& s1, const std::string& s2) {
  Replacer r(file, s1, s2);
  if (r.process()) {
    std::cerr << "[FAIL] " << name << ": process() unexpectedly succeeded\n";
    ++g_fails;
    return;
  }
  std::ifstream out((file + ".replace").c_str());
  if (out.is_open()) {
    std::cerr << "[FAIL] " << name << ": stray .replace file was created\n";
    ++g_fails;
  }
}

// Deterministic LCG so fuzz failures reproduce identically everywhere.
static unsigned long g_seed = 42;
static unsigned long lcg() {
  g_seed = g_seed * 1103515245UL + 12345UL;
  return (g_seed >> 16) & 0x7fffUL;
}

static std::string randomString(std::string::size_type maxLen,
                                bool allowEmpty) {
  static const char alphabet[] = "ab ";
  std::string::size_type len = lcg() % (maxLen + 1);
  if (!allowEmpty && len == 0) len = 1;
  std::string s;
  for (std::string::size_type i = 0; i < len; ++i)
    s += alphabet[lcg() % (sizeof(alphabet) - 1)];
  return s;
}

int main() {
  // 1. Normal cases (ported from the old tests/test.sh harness).
  runCase("basic replacement", "hello world", "world", "42");
  runCase("multiple occurrences", "foo bar foo baz foo", "foo", "qux");
  runCase("multiline", "line1\nline2\nline3", "line", "row");

  // 2. Edge cases evaluators try.
  runCase("s2 empty (deletion)", "remove the bad word", "bad ", "");
  runCase("no occurrence", "untouched text", "missing", "here");
  runCase("empty file", "", "findme", "replace");
  runCase("s1 == s2", "same same same", "same", "same");
  runCase("s1 single char", "a", "a", "abcde");
  runCase("s1 longer than file", "short", "very long string", "x");
  runCase("whole file is one match", "exact match", "exact match", "perfect");
  runCase("match at both boundaries", "XmidX", "X", "yy");

  // 3. Overlap / self-referential patterns.
  runCase("continuous overlap", "aaaaa", "aa", "b");           // -> "bba"
  runCase("s2 contains s1 (no rescan)", "abab", "ab", "cab");  // -> "cabcab"
  runCase("periodic pattern", "abababab", "abab", "X");        // -> "XX"

  // 4. Binary safety: embedded NUL bytes flow through untouched.
  {
    const char rawText[] = {'a', '\0', 'b', ' ', 'a', '\0', 'a'};
    const char rawPat[] = {'a', '\0'};
    runCase("embedded NUL bytes", std::string(rawText, sizeof(rawText)),
            std::string(rawPat, sizeof(rawPat)), "Z");
  }

  // 5. Pathological pattern (KMP stress): long run of near-misses.
  {
    const std::string longA(30000, 'a');
    const std::string pat = std::string(300, 'a') + "b";
    runCase("pathological miss", longA, pat, "x");
    runCase("pathological hit at end", longA + "b", pat, "x");
  }

  // 6. Error paths (these legitimately print to std::cerr).
  writeFile("build/t_err.txt", "hello");
  expectFailure("empty s1 rejected", "build/t_err.txt", "", "x");
  expectFailure("missing input file", "build/t_missing.txt", "a", "b");

  // 7. Differential fuzz: 300 random triples vs the oracle.
  for (int it = 0; it < 300; ++it) {
    std::ostringstream name;
    name << "fuzz #" << it;
    runCase(name.str().c_str(), randomString(300, true), randomString(4, false),
            randomString(4, true));
    if (g_fails) break;  // first fuzz failure is enough — it reproduces
  }

  if (g_fails) {
    std::cerr << g_fails << " ex04 check(s) FAILED\n";
    return 1;
  }
  std::cout << "ex04 tests: all OK\n";
  return 0;
}
