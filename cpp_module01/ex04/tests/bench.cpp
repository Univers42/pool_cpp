/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bench.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Benchmark: the shipped Replacer (std::string::find) vs the original KMP
// (tests/kmp_ref.hpp) on three workloads, including the pathological
// near-miss case that theoretically favors KMP. Also byte-compares both
// outputs, so it doubles as a large-scale differential test. Exits non-zero
// only on a mismatch. Numbers justify the implementation choice on every run.

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Replacer.hpp"
#include "kmp_ref.hpp"

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

static unsigned long countMatches(const std::string& text,
                                  const std::string& s1) {
  unsigned long n = 0;
  std::string::size_type pos = 0;
  std::string::size_type hit;
  while ((hit = text.find(s1, pos)) != std::string::npos) {
    ++n;
    pos = hit + s1.length();
  }
  return n;
}

static double msSince(std::clock_t start) {
  return 1000.0 * static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC;
}

static int g_fails = 0;

// Times the full pipeline for both implementations: read file, transform,
// write result. That is what the program actually does.
static void bench(const char* name, const std::string& text,
                  const std::string& s1, const std::string& s2) {
  const char* in = "build/bench_in.txt";
  if (!writeFile(in, text)) {
    std::cerr << "[SETUP-FAIL] " << name << "\n";
    ++g_fails;
    return;
  }

  std::clock_t t0 = std::clock();
  Replacer r(in, s1, s2);
  bool ok = r.process();
  double replacerMs = msSince(t0);

  t0 = std::clock();
  std::string loaded;
  readFile(in, loaded);
  std::string expected = kmpReplace(loaded, s1, s2);
  writeFile("build/bench_kmp.out", expected);
  double kmpMs = msSince(t0);

  std::string got;
  if (!ok || !readFile(std::string(in) + ".replace", got) || got != expected) {
    std::cerr << "[FAIL] " << name << ": Replacer output != KMP reference\n";
    ++g_fails;
    return;
  }

  double mb = static_cast<double>(text.length()) / (1024.0 * 1024.0);
  std::cout.setf(std::ios::fixed);
  std::cout.precision(1);
  std::cout << "  " << name << ": " << mb << " MB, "
            << countMatches(loaded, s1) << " matches | Replacer(find) "
            << replacerMs << " ms | KMP ref " << kmpMs << " ms\n";
}

int main() {
  std::cout << "ex04 benchmark (read + replace + write, lower is better)\n";

  // Workload 1: prose, frequent matches.
  std::string prose;
  prose.reserve(16u * 1024u * 1024u);
  while (prose.length() < 16u * 1024u * 1024u)
    prose += "the quick brown fox jumps over the lazy dog. ";
  bench("prose/frequent ", prose, "fox", "wolf");

  // Workload 2: prose, pattern absent.
  bench("prose/absent   ", prose, "zebra", "unicorn");

  // Workload 3: pathological near-miss runs — worst case for naive search.
  bench("pathological   ", std::string(2u * 1024u * 1024u, 'a'),
        std::string(500, 'a') + "b", "x");

  if (g_fails) return 1;
  std::cout << "ex04 bench: outputs verified identical\n";
  return 0;
}
