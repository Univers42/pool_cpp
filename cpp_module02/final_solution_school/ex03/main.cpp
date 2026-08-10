/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 21:49:55 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/04 17:01:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

#include "Point.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// Colour helpers
// ─────────────────────────────────────────────────────────────────────────────
#define GREEN  "\033[32m"
#define RED    "\033[31m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"
#define BOLD   "\033[1m"
#define RESET  "\033[0m"

// ─────────────────────────────────────────────────────────────────────────────
// Global counters
// ─────────────────────────────────────────────────────────────────────────────
static int g_total  = 0;
static int g_passed = 0;

// ─────────────────────────────────────────────────────────────────────────────
// Banner helpers
// ─────────────────────────────────────────────────────────────────────────────
static void banner(const char* title)
{
    std::cout << "\n" BOLD CYAN
              << "══════════════════════════════════════════════════════\n"
              << "  " << title << "\n"
              << "══════════════════════════════════════════════════════\n"
              << RESET;
}

static void sub(const char* title)
{
    std::cout << BOLD << "\n  ── " << title << " ──\n" << RESET;
}

// ─────────────────────────────────────────────────────────────────────────────
// Core assertion
// ─────────────────────────────────────────────────────────────────────────────
static void check(const std::string& name, bool expected, bool got)
{
    ++g_total;
    bool ok = (expected == got);
    if (ok)
    {
        ++g_passed;
        std::cout << GREEN << "  [PASS]" << RESET << " " << name << "\n";
    }
    else
    {
        std::cout << RED << "  [FAIL]" << RESET << " " << name
                  << "  (expected " << (expected ? "true" : "false")
                  << ", got "       << (got      ? "true" : "false") << ")\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// HOW BSP WORKS — cross-product sign method
// ─────────────────────────────────────────────────────────────────────────────
//
//  For a triangle ABC and a query point P, compute the signed area (cross
//  product) of three sub-triangles:
//
//    d1 = cross(AB, AP)
//    d2 = cross(BC, BP)
//    d3 = cross(CA, CP)
//
//  cross(UV, UW) = (Vx-Ux)*(Wy-Uy) - (Vy-Uy)*(Wx-Ux)
//
//  Interpretation:
//    > 0  →  P is to the LEFT  of edge (counter-clockwise)
//    < 0  →  P is to the RIGHT of edge (clockwise)
//    = 0  →  P lies exactly ON the edge (or vertex)
//
//  P is STRICTLY INSIDE iff all three cross products share the same sign
//  AND none of them is zero.
//
//  If any cross product is zero, P is on an edge or vertex → return FALSE.
//
// ─────────────────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────────────────
// SUITE 1 — Right-angle triangle at origin
//
//   C (0,10)
//   │╲
//   │  ╲
//   │    ╲
//   │      ╲
//   A──────── B (10,0)
//  (0,0)
//
// ─────────────────────────────────────────────────────────────────────────────
static void suite_right_triangle(void)
{
    banner("SUITE 1 — Right-angle triangle  A(0,0) B(10,0) C(0,10)");

    Point A(0.0f,  0.0f);
    Point B(10.0f, 0.0f);
    Point C(0.0f, 10.0f);

    std::cout <<
        "   C(0,10)\n"
        "   │╲\n"
        "   │  ╲\n"
        "   │  P ╲\n"
        "   │      ╲\n"
        "   A────────B(10,0)\n\n";

    sub("Strictly inside");
    check("(1,1)   centre area",        true,  bsp(A, B, C, Point(1.0f,  1.0f)));
    check("(2,2)   diagonal third",     true,  bsp(A, B, C, Point(2.0f,  2.0f)));
    check("(4,4)   near hypotenuse",    true,  bsp(A, B, C, Point(4.0f,  4.0f)));
    check("(0.5,0.5) very close corner",true,  bsp(A, B, C, Point(0.5f,  0.5f)));

    sub("Strictly outside");
    check("(10,10) far corner",         false, bsp(A, B, C, Point(10.0f, 10.0f)));
    check("(-1,-1) behind origin",      false, bsp(A, B, C, Point(-1.0f,-1.0f)));
    check("(6,6)   beyond hypotenuse",  false, bsp(A, B, C, Point(6.0f,  6.0f)));
    check("(11,0)  right of B",         false, bsp(A, B, C, Point(11.0f, 0.0f)));
    check("(0,11)  above C",            false, bsp(A, B, C, Point(0.0f, 11.0f)));

    sub("On edge / vertex  → must return FALSE (not strictly inside)");
    check("(0,0)   vertex A",           false, bsp(A, B, C, Point(0.0f,  0.0f)));
    check("(10,0)  vertex B",           false, bsp(A, B, C, Point(10.0f, 0.0f)));
    check("(0,10)  vertex C",           false, bsp(A, B, C, Point(0.0f, 10.0f)));
    check("(5,0)   mid AB edge",        false, bsp(A, B, C, Point(5.0f,  0.0f)));
    check("(0,5)   mid AC edge",        false, bsp(A, B, C, Point(0.0f,  5.0f)));
    check("(5,5)   mid BC hypotenuse",  false, bsp(A, B, C, Point(5.0f,  5.0f)));
}

// ─────────────────────────────────────────────────────────────────────────────
// SUITE 2 — Equilateral-ish triangle (all positive coords)
//
//        T(5,8)
//       /  ╲
//      /  P  ╲
//     /        ╲
//    L(0,0)────R(10,0)
//
// ─────────────────────────────────────────────────────────────────────────────
static void suite_general_triangle(void)
{
    banner("SUITE 2 — General triangle  L(0,0) R(10,0) T(5,8)");

    Point L(0.0f, 0.0f);
    Point R(10.0f, 0.0f);
    Point T(5.0f,  8.0f);

    std::cout <<
        "        T(5,8)\n"
        "       /  ╲\n"
        "      /  P  ╲\n"
        "     /        ╲\n"
        "    L(0,0)──────R(10,0)\n\n";

    sub("Strictly inside");
    check("(5,4)  centroid area",       true,  bsp(L, R, T, Point(5.0f, 4.0f)));
    check("(3,2)  left half",           true,  bsp(L, R, T, Point(3.0f, 2.0f)));
    check("(7,2)  right half",          true,  bsp(L, R, T, Point(7.0f, 2.0f)));
    check("(5,7)  near apex",           true,  bsp(L, R, T, Point(5.0f, 7.0f)));

    sub("Strictly outside");
    check("(5,9)  above apex",          false, bsp(L, R, T, Point(5.0f,  9.0f)));
    check("(-1,0) left of L",           false, bsp(L, R, T, Point(-1.0f, 0.0f)));
    check("(11,0) right of R",          false, bsp(L, R, T, Point(11.0f, 0.0f)));
    check("(0,8)  same height as T",    false, bsp(L, R, T, Point(0.0f,  8.0f)));

    sub("On edge / vertex  → FALSE");
    check("(5,8)  vertex T",            false, bsp(L, R, T, Point(5.0f,  8.0f)));
    check("(0,0)  vertex L",            false, bsp(L, R, T, Point(0.0f,  0.0f)));
    check("(5,0)  mid base LR",         false, bsp(L, R, T, Point(5.0f,  0.0f)));
    check("(2.5,4) mid left edge LT",   false, bsp(L, R, T, Point(2.5f,  4.0f)));
    check("(7.5,4) mid right edge RT",  false, bsp(L, R, T, Point(7.5f,  4.0f)));
}

// ─────────────────────────────────────────────────────────────────────────────
// SUITE 3 — Negative coordinate triangle
//
//        T(0,5)
//       /  ╲
//      /  P  ╲
//     /        ╲
//   L(-5,-5)──R(5,-5)
//
// ─────────────────────────────────────────────────────────────────────────────
static void suite_negative_coords(void)
{
    banner("SUITE 3 — Negative coordinates  L(-5,-5) R(5,-5) T(0,5)");

    Point L(-5.0f, -5.0f);
    Point R( 5.0f, -5.0f);
    Point T( 0.0f,  5.0f);

    std::cout <<
        "        T(0,5)\n"
        "       /  ╲\n"
        "      /  P  ╲\n"
        "     /        ╲\n"
        "  L(-5,-5)──────R(5,-5)\n\n";

    sub("Strictly inside");
    check("(0,0)   centroid",           true,  bsp(L, R, T, Point( 0.0f,  0.0f)));
    check("(-2,-2) lower-left area",    true,  bsp(L, R, T, Point(-2.0f, -2.0f)));
    check("(2,-2)  lower-right area",   true,  bsp(L, R, T, Point( 2.0f, -2.0f)));
    check("(0,3)   near apex",          true,  bsp(L, R, T, Point( 0.0f,  3.0f)));

    sub("Strictly outside");
    check("(0,6)   above apex",         false, bsp(L, R, T, Point( 0.0f,  6.0f)));
    check("(-6,-5) left of L",          false, bsp(L, R, T, Point(-6.0f, -5.0f)));
    check("(5,5)   top-right corner",   false, bsp(L, R, T, Point( 5.0f,  5.0f)));

    sub("On vertex / edge  → FALSE");
    check("(0,5)   vertex T",           false, bsp(L, R, T, Point( 0.0f,  5.0f)));
    check("(-5,-5) vertex L",           false, bsp(L, R, T, Point(-5.0f, -5.0f)));
    check("(0,-5)  mid base LR",        false, bsp(L, R, T, Point( 0.0f, -5.0f)));
}

// ─────────────────────────────────────────────────────────────────────────────
// SUITE 4 — Thin / almost-degenerate triangle
//
//  Testing that bsp stays correct even when the triangle is very narrow
//  (but still non-degenerate — area > 0).
//
//   A(0,0) ──────────────── B(100,0)
//                  C(50, 1)   ← very flat
//
// ─────────────────────────────────────────────────────────────────────────────
static void suite_thin_triangle(void)
{
    banner("SUITE 4 — Thin triangle  A(0,0) B(100,0) C(50,1)");

    Point A(0.0f,  0.0f);
    Point B(100.0f, 0.0f);
    Point C(50.0f,  1.0f);

    std::cout <<
        "  A(0,0) ──────────────── B(100,0)\n"
        "              C(50,1)  ← very flat\n\n";

    sub("Strictly inside  (tiny strip near C)");
    check("(50, 0.5) centre of strip",  true,  bsp(A, B, C, Point(50.0f,  0.5f)));
    check("(25, 0.4) left of centre",   true,  bsp(A, B, C, Point(25.0f,  0.4f)));
    check("(75, 0.4) right of centre",  true,  bsp(A, B, C, Point(75.0f,  0.4f)));

    sub("Outside / on edge  → FALSE");
    check("(50,0)  on base AB",         false, bsp(A, B, C, Point(50.0f,  0.0f)));
    check("(50,1)  vertex C",           false, bsp(A, B, C, Point(50.0f,  1.0f)));
    check("(50,2)  above C",            false, bsp(A, B, C, Point(50.0f,  2.0f)));
    check("(-1,0)  left of A",          false, bsp(A, B, C, Point(-1.0f,  0.0f)));
}

// ─────────────────────────────────────────────────────────────────────────────
// SUITE 5 — Clockwise winding (vertices given in CW order)
//
//  The BSP algorithm must work regardless of winding order.
//  We reuse the right-angle triangle but pass vertices in CW order:
//  A(0,0) C(0,10) B(10,0)  instead of A B C.
//
// ─────────────────────────────────────────────────────────────────────────────
static void suite_clockwise_winding(void)
{
    banner("SUITE 5 — Clockwise winding  A(0,0) C(0,10) B(10,0)");

    Point A(0.0f,  0.0f);
    Point B(10.0f, 0.0f);
    Point C(0.0f, 10.0f);

    std::cout <<
        "  Same triangle as Suite 1, vertices passed in CW order.\n"
        "  bsp() must return the same results either way.\n\n";

    // Pass as A, C, B  (clockwise)
    check("(2,2) inside  (CW winding)", true,  bsp(A, C, B, Point(2.0f, 2.0f)));
    check("(5,5) on hyp  (CW winding)", false, bsp(A, C, B, Point(5.0f, 5.0f)));
    check("(10,10) outside(CW winding)",false,  bsp(A, C, B, Point(10.0f,10.0f)));
    check("(0,0) vertex A(CW winding)", false, bsp(A, C, B, Point(0.0f, 0.0f)));
}

// ─────────────────────────────────────────────────────────────────────────────
// SUMMARY
// ─────────────────────────────────────────────────────────────────────────────
static void print_summary(void)
{
    std::cout << "\n" BOLD
              << "══════════════════════════════════════════════════════\n"
              << "  RESULTS: " << g_passed << " / " << g_total << " tests passed\n"
              << "══════════════════════════════════════════════════════\n"
              << RESET;
    if (g_passed == g_total)
        std::cout << GREEN BOLD "  ✓  All tests passed — BSP is correct!\n" RESET "\n";
    else
        std::cout << RED BOLD "  ✗  " << (g_total - g_passed)
                  << " test(s) failed.\n" RESET "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// ENTRY POINT
// ─────────────────────────────────────────────────────────────────────────────
int main(void)
{
    suite_right_triangle();
    suite_general_triangle();
    suite_negative_coords();
    suite_thin_triangle();
    suite_clockwise_winding();
    print_summary();
    return (g_passed == g_total ? 0 : 1);
}
