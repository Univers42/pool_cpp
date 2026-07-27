/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replacer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:30:16 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:50:19 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Replacer.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Replacer::Replacer(const std::string& filename, const std::string& s1,
                   const std::string& s2)
    : inFile(filename), outFile(filename + ".replace"), s1(s1), s2(s2) {}

Replacer::~Replacer() {}

bool Replacer::process() {
  typedef std::string::size_type size_type;

  if (this->s1.empty()) {
    std::cerr << "Error: The search string cannot be empty.\n";
    return false;
  }

  std::ifstream ifs(this->inFile.c_str(), std::ios::in | std::ios::binary);
  if (!ifs.is_open()) {
    std::cerr << "Error: Cannot open input file '" << this->inFile << "'.\n";
    return false;
  }

  // Bulk-load file into memory; binary mode + rdbuf keeps NUL bytes intact.
  std::ostringstream oss;
  oss << ifs.rdbuf();
  if (ifs.bad()) {
    std::cerr << "Error: Read from '" << this->inFile << "' failed.\n";
    return false;
  }
  std::string text = oss.str();

  std::ofstream ofs(this->outFile.c_str(), std::ios::out | std::ios::binary);
  if (!ofs.is_open()) {
    std::cerr << "Error: Cannot create output file '" << this->outFile
              << "'.\n";
    return false;
  }

  // ponytail: std::string::find + advance replaced a hand-rolled KMP loop —
  // measured 2.4x-11.5x FASTER here (find rides SIMD memchr/memcmp) and a
  // fifth of the code. The KMP survives as the independent reference oracle
  // in tests/kmp_ref.hpp; tests/bench.cpp re-measures both on every run.
  // Semantics: every occurrence, left to right, non-overlapping (sed-style).
  const size_type N = text.length();
  size_type pos = 0;
  size_type hit;
  while ((hit = text.find(this->s1, pos)) != std::string::npos) {
    ofs.write(text.c_str() + pos, static_cast<std::streamsize>(hit - pos));
    ofs.write(this->s2.c_str(),
              static_cast<std::streamsize>(this->s2.length()));
    pos = hit + this->s1.length();
  }
  ofs.write(text.c_str() + pos, static_cast<std::streamsize>(N - pos));

  ofs.flush();
  if (!ofs.good()) {
    std::cerr << "Error: Write to '" << this->outFile << "' failed.\n";
    return false;
  }

  return true;
}
