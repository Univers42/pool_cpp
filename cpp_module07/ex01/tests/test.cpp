/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 11:49:33 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 11:49:40 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Plain C++98 tests for iter. Exits non-zero on any failure.

#include <iostream>
#include <string>

#include "../iter.hpp"

static int g_fail = 0;

static void check(bool ok, const char* what) {
  std::cout << (ok ? "ok:   " : "FAIL: ") << what << std::endl;
  if (!ok) g_fail = 1;
}

static void increment(int& n) { n++; }

static int g_sum = 0;
static void addTo(const int& n) { g_sum += n; }

static int g_calls = 0;
static void countCall(const int&) { ++g_calls; }

static std::string g_cat;
template <typename T>
static void append(const T& s) {
  g_cat += s;
}

int main(void) {
  int a[] = {1, 2, 3};
  ::iter(a, 3, increment);
  check(a[0] == 2 && a[1] == 3 && a[2] == 4,
        "iter mutates through a non-const ref callback");

  const int b[] = {10, 20, 30};
  ::iter(b, 3, addTo);
  check(g_sum == 60, "iter works on a const array (const ref callback)");

  const std::string s[] = {"4", "2"};
  ::iter(s, 2, append<std::string>);
  check(g_cat == "42", "instantiated function template as callback");

  g_sum = 0;
  ::iter(b, 0, addTo);
  check(g_sum == 0, "length 0 never calls the callback");

  // value-independent: catches off-by-one in either direction even when
  // adjacent memory happens to hold values the sum checks can't see
  g_calls = 0;
  ::iter(b, 3, countCall);
  check(g_calls == 3, "callback runs exactly length times");

  return g_fail;
}
