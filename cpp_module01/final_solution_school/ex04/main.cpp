/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:22:11 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:56:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Replacer.hpp"

/**
 * @brief Entry point — validates arguments and delegates to Replacer.
 *
 * @details
 * Expected usage:
 * @code
 *   ./ex04 <filename> <s1> <s2>
 * @endcode
 * - @p filename  Path to the file to process.
 * - @p s1        String to search for (must not be empty — Replacer enforces this).
 * - @p s2        Replacement string (may be empty, which deletes all occurrences).
 *
 * The program creates `<filename>.replace` containing the modified content.
 * The original file is **never** modified.
 *
 * @param argc Number of command-line arguments (must be exactly 4).
 * @param argv Argument vector: argv[1]=filename, argv[2]=s1, argv[3]=s2.
 * @return 0 on success, 1 on wrong argument count or processing failure.
 */
int main(int argc, char** argv) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <filename> <s1> <s2>\n";
    return 1;
  }

  Replacer replacer(argv[1], argv[2], argv[3]);

  if (!replacer.process()) {
    return 1;
  }

  return 0;
}
