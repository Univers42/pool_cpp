/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 11:49:33 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/13 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Tests for iter, fixed cases plus a fuzz run.
//
//   make test               build and run
//   ./build/bin/test 1234   different fuzz seed
//   ./tests/mutants.sh      breaks the header, these tests must fail
//   ./tests/compile_fail.sh the code that must NOT compile
//
// These record the order elements were visited in, not a sum and not a count.
// My first version summed the values and a loop running length + 1 times got
// through it. A backwards loop gets through a count too.

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "../bigint.hpp"
#include "../iter.hpp"
#include "../vect2.hpp"


// Small harness so a failure says which section it came from. All static,
// constructors private so nobody can build one.
class Check {
 public:
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
      g_errors[g_errorCount++] = std::string(s.name) + " - " + detail;
  }

  static int report(const char* title) {
    int checks = 0, failed = 0;
    std::cout << "\n" << title << "\n" << std::endl;
    for (int i = 0; i < g_sectionCount; ++i) {
      const Section& s = g_sections[i];
      // 48 - len is unsigned, so a long name would wrap and throw.
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

 private:
  // Declared, never defined: an accidental Check c; fails at link time.
  Check();
  Check(const Check& other);
  Check& operator=(const Check& other);
  ~Check();

  struct Section {
    const char* name;
    int checks;
    int failed;
  };

  static const int kMaxSections = 16;
  static const int kMaxErrors = 12;

  static Section g_sections[kMaxSections];
  static int g_sectionCount;
  static std::string g_errors[kMaxErrors];
  static int g_errorCount;
};

Check::Section Check::g_sections[Check::kMaxSections];
int Check::g_sectionCount = 0;
std::string Check::g_errors[Check::kMaxErrors];
int Check::g_errorCount = 0;


// The visit log. Order is what separates a backwards loop, a skipped element
// and a doubled one from a correct run.
static const int kLogMax = 64;
static int g_log[kLogMax];
static int g_logCount = 0;
static int g_overflow = 0;

static void resetLog() {
  g_logCount = 0;
  g_overflow = 0;
}

static void record(const int& n) {
  if (g_logCount < kLogMax)
    g_log[g_logCount++] = n;
  else
    ++g_overflow;
}

static std::string logAsString() {
  std::ostringstream os;
  for (int i = 0; i < g_logCount; ++i) os << g_log[i] << (i + 1 < g_logCount ? "," : "");
  return os.str();
}

static void increment(int& n) { n++; }
static void printableNoop(const int&) {}

// By value: compiles fine and quietly cannot change anything.
static void byValue(int n) { n += 1000; }

// Returns a value; iter drops it.
static int returnsSomething(const int& n) { return n * 2; }

// Has state, to show F gets copied.
struct Tally {
  int calls;
  int sum;
  Tally() : calls(0), sum(0) {}
  void operator()(const int& n) {
    ++calls;
    sum += n;
  }
};

// The same, writing through borrowed pointers so copies share state.
struct TallyInto {
  int* calls;
  int* sum;
  TallyInto(int* c, int* s) : calls(c), sum(s) {}
  void operator()(const int& n) const {
    ++(*calls);
    *sum += n;
  }
};

// Counts its own copies: iter should make exactly one.
struct CountsCopies {
  static int copies;
  CountsCopies() {}
  CountsCopies(const CountsCopies&) { ++copies; }
  void operator()(const int&) const {}
};
int CountsCopies::copies = 0;

// operator() is a template: one object works for every element type.
struct AppendAny {
  std::string* out;
  explicit AppendAny(std::string* o) : out(o) {}
  template <typename T>
  void operator()(const T& elem) const {
    std::ostringstream os;
    os << elem;
    *out += os.str();
  }
};

// Instantiated function template as a callback, which the subject asks for.
static std::string g_cat;
template <typename T>
static void append(const T& s) {
  std::ostringstream os;
  os << s;
  g_cat += os.str();
}

// Throws partway through, to check iter just lets it out.
static void throwOnThree(const int& n) {
  record(n);
  if (n == 3) throw std::string("boom");
}

static void bumpThroughPointer(int* const& p) { ++(*p); }

struct Student {
  std::string name;
  int grade;
};
static void promote(Student& s) { s.grade++; }


static void test_subject_contract() {
  Check::section("the subject's three required callback shapes");

  int a[] = {1, 2, 3};
  ::iter(a, 3, increment);
  Check::ok(a[0] == 2 && a[1] == 3 && a[2] == 4,
        "non-const array + T& callback mutates in place");

  const int b[] = {10, 20, 30};
  resetLog();
  ::iter(b, 3, record);
  Check::ok(logAsString() == "10,20,30", "const array + const T& callback reads");

  const std::string s[] = {"4", "2"};
  g_cat = "";
  ::iter(s, 2, append<std::string>);
  Check::ok(g_cat == "42", "an instantiated function template is accepted");

  // T deduces the element type, so a const array of a class type works too.
  Student class42[2];
  class42[0].name = "ana";
  class42[0].grade = 1;
  class42[1].name = "ben";
  class42[1].grade = 2;
  ::iter(class42, 2, promote);
  Check::ok(class42[0].grade == 2 && class42[1].grade == 3,
        "user-defined element type mutates through T&");
}


static void test_visits_every_element_in_order() {
  Check::section("f runs on 0..length-1, in order, exactly once each");

  const int a[] = {7, 8, 9, 10};

  resetLog();
  ::iter(a, 4, record);
  Check::ok(g_logCount == 4, "exactly 4 invocations for length 4");
  Check::ok(logAsString() == "7,8,9,10", "ascending index order, no repeats");

  // A backwards loop has the same count and the same sum as a correct one.
  Check::ok(logAsString() != "10,9,8,7", "not iterated backwards");

  resetLog();
  ::iter(a, 1, record);
  Check::ok(logAsString() == "7", "length 1 visits only the first element");

  resetLog();
  ::iter(a, 0, record);
  Check::ok(g_logCount == 0, "length 0 never calls the callback");

  // iter takes a pointer, so any sub-range is expressible.
  resetLog();
  ::iter(a + 1, 2, record);
  Check::ok(logAsString() == "8,9", "a sub-range visits exactly that sub-range");

  resetLog();
  ::iter(a + 3, 1, record);
  Check::ok(logAsString() == "10", "the last element alone");
}

static void test_stays_inside_the_range() {
  Check::section("iter never reads or writes outside [0, length)");

  // Guard slots either side, so an off-by-one lands on one of them whatever
  // the values are.
  const int kGuard = -777;
  int buf[12];
  for (int i = 0; i < 12; ++i) buf[i] = kGuard;
  for (int i = 4; i < 8; ++i) buf[i] = i;

  resetLog();
  ::iter(buf + 4, 4, record);
  Check::ok(logAsString() == "4,5,6,7", "read exactly the four middle elements");
  Check::ok(g_log[0] != kGuard, "never read the guard before the range");

  // Now a mutating pass: the guards must still be untouched afterwards.
  ::iter(buf + 4, 4, increment);
  bool guardsIntact = true;
  for (int i = 0; i < 4; ++i)
    if (buf[i] != kGuard) guardsIntact = false;
  for (int i = 8; i < 12; ++i)
    if (buf[i] != kGuard) guardsIntact = false;
  Check::ok(guardsIntact, "the eight guard slots were not written");
  Check::ok(buf[4] == 5 && buf[7] == 8, "and the four real slots were");

  // Zero length on a guarded buffer must touch nothing at all.
  ::iter(buf + 4, 0, increment);
  Check::ok(buf[4] == 5, "length 0 wrote nothing");
}


static void test_callable_shapes() {
  Check::section("every callable shape F is allowed to deduce");

  int a[] = {1, 2, 3};

  // plain function name, decays to a pointer
  resetLog();
  ::iter(a, 3, record);
  Check::ok(g_logCount == 3, "function name decays to a function pointer");

  // a function pointer held in a variable
  void (*fp)(const int&) = record;
  resetLog();
  ::iter(a, 3, fp);
  Check::ok(g_logCount == 3, "a function-pointer variable works identically");

  // a functor
  int calls = 0, sum = 0;
  ::iter(a, 3, TallyInto(&calls, &sum));
  Check::ok(calls == 3 && sum == 6, "a functor object works");

  // a functor whose operator() is a template, over two element types
  std::string out;
  ::iter(a, 3, AppendAny(&out));
  const double d[] = {1.5};
  ::iter(d, 1, AppendAny(&out));
  Check::ok(out == "1231.5", "one templated functor served int and double");

  // a callback returning non-void: the value is discarded, not an error
  ::iter(a, 3, returnsSomething);
  Check::ok(a[0] == 1, "a returning callback is accepted and its result dropped");

  // Looks identical to the mutating case at the call site, but cannot write.
  ::iter(a, 3, byValue);
  Check::ok(a[0] == 1 && a[1] == 2 && a[2] == 3,
        "a by-value callback gets a copy and cannot reach the array");

  // a no-op still has to be invoked the right number of times
  ::iter(a, 3, printableNoop);
  Check::ok(true, "a callback that does nothing is still a valid callback");
}

static void test_functor_is_copied() {
  Check::section("F is a by-value parameter, so functor state stays behind");

  const int a[] = {5, 10, 15};

  Tally counter;
  ::iter(a, 3, counter);
  Check::ok(counter.calls == 0, "the caller's functor was never touched");
  Check::ok(counter.sum == 0, "its accumulator is still zero");

  // ...and the work really did happen, on the copy.
  int calls = 0, sum = 0;
  ::iter(a, 3, TallyInto(&calls, &sum));
  Check::ok(calls == 3 && sum == 30, "a functor holding pointers sees every call");

  // One copy, argument into parameter. Copying per element would show more.
  CountsCopies::copies = 0;
  CountsCopies probe;
  ::iter(a, 3, probe);
  Check::ok(CountsCopies::copies == 1,
        "F is copied once per call, not once per element");
}


static void test_constness() {
  Check::section("const-ness rides on T (the rest is in compile_fail.sh)");

  const int frozen[] = {1, 2, 3};
  resetLog();
  ::iter(frozen, 3, record);
  Check::ok(logAsString() == "1,2,3", "a const array binds to a const T& callback");

  const std::string words[] = {"a", "b"};
  g_cat = "";
  ::iter(words, 2, append<std::string>);
  Check::ok(g_cat == "ab", "const std::string[] deduces T = const std::string");

  // Adding const is fine; it is taking it away the compiler refuses.
  int warm[] = {4, 5};
  resetLog();
  ::iter(warm, 2, record);
  Check::ok(logAsString() == "4,5", "non-const array + const T& callback is fine");
  Check::ok(warm[0] == 4 && warm[1] == 5, "and it left the array alone");
}


static void test_edges() {
  Check::section("edges: null pointer, zero length, single element");

  int calls = 0, sum = 0;
  int* nothing = NULL;
  ::iter(nothing, 0, TallyInto(&calls, &sum));
  Check::ok(calls == 0, "NULL with length 0 never dereferences");

  const int* frozenNothing = NULL;
  resetLog();
  ::iter(frozenNothing, 0, record);
  Check::ok(g_logCount == 0, "const NULL with length 0 is equally safe");

  const int one[] = {99};
  resetLog();
  ::iter(one, 1, record);
  Check::ok(logAsString() == "99", "a single-element array");

  // Called twice in a row: no state is carried between calls.
  resetLog();
  ::iter(one, 1, record);
  ::iter(one, 1, record);
  Check::ok(logAsString() == "99,99", "iter holds no state between calls");
}


static void test_fuzz(int iterations) {
  Check::section("fuzz: random lengths and contents, sequence checked exactly");

  bool allGood = true;
  std::string firstFailure;
  bool sawEmpty = false, sawFull = false;

  for (int i = 0; i < iterations; ++i) {
    const int kCap = 16;
    const int kGuardSlots = 4;
    int buf[kCap + 2 * kGuardSlots];
    const int kGuard = -999999;

    const int len = std::rand() % (kCap + 1);
    if (len == 0) sawEmpty = true;
    if (len == kCap) sawFull = true;

    for (int k = 0; k < kCap + 2 * kGuardSlots; ++k) buf[k] = kGuard;
    int* data = buf + kGuardSlots;
    for (int k = 0; k < len; ++k) data[k] = (std::rand() % 2001) - 1000;

    // Expected visit sequence, built before the call.
    std::ostringstream expected;
    for (int k = 0; k < len; ++k)
      expected << data[k] << (k + 1 < len ? "," : "");

    resetLog();
    ::iter(data, static_cast<size_t>(len), record);

    bool good = true;
    if (g_overflow != 0) good = false;
    if (g_logCount != len) good = false;
    if (logAsString() != expected.str()) good = false;

    // Guards on both sides must be untouched by a mutating pass.
    int before[kCap];
    for (int k = 0; k < len; ++k) before[k] = data[k];
    ::iter(data, static_cast<size_t>(len), increment);
    for (int k = 0; k < len; ++k)
      if (data[k] != before[k] + 1) good = false;
    for (int k = 0; k < kGuardSlots; ++k)
      if (buf[k] != kGuard) good = false;
    for (int k = kGuardSlots + len; k < kCap + 2 * kGuardSlots; ++k)
      if (buf[k] != kGuard) good = false;

    if (!good && allGood) {
      std::ostringstream why;
      why << "len=" << len << " expected=[" << expected.str() << "] got=["
          << logAsString() << "]";
      firstFailure = why.str();
      allGood = false;
    }
  }

  Check::ok(allGood, "every fuzz round matched its expected sequence" +
                     (firstFailure.empty() ? "" : " - first bad: " +
                                                      firstFailure));
  Check::ok(sawEmpty, "the run actually produced a length of 0");
  Check::ok(sawFull, "the run actually produced a full-capacity array");
}

static void test_more_edges() {
  Check::section("edges: throwing callback, pointers, 2D rows, nesting");

  // iter holds nothing and allocates nothing, so an exception from the
  // callback just travels straight out. The elements before it were still
  // visited.
  const int a[] = {1, 2, 3, 4, 5};
  resetLog();
  bool threw = false;
  try {
    ::iter(a, 5, throwOnThree);
  } catch (const std::string&) {
    threw = true;
  }
  Check::ok(threw, "an exception from the callback escapes iter");
  Check::ok(logAsString() == "1,2,3", "and the earlier elements were visited");

  // An array of pointers: T deduces int*, and the callback can write through
  // them even though the pointers themselves are const.
  int one = 10, two = 20, three = 30;
  int* pointers[] = {&one, &two, &three};
  ::iter(pointers, 3, bumpThroughPointer);
  Check::ok(one == 11 && two == 21 && three == 31,
            "iter over an array of pointers writes through them");

  // One row of a 2D array is just an int[4].
  int grid[3][4];
  for (int r = 0; r < 3; ++r)
    for (int c = 0; c < 4; ++c) grid[r][c] = r * 10 + c;
  resetLog();
  ::iter(grid[1], 4, record);
  Check::ok(logAsString() == "10,11,12,13", "iter over one row of a 2D array");
  ::iter(grid[1], 4, increment);
  Check::ok(grid[0][0] == 0 && grid[2][0] == 20, "the other rows are untouched");

  // The outer array of a 2D array is an array of int[4], so a callback taking
  // that type works too.
  resetLog();
  for (int r = 0; r < 3; ++r) ::iter(grid[r], 4, record);
  Check::ok(g_logCount == 12, "walking every row visits all 12 elements");

  // A single-element const array of a class type.
  const std::string only[] = {"solo"};
  g_cat = "";
  ::iter(only, 1, append<std::string>);
  Check::ok(g_cat == "solo", "one const std::string");

  // A big one, to be sure nothing is quietly capped.
  const int kBig = 5000;
  int* heap = new int[kBig];
  for (int i = 0; i < kBig; ++i) heap[i] = 1;
  int calls = 0, sum = 0;
  ::iter(heap, kBig, TallyInto(&calls, &sum));
  Check::ok(calls == kBig && sum == kBig, "5000 elements, all visited once");
  ::iter(heap, kBig, increment);
  Check::ok(heap[0] == 2 && heap[kBig - 1] == 2, "and all mutated");
  delete[] heap;

  // length 1 on the very last element of a range, a classic off-by-one spot.
  resetLog();
  ::iter(a + 4, 1, record);
  Check::ok(logAsString() == "5", "the last element on its own");
}

// ---------------------------------------------------------------------------
// The complex element types. bigint is a fresh implementation, so its own
// arithmetic is checked first: a wrong answer coming out of iter looks exactly
// the same whether iter or the element type produced it, and only one of those
// two suspects is the subject of this exercise.

struct HornerInto {
  bigint* acc;
  explicit HornerInto(bigint* a) : acc(a) {}
  void operator()(const int& digit) const {
    *acc <<= bigint(1);
    *acc += bigint(digit);
  }
};

struct FibInto {
  bigint* prev;
  bigint* cur;
  FibInto(bigint* p, bigint* c) : prev(p), cur(c) {}
  void operator()(bigint& slot) const {
    slot = *prev;
    const bigint next = *prev + *cur;
    *prev = *cur;
    *cur = next;
  }
};

struct SumInto {
  bigint* total;
  explicit SumInto(bigint* t) : total(t) {}
  void operator()(const bigint& n) const { *total += n; }
};

struct SumByValue {
  bigint total;
  void operator()(const bigint& n) { total += n; }
};

static void bumpVect(vect2& v) { ++v; }

static vect2 g_cellTotal;
static void addCell(const vect2& v) { g_cellTotal += v; }

// Callbacks taking a reference to an ARRAY, so the inner length rides along
// in the type and the nested iter does not have to be told it.
template <typename T, size_t N>
static void addRow(const T (&row)[N]) {
  ::iter(row, N, addCell);
}

template <size_t N>
static void bumpRow(vect2 (&row)[N]) {
  ::iter(row, N, bumpVect);
}

static void test_bigint_itself() {
  Check::section("bigint's own arithmetic");

  Check::ok(bigint().str() == "0", "default constructs zero");
  Check::ok(bigint(0).str() == "0", "int zero");
  Check::ok(bigint(-7).str() == "0", "a negative int clamps to zero");
  Check::ok(bigint(1234).str() == "1234", "int digits, most significant first");
  Check::ok(bigint("").str() == "0", "the empty string normalises");
  Check::ok(bigint("000").str() == "0", "a run of zeros normalises");
  Check::ok(bigint("00042").str() == "42", "leading zeros are stripped");

  Check::ok((bigint() + bigint()).str() == "0", "0 + 0");
  Check::ok((bigint(999) + bigint(1)).str() == "1000", "carry across all 9s");
  Check::ok((bigint(19) + bigint(1)).str() == "20", "carry stops part way");
  Check::ok((bigint(1) + bigint(99)).str() == "100", "shorter left operand");
  Check::ok((bigint(5) + bigint(123)).str() == "128",
            "the padding is not a digit");

  // Past every builtin, and a value that is easy to look up.
  bigint p(1);
  for (int i = 0; i < 64; ++i) p += p;
  Check::ok(p.str() == "18446744073709551616", "2^64 by repeated doubling");

  Check::ok((bigint(42) << bigint(3)).str() == "42000", "<< is x10^k");
  Check::ok((bigint(0) << bigint(3)).str() == "0", "zero does not grow");
  Check::ok((bigint(42000) >> bigint(3)).str() == "42", ">> undoes it");
  Check::ok((bigint(42) >> bigint(9)).str() == "0", "shifting past the end");

  bigint c(9);
  const bigint post = c++;
  Check::ok(post.str() == "9" && c.str() == "10",
            "c++ hands back the old value");
  Check::ok((++c).str() == "11", "++c hands back the new one");

  Check::ok(bigint(9) < bigint(10), "length is compared before content");
  Check::ok(!(bigint(10) < bigint(9)), "and it is not symmetric");
  Check::ok(bigint(77) == bigint(77) && bigint(19) != bigint(20), "== and !=");
  Check::ok(bigint(20) > bigint(19), "operator>");
  Check::ok(bigint(1) <= bigint(1) && bigint(1) >= bigint(1),
            "<= and >= on a tie");

  // It owns a std::string, so the usual two questions.
  bigint a(5);
  bigint& alias = a;
  a = alias;
  Check::ok(a.str() == "5", "self-assignment leaves it alone");
  bigint b(a);
  ++b;
  Check::ok(a.str() == "5" && b.str() == "6", "a copy is independent");
}

static void test_vect2_itself() {
  Check::section("vect2's own operators");

  const vect2 a(1, 2), b(3, 4);
  Check::ok((a + b) == vect2(4, 6), "operator+");
  Check::ok((b - a) == vect2(2, 2), "operator-");
  Check::ok((-a) == vect2(-1, -2), "unary minus");
  Check::ok((a * 3) == vect2(3, 6) && (3 * a) == vect2(3, 6),
            "scalar on either side");
  Check::ok(!(a != vect2(1, 2)), "operator!=");

  vect2 c(a);
  ++c;
  Check::ok(c == vect2(2, 3), "pre-increment");
  const vect2 old = c++;
  Check::ok(old == vect2(2, 3) && c == vect2(3, 4),
            "c++ hands back the old value");
  --c;
  Check::ok(c == vect2(2, 3), "pre-decrement");
  c += vect2(1, 1);
  c -= vect2(0, 1);
  c *= 2;
  Check::ok(c == vect2(6, 6), "+=, -=, *=");

  c[0] = 9;
  Check::ok(c[0] == 9, "operator[] yields int& on a mutable vect2");
  const vect2 frozen(7, 8);
  Check::ok(frozen[0] == 7 && frozen[1] == 8, "and int on a const one");

  std::ostringstream os;
  os << vect2(5, 6);
  Check::ok(os.str() == "{5, 6}", "operator<<");
}

static void test_iter_on_complex_types() {
  Check::section("iter driving vect2 and bigint");

  // Horner is the one callback here whose answer depends on the visit ORDER,
  // which is exactly what a count or a sum cannot check.
  const int digits[] = {1, 2, 3, 4, 5};
  bigint fwd;
  ::iter(digits, 5, HornerInto(&fwd));
  Check::ok(fwd.str() == "12345", "an int[] in, a bigint out");

  int backwards[5];
  for (int i = 0; i < 5; ++i) backwards[i] = digits[4 - i];
  bigint rev;
  ::iter(backwards, 5, HornerInto(&rev));
  Check::ok(rev.str() == "54321", "reversing the input reverses the answer");

  // 2^127 - 1: 39 digits, so nothing is quietly capped on the way through.
  const int mersenne[] = {1, 7, 0, 1, 4, 1, 1, 8, 3, 4, 6, 0, 4,
                          6, 9, 2, 3, 1, 7, 3, 1, 6, 8, 7, 3, 0,
                          3, 7, 1, 5, 8, 8, 4, 1, 0, 5, 7, 2, 7};
  bigint big;
  ::iter(mersenne, 39, HornerInto(&big));
  Check::ok(big.str() == "170141183460469231731687303715884105727",
            "39 digits survive the round trip");
  Check::ok(big.digits() == 39, "and the length agrees");

  // The array as the output, then as the input. The known values are what do
  // the work here: the identity below cancels out of any mutation that hits
  // both passes the same way, which a reversed iter and a broken carry both
  // do. It stays because two passes agreeing is still worth something, but it
  // is not what catches anything.
  bigint fib[30];
  bigint prev(0), cur(1);
  ::iter(fib, 30, FibInto(&prev, &cur));
  Check::ok(fib[0].str() == "0" && fib[1].str() == "1" && fib[10].str() == "55",
            "iter filled a bigint[30] in order");
  Check::ok(fib[29].str() == "514229", "including the far end");

  bigint total;
  ::iter(fib, 30, SumInto(&total));
  Check::ok(total + bigint(1) == cur, "sum F(0..29) + 1 == F(31)");

  // F is a by-value parameter, and on bigint that copy is a real allocation.
  SumByValue collector;
  ::iter(fib, 30, collector);
  Check::ok(collector.total == bigint(), "a by-value accumulator sees nothing");

  const size_t before = bigint::copies();
  ::iter(fib, 30, collector);
  Check::ok(bigint::copies() - before == 1,
            "F is copied once per call, not once per element");

  // vect2 through a plain mutating callback.
  vect2 path[] = {vect2(0, 0), vect2(1, 1), vect2(2, 2)};
  ::iter(path, 3, bumpVect);
  Check::ok(path[0] == vect2(1, 1) && path[2] == vect2(3, 3),
            "mutating a class element through T&");

  // An array of arrays: T is vect2[3], and the callback runs iter again.
  vect2 grid[2][3] = {{vect2(1, 1), vect2(2, 2), vect2(3, 3)},
                      {vect2(4, 4), vect2(5, 5), vect2(6, 6)}};
  g_cellTotal = vect2();
  ::iter(grid, 2, addRow<vect2, 3>);
  Check::ok(g_cellTotal == vect2(21, 21), "nested iter reached all 6 cells");

  ::iter(grid, 2, bumpRow<3>);
  Check::ok(grid[0][0] == vect2(2, 2) && grid[1][2] == vect2(7, 7),
            "and the inner iter can mutate too");

  // The same, const: T becomes const vect2[3] and the row binds read-only.
  const vect2 sealed[2][3] = {{vect2(1, 0), vect2(1, 0), vect2(1, 0)},
                              {vect2(1, 0), vect2(1, 0), vect2(1, 0)}};
  g_cellTotal = vect2();
  ::iter(sealed, 2, addRow<vect2, 3>);
  Check::ok(g_cellTotal == vect2(6, 0), "a const 2D array reads fine");

  // Slices work here as well, since iter only ever had a pointer.
  bigint tail;
  ::iter(fib + 27, 3, SumInto(&tail));
  Check::ok(tail == fib[27] + fib[28] + fib[29], "a slice of a bigint array");
}

int main(int argc, char** argv) {
  unsigned int seed = 42;
  if (argc > 1) seed = static_cast<unsigned int>(std::atoi(argv[1]));
  std::srand(seed);
  const int iterations = 3000;

  test_subject_contract();
  test_visits_every_element_in_order();
  test_stays_inside_the_range();
  test_callable_shapes();
  test_functor_is_copied();
  test_constness();
  test_edges();
  test_more_edges();
  test_bigint_itself();
  test_vect2_itself();
  test_iter_on_complex_types();
  test_fuzz(iterations);

  std::ostringstream title;
  title << "ex01 iter  (seed " << seed << ", " << iterations
        << " fuzz iterations)";
  return Check::report(title.str().c_str());
}
