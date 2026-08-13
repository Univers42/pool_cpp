/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 00:00:00 by marvin            #+#    #+#             */
/*   Updated: 2026/08/11 00:00:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Deterministic + fuzz tests for ex00 ScalarConverter.
//
//   make test              build and run
//   ./build/bin/test       run directly
//   ./build/bin/test 1234  different fuzz seed
//   ./build/bin/test long  1,000,000 fuzz cases per layer
//   ./tests/mutants.sh     prove these tests can actually fail
//
// Three layers, three different notions of "correct":
//
//   table    literal -> exact expected output, hand-derived from the subject.
//   model    literals built from known values, checked against an INDEPENDENT
//            implementation of the spec: acceptance re-derived from strtod's
//            endptr plus a character whitelist (a different derivation from
//            ScalarConverter's single-pass scanner), and formatting via
//            snprintf("%g") rather than iostreams.
//   chaos    random bytes. The value is unpredictable, so this layer asserts
//            invariants that must hold for every input.

// No <cstdio>: the *printf family is forbidden by the evaluation sheet, and
// std::snprintf is not C++98 anyway. Everything formats through ostringstream.
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "ScalarConverter.hpp"

// ─── harness ────────────────────────────────────────────────────────────────

namespace t {

struct Section {
  const char* name;
  int checks;
  int failed;
};

// Fixed-size storage: the subject forbids STL containers before Module 08,
// so the harness uses plain arrays with an explicit count.
static const int kMaxSections = 16;
static const int kMaxErrors = 10;
static const int kMaxCover = 24;

static Section g_sections[kMaxSections];
static int g_sectionCount = 0;
static std::string g_errors[kMaxErrors];
static int g_errorCount = 0;

static const char* g_coverName[kMaxCover];
static long g_coverCount[kMaxCover];
static int g_coverUsed = 0;

static void section(const char* name) {
  if (g_sectionCount >= kMaxSections) return;
  Section& s = g_sections[g_sectionCount++];
  s.name = name;
  s.checks = 0;
  s.failed = 0;
}

static void ok(bool cond, const std::string& detail) {
  Section& s = g_sections[g_sectionCount - 1];
  ++s.checks;
  if (cond) return;
  ++s.failed;
  if (g_errorCount < kMaxErrors)
    g_errors[g_errorCount++] = std::string(s.name) + " — " + detail;
}

// The coverage key set is small and known, so a linear scan over a fixed array
// does the job a keyed container would.
static long& cover(const char* branch) {
  for (int i = 0; i < g_coverUsed; ++i)
    if (std::strcmp(g_coverName[i], branch) == 0) return g_coverCount[i];
  g_coverName[g_coverUsed] = branch;
  g_coverCount[g_coverUsed] = 0;
  return g_coverCount[g_coverUsed++];
}

static void hit(const char* branch) { ++cover(branch); }

static int report(const char* title) {
  int checks = 0, failed = 0;
  std::cout << "\n" << title << "\n" << std::endl;
  for (int i = 0; i < g_sectionCount; ++i) {
    const Section& s = g_sections[i];
    // Clamped: an unsigned 48 - len would wrap for a long name and throw.
    size_t len = std::string(s.name).length();
    std::string dots(len < 48 ? 48 - len : 1, '.');
    std::cout << "  " << s.name << " " << dots << " " << s.checks << " "
              << (s.failed ? "FAILED" : "ok") << std::endl;
    checks += s.checks;
    failed += s.failed;
  }
  std::cout << std::endl;
  if (!failed) {
    std::cout << "  " << g_sectionCount << " sections, " << checks
              << " checks, all passed" << std::endl;
    return 0;
  }
  std::cout << "  " << failed << " of " << checks << " checks FAILED"
            << std::endl
            << std::endl;
  for (int i = 0; i < g_errorCount; ++i)
    std::cout << "    " << g_errors[i] << std::endl;
  return 1;
}

}  // namespace t

static unsigned char uc(char c) { return static_cast<unsigned char>(c); }

// Stream-based replacement for the snprintf calls this file used to make.
// ostream's default float formatting is %g with 6 significant digits, which is
// exactly what ScalarConverter's own output goes through.
template <typename T>
static std::string toStr(const T& v) {
  std::ostringstream o;
  o << v;
  return o.str();
}

// Run convert() with cout redirected, return everything it printed.
static std::string capture(const std::string& literal) {
  std::ostringstream out;
  std::streambuf* old = std::cout.rdbuf(out.rdbuf());
  ScalarConverter::convert(literal);
  std::cout.rdbuf(old);
  return out.str();
}

// Printable rendering of arbitrary input, so failure reports stay readable.
static std::string show(const std::string& s) {
  std::string r;
  for (size_t i = 0; i < s.length(); ++i) {
    if (s[i] == '\n') {
      r += "\\n";
    } else if (std::isprint(uc(s[i]))) {
      r += s[i];
    } else {
      static const char* kHex = "0123456789abcdef";
      r += "\\x";
      r += kHex[uc(s[i]) >> 4];
      r += kHex[uc(s[i]) & 0x0f];
    }
  }
  return r;
}

static std::string diff(const std::string& in, const std::string& want,
                        const std::string& got) {
  return "[" + show(in) + "] want [" + show(want) + "] got [" + show(got) + "]";
}

// ─── the independent model ──────────────────────────────────────────────────
//
// Derived a different way than ScalarConverter on purpose: acceptance is
// decided by what strtod consumes plus an explicit character whitelist, not by
// a hand-rolled scanner.

static const char* kAllImpossible =
    "char: impossible\nint: impossible\nfloat: impossible\ndouble: "
    "impossible\n";

static bool modelAccepts(const std::string& s, double* out) {
  if (s.empty()) return false;

  // char literals: 'x', or a lone printable non-digit
  if (s.length() == 3 && s[0] == '\'' && s[2] == '\'') {
    *out = static_cast<double>(s[1]);
    return true;
  }
  if (s.length() == 1 && std::isprint(uc(s[0])) && !std::isdigit(uc(s[0]))) {
    *out = static_cast<double>(s[0]);
    return true;
  }

  // pseudo-literals: exact whitelist, nothing else
  static const char* kPseudo[] = {"nan",  "nanf",  "+inf", "+inff",
                                  "-inf", "-inff", "inf",  "inff"};
  for (int i = 0; i < 8; ++i)
    if (s == kPseudo[i]) {
      *out = std::strtod(s.c_str(), NULL);
      return true;
    }

  // numeric: strip at most one trailing 'f'; every remaining byte must be a
  // digit, sign or dot (this kills 0x5, 1e10, leading blanks, "infinity");
  // then strtod must consume all of it (this kills "++42", "4.2.3", ".", "-").
  std::string num = s;
  if (num[num.length() - 1] == 'f') num.erase(num.length() - 1);
  if (num.empty()) return false;
  for (size_t i = 0; i < num.length(); ++i)
    if (!std::strchr("0123456789+-.", num[i])) return false;

  char* endp = NULL;
  double d = std::strtod(num.c_str(), &endp);
  if (endp == NULL || *endp != '\0') return false;
  *out = d;
  return true;
}

static bool wantsDot(double v) {
  return v == std::floor(v) && std::fabs(v) < 1e6;
}

static std::string modelOutput(double v) {
  std::string r;

  if (std::isnan(v) || v < 0.0 || v > 127.0) {
    r += "char: impossible\n";
  } else if (!std::isprint(static_cast<int>(v))) {
    r += "char: Non displayable\n";
  } else {
    r += "char: '";
    r += static_cast<char>(v);
    r += "'\n";
  }

  if (std::isnan(v) ||
      v < static_cast<double>(std::numeric_limits<int>::min()) ||
      v > static_cast<double>(std::numeric_limits<int>::max())) {
    r += "int: impossible\n";
  } else {
    r += "int: ";
    r += toStr(static_cast<int>(v));
    r += "\n";
  }

  float f = static_cast<float>(v);
  r += "float: ";
  r += toStr(f);
  if (wantsDot(static_cast<double>(f))) r += ".0";
  r += "f\n";

  r += "double: ";
  r += toStr(v);
  if (wantsDot(v)) r += ".0";
  r += "\n";

  return r;
}

// ─── layer 1: the deterministic table ───────────────────────────────────────

struct Case {
  const char* in;
  const char* want;
};

static void table_subject_examples() {
  t::section("subject's own examples");
  static const Case k[] = {
      {"0", "char: Non displayable\nint: 0\nfloat: 0.0f\ndouble: 0.0\n"},
      {"nan", "char: impossible\nint: impossible\nfloat: nanf\ndouble: nan\n"},
      {"42.0f", "char: '*'\nint: 42\nfloat: 42.0f\ndouble: 42.0\n"},
  };
  for (size_t i = 0; i < sizeof(k) / sizeof(k[0]); ++i) {
    std::string got = capture(k[i].in);
    t::ok(got == k[i].want, diff(k[i].in, k[i].want, got));
  }
}

static void table_plain_literals() {
  t::section("int, double and float literals");
  static const Case k[] = {
      {"42", "char: '*'\nint: 42\nfloat: 42.0f\ndouble: 42.0\n"},
      {"+42", "char: '*'\nint: 42\nfloat: 42.0f\ndouble: 42.0\n"},
      {"-42", "char: impossible\nint: -42\nfloat: -42.0f\ndouble: -42.0\n"},
      {"42f", "char: '*'\nint: 42\nfloat: 42.0f\ndouble: 42.0\n"},
      {"0f", "char: Non displayable\nint: 0\nfloat: 0.0f\ndouble: 0.0\n"},
      {"4.2", "char: Non displayable\nint: 4\nfloat: 4.2f\ndouble: 4.2\n"},
      {"4.2f", "char: Non displayable\nint: 4\nfloat: 4.2f\ndouble: 4.2\n"},
      {"4.", "char: Non displayable\nint: 4\nfloat: 4.0f\ndouble: 4.0\n"},
      {".5", "char: Non displayable\nint: 0\nfloat: 0.5f\ndouble: 0.5\n"},
      {"-.5", "char: impossible\nint: 0\nfloat: -0.5f\ndouble: -0.5\n"},
  };
  for (size_t i = 0; i < sizeof(k) / sizeof(k[0]); ++i) {
    std::string got = capture(k[i].in);
    t::ok(got == k[i].want, diff(k[i].in, k[i].want, got));
  }
}

static void table_char_literals() {
  t::section("char literals, including punctuation");
  static const Case k[] = {
      {"c", "char: 'c'\nint: 99\nfloat: 99.0f\ndouble: 99.0\n"},
      {"a", "char: 'a'\nint: 97\nfloat: 97.0f\ndouble: 97.0\n"},
      {"'a'", "char: 'a'\nint: 97\nfloat: 97.0f\ndouble: 97.0\n"},
      {"'0'", "char: '0'\nint: 48\nfloat: 48.0f\ndouble: 48.0\n"},
      {"f", "char: 'f'\nint: 102\nfloat: 102.0f\ndouble: 102.0\n"},
      {"'*'", "char: '*'\nint: 42\nfloat: 42.0f\ndouble: 42.0\n"},
      // A lone printable non-digit is a char literal even when it looks like a
      // malformed number. "+" is the character '+', not a broken int.
      {"+", "char: '+'\nint: 43\nfloat: 43.0f\ndouble: 43.0\n"},
      {"-", "char: '-'\nint: 45\nfloat: 45.0f\ndouble: 45.0\n"},
      {".", "char: '.'\nint: 46\nfloat: 46.0f\ndouble: 46.0\n"},
      {"'", "char: '''\nint: 39\nfloat: 39.0f\ndouble: 39.0\n"},
  };
  for (size_t i = 0; i < sizeof(k) / sizeof(k[0]); ++i) {
    std::string got = capture(k[i].in);
    t::ok(got == k[i].want, diff(k[i].in, k[i].want, got));
  }
}

static void table_boundaries() {
  t::section("the boundaries the README pins down");
  static const Case k[] = {
      {"31", "char: Non displayable\nint: 31\nfloat: 31.0f\ndouble: 31.0\n"},
      {"32", "char: ' '\nint: 32\nfloat: 32.0f\ndouble: 32.0\n"},
      {"126", "char: '~'\nint: 126\nfloat: 126.0f\ndouble: 126.0\n"},
      {"127",
       "char: Non displayable\nint: 127\nfloat: 127.0f\ndouble: 127.0\n"},
      {"128", "char: impossible\nint: 128\nfloat: 128.0f\ndouble: 128.0\n"},
      {"-1", "char: impossible\nint: -1\nfloat: -1.0f\ndouble: -1.0\n"},
      {"2147483647",
       "char: impossible\nint: 2147483647\nfloat: 2.14748e+09f\ndouble: "
       "2.14748e+09\n"},
      {"2147483648",
       "char: impossible\nint: impossible\nfloat: 2.14748e+09f\ndouble: "
       "2.14748e+09\n"},
      {"-2147483648",
       "char: impossible\nint: -2147483648\nfloat: -2.14748e+09f\ndouble: "
       "-2.14748e+09\n"},
      {"9999999999",
       "char: impossible\nint: impossible\nfloat: 1e+10f\ndouble: 1e+10\n"},
      // the documented wantsPointZero ceiling
      {"999999",
       "char: impossible\nint: 999999\nfloat: 999999.0f\ndouble: 999999.0\n"},
      {"1000000",
       "char: impossible\nint: 1000000\nfloat: 1e+06f\ndouble: 1e+06\n"},
  };
  for (size_t i = 0; i < sizeof(k) / sizeof(k[0]); ++i) {
    std::string got = capture(k[i].in);
    t::ok(got == k[i].want, diff(k[i].in, k[i].want, got));
  }
}

static void table_pseudo_literals() {
  t::section("all eight pseudo-literals");
  static const Case k[] = {
      {"nan", "char: impossible\nint: impossible\nfloat: nanf\ndouble: nan\n"},
      {"nanf", "char: impossible\nint: impossible\nfloat: nanf\ndouble: nan\n"},
      {"inf", "char: impossible\nint: impossible\nfloat: inff\ndouble: inf\n"},
      {"inff", "char: impossible\nint: impossible\nfloat: inff\ndouble: inf\n"},
      {"+inf", "char: impossible\nint: impossible\nfloat: inff\ndouble: inf\n"},
      {"+inff",
       "char: impossible\nint: impossible\nfloat: inff\ndouble: inf\n"},
      {"-inf",
       "char: impossible\nint: impossible\nfloat: -inff\ndouble: -inf\n"},
      {"-inff",
       "char: impossible\nint: impossible\nfloat: -inff\ndouble: -inf\n"},
  };
  for (size_t i = 0; i < sizeof(k) / sizeof(k[0]); ++i) {
    std::string got = capture(k[i].in);
    t::ok(got == k[i].want, diff(k[i].in, k[i].want, got));
  }
}

static void table_rejects() {
  t::section("the traps the validators exist for");
  static const char* kBad[] = {
      "",        "abc",  "0x5",  "1e10", "infinity", " 42",  "42 ",
      "++42",    "--42", "4.2.3", "42abc", "42ff",   "ff",   "-f",
      "..",      "'ab'", "''",   "NAN",  "INF",      "4-2",
  };
  for (size_t i = 0; i < sizeof(kBad) / sizeof(kBad[0]); ++i) {
    std::string got = capture(kBad[i]);
    t::ok(got == kAllImpossible, diff(kBad[i], kAllImpossible, got));
  }
}

// ─── layer 2: structured fuzz against the model ─────────────────────────────

// Build a literal we know to be valid, from a value we already know.
static std::string makeLiteral(int kind) {
  switch (kind % 7) {
    case 0: {  // random int, sometimes pushed past INT_MAX
      long v = (static_cast<long>(std::rand()) << 16) ^ std::rand();
      if (std::rand() % 2) v = -v;
      if (std::rand() % 8 == 0) v *= 1000;
      return toStr(v) + (std::rand() % 4 == 0 ? "f" : "");
    }
    case 1: {  // random decimal, fixed notation (the grammar has no exponent)
      double d = (std::rand() % 2000000 - 1000000) /
                 static_cast<double>(1 + std::rand() % 1000);
      std::ostringstream o;
      o << std::fixed << std::setprecision(6) << d;
      return o.str() + (std::rand() % 3 == 0 ? "f" : "");
    }
    case 2: {  // clustered on the char boundaries
      return toStr(std::rand() % 160 - 16);
    }
    case 3: {  // quoted char literal
      std::string s = "'";
      s += static_cast<char>(32 + std::rand() % 95);
      s += "'";
      return s;
    }
    case 4: {  // lone printable non-digit
      char c;
      do {
        c = static_cast<char>(32 + std::rand() % 95);
      } while (std::isdigit(uc(c)));
      return std::string(1, c);
    }
    case 5: {  // clustered on the 1e6 .0-suffix ceiling
      return toStr(999990L + std::rand() % 20) +
             (std::rand() % 2 ? ".0" : "");
    }
    default: {  // the pseudo-literals
      static const char* kPseudo[] = {"nan",  "nanf",  "+inf", "+inff",
                                      "-inf", "-inff", "inf",  "inff"};
      return kPseudo[std::rand() % 8];
    }
  }
}

static void fuzz_against_model(int iterations) {
  t::section("fuzz: constructed literals vs the model");

  for (int i = 0; i < iterations; ++i) {
    std::string lit = makeLiteral(i);
    double v = 0.0;
    std::string got = capture(lit);

    if (!modelAccepts(lit, &v)) {
      // The generator only ever emits valid literals, so this means the
      // generator drifted, not the implementation.
      t::ok(got == kAllImpossible,
            "generator emitted something the model rejects: " + show(lit));
      continue;
    }
    std::string want = modelOutput(v);
    t::ok(got == want, diff(lit, want, got));

    if (got.compare(0, 16, "char: impossible") == 0)
      t::hit("char: impossible");
    if (got.compare(0, 21, "char: Non displayable") == 0)
      t::hit("char: Non displayable");
    if (got.compare(0, 7, "char: '") == 0) t::hit("char: quoted");
    t::hit(got.find("\nint: impossible") != std::string::npos ? "int: impossible"
                                                              : "int: value");
    if (got.find(".0f") != std::string::npos) t::hit("float: .0 suffix");
    if (got.find("e+") != std::string::npos) t::hit("scientific notation");
    if (got.find("nan") != std::string::npos) t::hit("nan");
    if (got.find("inf") != std::string::npos) t::hit("inf");
  }
}

// ─── layer 3: chaos fuzz, invariants only ───────────────────────────────────

static std::string randomJunk() {
  // Alphabet sits on the decision boundaries, and includes bytes that break a
  // careless isprint/isdigit call (\x80, \xff are negative as plain char).
  static const char* kAlpha =
      "0123456789+-.fF'\"eExXnNaAiI \t\v\r\x7f\x80\xff";
  static const size_t kLen = std::strlen(kAlpha);
  std::string s;
  size_t n = static_cast<size_t>(std::rand() % 9);
  for (size_t i = 0; i < n; ++i) s += kAlpha[std::rand() % kLen];
  return s;
}

// convert() prints four lines; the cap is generous so a malformed run still
// gets counted rather than overflowing.
struct Lines {
  std::string l[8];
  int n;
};

static Lines splitLines(const std::string& s) {
  Lines v;
  v.n = 0;
  std::string cur;
  for (size_t i = 0; i < s.length(); ++i) {
    if (s[i] == '\n') {
      if (v.n < 8) v.l[v.n++] = cur;
      cur.clear();
    } else {
      cur += s[i];
    }
  }
  if (!cur.empty() && v.n < 8) v.l[v.n++] = cur;
  return v;
}

static void fuzz_invariants(int iterations) {
  t::section("fuzz: invariants on random bytes");

  for (int i = 0; i < iterations; ++i) {
    std::string in = randomJunk();
    std::string got = capture(in);
    Lines L = splitLines(got);

    if (L.n != 4) {
      t::ok(false, "expected 4 lines: " + diff(in, "4 lines", got));
      continue;
    }
    bool shape = L.l[0].compare(0, 6, "char: ") == 0 &&
                 L.l[1].compare(0, 5, "int: ") == 0 &&
                 L.l[2].compare(0, 7, "float: ") == 0 &&
                 L.l[3].compare(0, 8, "double: ") == 0;
    t::ok(shape, "line prefixes: " + diff(in, "char:/int:/float:/double:", got));
    if (!shape) continue;

    // the float line carries its suffix whenever it holds a value
    if (L.l[2] != "float: impossible")
      t::ok(L.l[2][L.l[2].length() - 1] == 'f', "float lacks 'f': " + show(L.l[2]));

    // the char line is one of exactly three shapes
    std::string c = L.l[0].substr(6);
    t::ok(c == "impossible" || c == "Non displayable" ||
              (c.length() == 3 && c[0] == '\'' && c[2] == '\''),
          "illegal char line: " + show(L.l[0]));

    // a quoted char must agree with the int line
    if (c.length() == 3 && c[0] == '\'') {
      std::string is = L.l[1].substr(5);
      if (is != "impossible")
        t::ok(std::atoi(is.c_str()) == static_cast<int>(uc(c[1])),
              "char/int disagree: " + show(L.l[0] + " / " + L.l[1]));
    }

    // "double: impossible" is all-or-nothing
    bool allImp = (got == kAllImpossible);
    t::ok((L.l[3] == "double: impossible") == allImp,
          "double: impossible must imply all four: " + show(got));

    // acceptance must agree with the independent model, and so must output
    double v = 0.0;
    bool modelYes = modelAccepts(in, &v);
    t::ok(modelYes == !allImp,
          std::string(modelYes ? "model accepts, converter rejected"
                               : "model rejects, converter accepted") +
              ": " + show(in));
    if (modelYes) t::ok(got == modelOutput(v), diff(in, modelOutput(v), got));

    // same input, same bytes
    t::ok(capture(in) == got, "not deterministic: " + show(in));

    t::hit(allImp ? "chaos: rejected" : "chaos: accepted");
  }
}

// ─── coverage ───────────────────────────────────────────────────────────────

// A fuzz run that only ever lands on the reject path proves nothing, so the
// branches we care about are asserted to have been reached.
static void check_coverage() {
  t::section("coverage: every branch was exercised");

  static const char* kMustHit[] = {
      "char: impossible", "char: Non displayable", "char: quoted",
      "int: impossible",  "int: value",            "float: .0 suffix",
      "scientific notation", "nan", "inf", "chaos: accepted", "chaos: rejected"};

  std::cout << std::endl;
  for (size_t i = 0; i < sizeof(kMustHit) / sizeof(kMustHit[0]); ++i) {
    long n = t::cover(kMustHit[i]);
    std::cout << "    " << std::left << std::setw(24) << kMustHit[i]
              << std::right << std::setw(9) << n << (n ? "" : "  NEVER HIT")
              << std::endl;
    t::ok(n > 0, std::string("branch never exercised: ") + kMustHit[i]);
  }
}

// ─── main ───────────────────────────────────────────────────────────────────

int main(int argc, char** argv) {
  unsigned seed = 20260811u;
  int iterations = 50000;

  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "long") == 0)
      iterations = 1000000;
    else
      seed = static_cast<unsigned>(std::strtoul(argv[i], NULL, 10));
  }
  std::srand(seed);

  table_subject_examples();
  table_plain_literals();
  table_char_literals();
  table_boundaries();
  table_pseudo_literals();
  table_rejects();
  fuzz_against_model(iterations);
  fuzz_invariants(iterations);
  check_coverage();

  std::ostringstream title;
  title << "ex00 ScalarConverter  (seed " << seed << ", " << iterations
        << " fuzz cases/layer)";
  return t::report(title.str().c_str());
}
