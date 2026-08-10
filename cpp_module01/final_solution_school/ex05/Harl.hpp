/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Harl.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:50:34 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:51:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX05_HARL_HPP_
#define CPP_MODULE01_EX05_HARL_HPP_

#include <iostream>
#include <string>

#define C_RESET "\033[0m"
#define C_RED "\033[31m"
#define C_GREEN "\033[32m"
#define C_YELLOW "\033[33m"
#define C_CYAN "\033[36m"
#define C_MAGENTA "\033[35m"
#define C_BOLD "\033[1m"
#define C_DIM "\033[2m"

/**
 * @brief A complaining character that dispatches log-level messages using a
 *        pointer-to-member-function table.
 *
 * @details
 * ### The problem this solves
 * A naïve implementation of @ref complain would look like this:
 * @code
 *   if      (level == "DEBUG")   debug();
 *   else if (level == "INFO")    info();
 *   else if (level == "WARNING") warning();
 *   else if (level == "ERROR")   error();
 * @endcode
 * The subject explicitly forbids this "forest of if/else".
 *
 * ### Solution — pointer-to-member-function dispatch table
 * Instead, @ref complain builds two parallel arrays of the same length:
 * - A `std::string[]` of level names.
 * - A `void (Harl::*[])()` array of pointers to the matching member functions.
 *
 * A single loop walks both arrays.  When a match is found at index `i`, the
 * corresponding function is invoked through the pointer and the method returns
 * immediately.  The result is O(N) dispatch with zero branching logic.
 *
 * #### Pointer-to-member-function syntax reminder
 * @code
 *   void (Harl::*fp)(void) = &Harl::debug; // declare & take address
 *   (this->*fp)();                          // invoke via 'this'
 * @endcode
 * The `->*` operator dereferences the pointer in the context of an object.
 * Without the `this->` prefix the compiler cannot resolve which object to
 * call the method on.
 *
 * #### Why the level methods are private
 * `debug`, `info`, `warning`, and `error` are implementation details —
 * callers are only supposed to use @ref complain.  Making them private enforces
 * that contract and prevents accidental direct calls from outside the class.
 */
class Harl {
 private:
  /**
   * @brief Prints a DEBUG-level complaint about extra bacon.
   * @note Called indirectly via the dispatch table in @ref complain.
   */
  void debug(void);

  /**
   * @brief Prints an INFO-level complaint about bacon quantity.
   * @note Called indirectly via the dispatch table in @ref complain.
   */
  void info(void);

  /**
   * @brief Prints a WARNING-level complaint about seniority and free bacon.
   * @note Called indirectly via the dispatch table in @ref complain.
   */
  void warning(void);

  /**
   * @brief Prints an ERROR-level demand to speak to the manager.
   * @note Called indirectly via the dispatch table in @ref complain.
   */
  void error(void);

 public:
  /** @brief Default constructor. */
  Harl();

  /** @brief Destructor. */
  ~Harl();

  /**
   * @brief Dispatches the complaint matching @p level using a
   *        pointer-to-member-function table — no if/else chain.
   *
   * @details
   * Valid values for @p level (case-sensitive):
   * | level     | method called   |
   * |-----------|-----------------|
   * | "DEBUG"   | @ref debug()    |
   * | "INFO"    | @ref info()     |
   * | "WARNING" | @ref warning()  |
   * | "ERROR"   | @ref error()    |
   *
   * If @p level does not match any entry the function returns silently
   * (no output, no error).
   *
   * @param level The log level string to dispatch.  Unknown values are
   *              ignored.
   */
  void complain(std::string level);
};

#endif  // CPP_MODULE01_EX05_HARL_HPP_
