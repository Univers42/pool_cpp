/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Harl.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 05:44:54 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 20:51:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE01_EX06_HARL_HPP_
#define CPP_MODULE01_EX06_HARL_HPP_

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
 * @brief A complaining character whose level methods are dispatched by
 *        `main` through a `switch` fallthrough, and internally by a
 *        pointer-to-member-function table inside @ref complain.
 *
 * @details
 * ### ex06 vs ex05 — what changed and why
 * In ex05, @ref complain was the sole dispatcher.  In ex06 the **main
 * dispatch** moves to `main`, which uses a `switch` with intentional
 * fallthrough to print every level from the requested severity upward.
 * @ref complain is still used by each `case` to invoke a single level,
 * and internally uses the same **pointer-to-member table** as ex05 —
 * no if/else chain, no map, no heap allocation.
 *
 * ### Why the level methods are private
 * `debug`, `info`, `warning`, and `error` are implementation details.
 * The public contract is `complain(level)` only.  Keeping them private
 * ensures callers cannot bypass the dispatch logic or call individual levels
 * out of order.
 *
 * @see main() for the switch-fallthrough dispatch strategy.
 */
class Harl {
 public:
  /** @brief Default constructor. */
  Harl();

  /** @brief Destructor. */
  ~Harl();

  /**
   * @brief Calls the private method that matches @p level exactly, using a
   *        pointer-to-member-function dispatch table (no if/else chain).
   *
   * @details
   * Two parallel stack-allocated arrays (function pointers + label strings)
   * are scanned in a single loop.  On match the method is invoked through
   * `(this->*methods[i])()` and the function returns immediately.
   * Unknown values cause a silent no-op.
   *
   * @param level Case-sensitive level string: "DEBUG", "INFO", "WARNING",
   *              or "ERROR".  Unknown values produce no output.
   */
  void complain(std::string level);

 private:
  /** @brief Prints the DEBUG complaint. Called by complain("DEBUG"). */
  void debug(void);

  /** @brief Prints the INFO complaint. Called by complain("INFO"). */
  void info(void);

  /** @brief Prints the WARNING complaint. Called by complain("WARNING"). */
  void warning(void);

  /** @brief Prints the ERROR complaint. Called by complain("ERROR"). */
  void error(void);
};

#endif  // CPP_MODULE01_EX06_HARL_HPP_
