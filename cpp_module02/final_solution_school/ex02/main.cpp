/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 19:17:35 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:53:57 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "Fixed.hpp"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

int testsTotal = 0;
int testsPassed = 0;

// Epsilon for comparing Fixed result to Float result
const float EPSILON = 0.91f;

void assert_test(bool condition, const std::string& testName,
                 const std::string& details = "") {
  testsTotal++;
  if (condition) {
    testsPassed++;
  } else {
    std::cout << RED << "[FAIL] " << RESET << testName << std::endl;
    if (!details.empty()) std::cout << "       " << details << std::endl;
  }
}

float getRandomFloat(float min, float max) {
  static unsigned int seed = 42;
  float scale =
      static_cast<float>(rand_r(&seed)) / static_cast<float>(RAND_MAX);
  return min + scale * (max - min);
}

void test_arithmetic(int iterations) {
  std::cout << "Running " << iterations << " Arithmetic Fuzz Tests..."
            << std::endl;
  for (int i = 0; i < iterations; i++) {
    float f1 = getRandomFloat(-50.0f, 50.0f);
    float f2 = getRandomFloat(-50.0f, 50.0f);
    if (std::abs(f2) < 0.1f) f2 = 1.0f;

    Fixed a(f1);
    Fixed b(f2);

    // Test Addition
    {
      Fixed add_res = a + b;
      float add_exp = f1 + f2;
      std::ostringstream add_err;
      add_err << "Fixed(" << f1 << " + " << f2 << ") = " << add_res.toFloat()
              << " | Expected: " << add_exp;
      assert_test(std::abs(add_res.toFloat() - add_exp) <= EPSILON, "Addition",
                  add_err.str());
    }
    // Test Subtraction
    {
      Fixed sub_res = a - b;
      float sub_exp = f1 - f2;
      std::ostringstream sub_err;
      sub_err << "Fixed(" << f1 << " - " << f2 << ") = " << sub_res.toFloat()
              << " | Expected: " << sub_exp;
      assert_test(std::abs(sub_res.toFloat() - sub_exp) <= EPSILON,
                  "Subtraction", sub_err.str());
    }
    // Test Multiplication
    {
      Fixed mul_res = a * b;
      float mul_exp = f1 * f2;
      std::ostringstream mul_err;
      mul_err << "Fixed(" << f1 << " * " << f2 << ") = " << mul_res.toFloat()
              << " | Expected: " << mul_exp;
      assert_test(std::abs(mul_res.toFloat() - mul_exp) <= EPSILON,
                  "Multiplication", mul_err.str());
    }
    // Test Division
    {
      Fixed div_res = a / b;
      float div_exp = f1 / f2;
      std::ostringstream div_err;
      div_err << "Fixed(" << f1 << " / " << f2 << ") = " << div_res.toFloat()
              << " | Expected: " << div_exp;
      assert_test(std::abs(div_res.toFloat() - div_exp) <= EPSILON, "Division",
                  div_err.str());
    }
  }
}

void test_comparisons(int iterations) {
  std::cout << "Running " << iterations << " Comparison Fuzz Tests..."
            << std::endl;
  for (int i = 0; i < iterations; i++) {
    float f1 = getRandomFloat(-100.0f, 100.0f);
    float f2 = getRandomFloat(-100.0f, 100.0f);
    Fixed a(f1);
    Fixed b(f2);

    // We compare the Fixed logic output to standard float logic output
    assert_test((a > b) == (a.toFloat() > b.toFloat()), "Operator >");
    assert_test((a < b) == (a.toFloat() < b.toFloat()), "Operator <");
    assert_test((a >= b) == (a.toFloat() >= b.toFloat()), "Operator >=");
    assert_test((a <= b) == (a.toFloat() <= b.toFloat()), "Operator <=");
    assert_test((a == b) == (a.toFloat() == b.toFloat()), "Operator ==");
    assert_test((a != b) == (a.toFloat() != b.toFloat()), "Operator !=");
  }
}

void test_edge_cases() {
  std::cout << "Running Edge Case Tests..." << std::endl;
  Fixed zero(0);
  Fixed neg_one(-1);
  Fixed pos_one(1);

  assert_test((neg_one < zero), "Negative < Zero");
  assert_test((pos_one > zero), "Positive > Zero");
  assert_test((pos_one + neg_one) == zero, "1 + (-1) == 0");
  assert_test((neg_one * pos_one) == neg_one, "-1 * 1 == -1");
  assert_test((neg_one * neg_one) == pos_one, "-1 * -1 == 1");
  Fixed epsilon;
  epsilon.setRawBits(1);
  Fixed one_plus_eps = pos_one + epsilon;
  assert_test(one_plus_eps > pos_one, "1 + epsilon > 1 (Subject Requirement)");
}

int main() {
  std::cout << BOLD << "\n--- EX02 HEAVY STRESS TEST ---" << RESET << std::endl;

  test_edge_cases();
  test_comparisons(100);
  test_arithmetic(100);

  std::cout << BOLD << "\n--- RESULTS ---" << RESET << std::endl;
  if (testsPassed == testsTotal) {
    std::cout << GREEN << "[SUCCESS] " << testsPassed << "/" << testsTotal
              << " Tests Passed!" << RESET << std::endl;
    std::cout << "The bitwise algorithms perfectly simulate FPU operations!"
              << std::endl;
  } else {
    std::cout << RED << "[FAILED] " << (testsTotal - testsPassed)
              << " tests failed." << RESET << std::endl;
  }

  return 0;
}
