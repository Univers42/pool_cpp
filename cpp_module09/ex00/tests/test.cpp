// Plain C++98 checks for BitcoinExchange: DB + input parsing, date/value
// validation, closest-lower-date lookup, exact subject error messages.
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../BitcoinExchange.hpp"

static int g_fails = 0;

static void expectEq(const std::string &got, const std::string &want,
                     const char *label) {
  if (got == want) return;
  ++g_fails;
  std::cout << "FAIL " << label << "\n--- want ---\n"
            << want << "--- got ----\n"
            << got << "------------" << std::endl;
}

static void writeFile(const char *path, const char *content) {
  std::ofstream f(path);
  f << content;
}

// Runs processInputFile with cout AND cerr captured into one transcript,
// so message ordering is checked too. Optionally reports the return value
// (false only when the input file could not be opened => exit status 1).
static std::string run(const BitcoinExchange &btc, const char *inputPath,
                       bool *opened = NULL) {
  std::ostringstream buf;
  std::streambuf *oldOut = std::cout.rdbuf(buf.rdbuf());
  std::streambuf *oldErr = std::cerr.rdbuf(buf.rdbuf());
  bool ok = btc.processInputFile(inputPath);
  std::cout.rdbuf(oldOut);
  std::cerr.rdbuf(oldErr);
  if (opened) *opened = ok;
  return buf.str();
}

int main() {
  // --- synthetic DB: exact match, closest-lower, too-early ---
  writeFile("test_db.csv",
            "date,exchange_rate\n"
            "2011-01-01,0.3\n"
            "2011-01-07,0.32\n"
            "2012-01-11,7.1\n");
  writeFile("test_input.txt",
            "date | value\n"
            "2011-01-01 | 1\n"       // exact date
            "2011-01-03 | 2\n"       // closest lower = 2011-01-01
            "2010-12-31 | 1\n"       // before whole DB
            "2012-01-11 | 1.2\n"     // float value
            "2001-42-42\n"           // no pipe / bad date
            "2023-02-29 | 1\n"       // not a leap year
            "2024-02-29 | 1\n"       // leap year, valid
            "2012-01-11 | -1\n"      // negative
            "2012-01-11 | 2147483648\n"  // > 1000
            "2012-01-11 | abc\n"     // not a number
            "2012-01-11 | 0x5\n"     // hex: strtod eats it, we must not
            "2012-01-11 | 1e2\n"     // scientific notation: rejected
            "2012-01-11 | -0\n"      // negative sign, even on zero
            "2012-01-11 | 1000\n");  // boundary: valid

  BitcoinExchange btc;
  if (!btc.loadDatabase("test_db.csv")) {
    std::cout << "FAIL loadDatabase(test_db.csv)" << std::endl;
    return 1;
  }
  bool opened = false;
  expectEq(run(btc, "test_input.txt", &opened),
           "2011-01-01 => 1 = 0.3\n"
           "2011-01-03 => 2 = 0.6\n"
           "Error: no rate for date => 2010-12-31\n"
           "2012-01-11 => 1.2 = 8.52\n"
           "Error: bad input => 2001-42-42\n"
           "Error: bad input => 2023-02-29\n"
           "2024-02-29 => 1 = 7.1\n"
           "Error: not a positive number.\n"
           "Error: too large a number.\n"
           "Error: bad input => abc\n"
           "Error: bad input => 0x5\n"
           "Error: bad input => 1e2\n"
           "Error: not a positive number.\n"
           "2012-01-11 => 1000 = 7100\n",
           "synthetic transcript");
  if (!opened) {
    std::cout << "FAIL processInputFile(good file) returned false"
              << std::endl;
    ++g_fails;
  }

  // --- missing files ---
  BitcoinExchange empty;
  if (empty.loadDatabase("no_such_db.csv")) {
    std::cout << "FAIL loadDatabase(missing) returned true" << std::endl;
    ++g_fails;
  }
  expectEq(run(btc, "no_such_input.txt", &opened),
           "Error: could not open file.\n", "missing input file");
  if (opened) {
    std::cout << "FAIL processInputFile(missing) returned true (main would "
                 "exit 0)"
              << std::endl;
    ++g_fails;
  }

  // --- verbatim subject example against the real data.csv ---
  BitcoinExchange real;
  if (!real.loadDatabase("data.csv")) {
    std::cout << "FAIL loadDatabase(data.csv)" << std::endl;
    return 1;
  }
  expectEq(run(real, "input.txt"),
           "2011-01-03 => 3 = 0.9\n"
           "2011-01-03 => 2 = 0.6\n"
           "2011-01-03 => 1 = 0.3\n"
           "2011-01-03 => 1.2 = 0.36\n"
           "2011-01-09 => 1 = 0.32\n"
           "Error: not a positive number.\n"
           "Error: bad input => 2001-42-42\n"
           "2012-01-11 => 1 = 7.1\n"
           "Error: too large a number.\n",
           "subject example");

  std::remove("test_db.csv");
  std::remove("test_input.txt");

  if (g_fails == 0) std::cout << "OK: all BitcoinExchange checks passed\n";
  return g_fails == 0 ? 0 : 1;
}
