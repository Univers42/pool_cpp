/* ═══════════════════════════════════════════════════════════
 *  readme.cpp — A terminal README for CPP Module 04
 *
 *  This file is a narrative walkthrough of the project,
 *  rendered entirely with TermStyle + TermConf.
 *  Think of it as a README.md that lives in your terminal.
 *
 *  ┌────────────────────────────────────────────────────┐
 *  │  Toggle the README_STYLE define below to switch    │
 *  │  between visual presets:                           │
 *  │                                                    │
 *  │    0 = Rich   (colorful, wide, generous spacing)   │
 *  │    1 = Clean  (muted palette, balanced)            │
 *  │    2 = Mono   (grayscale, compact, serious)        │
 *  │    3 = Hacker (green-on-black, minimal)            │
 *  └────────────────────────────────────────────────────┘
 *
 *  Build:   make test
 *  Run:     ./build/bin/readme
 * ═══════════════════════════════════════════════════════════ */

#include "TermStyle.hpp"
#include "TermConf.hpp"
#include "Srgb.hpp"
#include <iostream>

/* ── CHANGE THIS VALUE TO SWITCH PRESETS ───────────────── */
#define README_STYLE 0

/* ═══════════════════════════════════════════════════════════
 *  Custom preset: Rich (warm, wide, generous spacing)
 * ═══════════════════════════════════════════════════════════ */
static void applyRich(TermStyle& ts) {
    TermConf c;
    c.globalWidth        = 72;

    // spacing — breathable like a real README
    c.h1SpaceBefore      = 2;
    c.h1SpaceAfter       = 1;
    c.h2SpaceBefore      = 1;
    c.h2SpaceAfter       = 0;
    c.h3SpaceBefore      = 1;
    c.h3SpaceAfter       = 0;
    c.logSpaceBefore     = 0;
    c.logSpaceAfter      = 0;
    c.sepSpaceBefore     = 0;
    c.sepSpaceAfter      = 0;
    c.quoteSpaceBefore   = 0;
    c.quoteSpaceAfter    = 0;
    c.sectionSpaceBefore = 1;
    c.sectionSpaceAfter  = 0;
    c.textSpaceBefore    = 0;
    c.textSpaceAfter     = 0;

    // warm sunset palette
    c.h1Fg     = Srgb(255, 200, 100);
    c.h1Border = Srgb(200, 140,  50);
    c.h1Font   = TermStyle::BOLD;
    c.h1Glyph  = Glyph::STAR;
    c.h1Sep    = Glyph::DOUBLE;
    c.h1Align  = TermStyle::CENTER;
    c.h1Bg     = Srgb( 60,  30,  10);
    c.h1HasBg  = true;
    c.h1PadV   = 1;

    c.h2Fg     = Srgb(130, 200, 255);
    c.h2Border = Srgb( 60, 120, 180);
    c.h2Font   = TermStyle::BOLD;
    c.h2Glyph  = Glyph::DIAMOND;
    c.h2Sep    = Glyph::HDASH_HVY;

    c.h3Fg     = Srgb(180, 230, 160);
    c.h3Border = Srgb( 80, 150,  60);
    c.h3Font   = TermStyle::BOLD | TermStyle::ITALIC;
    c.h3Glyph  = Glyph::TRIANGLE;
    c.h3Sep    = Glyph::DOTTED;

    c.infoFg        = Srgb( 80, 200, 240);
    c.infoGlyph     = Glyph::INFO;
    c.warnFg        = Srgb(255, 190,  50);
    c.warnGlyph     = Glyph::WARN;
    c.errorFg       = Srgb(255,  70,  70);
    c.errorGlyph    = Glyph::CROSS;
    c.successFg     = Srgb( 50, 230, 130);
    c.successGlyph  = Glyph::CHECK;

    c.quoteFg    = Srgb(200, 180, 150);
    c.quoteGlyph = Glyph::VBAR;
    c.textFg     = Srgb(220, 215, 205);
    c.sepBorder  = Srgb(120, 100,  70);
    c.sepStr     = Glyph::HDASH;

    // callout blocks
    c.calloutFg     = Srgb(210, 200, 180);
    c.calloutBg     = Srgb( 35,  25,  15);
    c.calloutBorder = Srgb(200, 140,  50);
    c.calloutGlyph  = Glyph::VBAR;
    c.calloutHasBg  = true;

    c.apply(ts);
}

/* ═══════════════════════════════════════════════════════════
 *  Custom preset: Clean (muted, balanced)
 * ═══════════════════════════════════════════════════════════ */
static void applyClean(TermStyle& ts) {
    TermConf c;
    c.globalWidth        = 68;

    c.h1SpaceBefore      = 1;
    c.h1SpaceAfter       = 0;
    c.h2SpaceBefore      = 1;
    c.h2SpaceAfter       = 0;
    c.h3SpaceBefore      = 1;
    c.h3SpaceAfter       = 0;
    c.sectionSpaceBefore = 1;
    c.sectionSpaceAfter  = 0;

    c.h1Fg     = Srgb(200, 200, 220);
    c.h1Border = Srgb(140, 140, 160);
    c.h1Font   = TermStyle::BOLD;
    c.h1Glyph  = Glyph::DIAMOND;
    c.h1Sep    = Glyph::HDASH_HVY;
    c.h1Align  = TermStyle::CENTER;
    c.h1Bg     = Srgb( 30,  30,  40);
    c.h1HasBg  = true;
    c.h1PadV   = 1;

    c.h2Fg     = Srgb(160, 190, 220);
    c.h2Border = Srgb(100, 130, 160);
    c.h2Font   = TermStyle::BOLD;
    c.h2Glyph  = Glyph::TRIANGLE;
    c.h2Sep    = Glyph::HDASH;

    c.h3Fg     = Srgb(170, 200, 170);
    c.h3Font   = TermStyle::ITALIC;
    c.h3Glyph  = Glyph::BULLET;

    c.infoFg   = Srgb(140, 190, 220);
    c.warnFg   = Srgb(220, 190, 100);
    c.errorFg  = Srgb(220, 100, 100);
    c.successFg= Srgb(100, 210, 140);

    c.quoteFg    = Srgb(170, 170, 170);
    c.quoteGlyph = Glyph::VBAR_THIN;
    c.textFg     = Srgb(200, 200, 200);
    c.sepBorder  = Srgb(100, 100, 110);
    c.sepStr     = Glyph::HDASH;

    c.calloutFg     = Srgb(190, 190, 200);
    c.calloutBg     = Srgb( 25,  25,  35);
    c.calloutBorder = Srgb(120, 120, 140);
    c.calloutGlyph  = Glyph::VBAR_THIN;
    c.calloutHasBg  = true;

    c.apply(ts);
}

/* ═══════════════════════════════════════════════════════════
 *  Custom preset: Mono (grayscale, compact)
 * ═══════════════════════════════════════════════════════════ */
static void applyMono(TermStyle& ts) {
    TermConf c;
    c.globalWidth        = 64;

    c.h1SpaceBefore      = 1;
    c.h1SpaceAfter       = 0;
    c.h2SpaceBefore      = 1;
    c.h2SpaceAfter       = 0;
    c.h3SpaceBefore      = 0;
    c.h3SpaceAfter       = 0;
    c.sectionSpaceBefore = 0;
    c.sectionSpaceAfter  = 0;

    c.h1Fg     = Srgb(240, 240, 240);
    c.h1Border = Srgb(160, 160, 160);
    c.h1Font   = TermStyle::BOLD;
    c.h1Glyph  = "";
    c.h1Sep    = Glyph::DOUBLE;
    c.h1Align  = TermStyle::CENTER;
    c.h1Bg     = Srgb( 40,  40,  40);
    c.h1HasBg  = true;
    c.h1PadV   = 1;

    c.h2Fg     = Srgb(210, 210, 210);
    c.h2Border = Srgb(130, 130, 130);
    c.h2Font   = TermStyle::BOLD;
    c.h2Glyph  = "";
    c.h2Sep    = Glyph::HDASH_HVY;

    c.h3Fg     = Srgb(190, 190, 190);
    c.h3Font   = TermStyle::BOLD;
    c.h3Glyph  = "";

    c.infoFg    = Srgb(180, 180, 180);
    c.infoGlyph = Glyph::BULLET;
    c.warnFg    = Srgb(220, 220, 180);
    c.warnGlyph = Glyph::BULLET;
    c.errorFg   = Srgb(220, 180, 180);
    c.errorGlyph= Glyph::CROSS;
    c.successFg = Srgb(180, 220, 180);
    c.successGlyph = Glyph::CHECK;

    c.quoteFg    = Srgb(150, 150, 150);
    c.quoteGlyph = Glyph::VBAR_THIN;
    c.textFg     = Srgb(190, 190, 190);
    c.sepBorder  = Srgb(100, 100, 100);
    c.sepStr     = Glyph::HDASH;

    c.calloutFg     = Srgb(170, 170, 170);
    c.calloutBg     = Srgb( 30,  30,  30);
    c.calloutBorder = Srgb(140, 140, 140);
    c.calloutGlyph  = Glyph::VBAR_THIN;
    c.calloutHasBg  = true;

    c.apply(ts);
}

/* ═══════════════════════════════════════════════════════════
 *  Custom preset: Hacker (green-on-black, minimal)
 * ═══════════════════════════════════════════════════════════ */
static void applyHacker(TermStyle& ts) {
    TermConf c;
    c.globalWidth        = 72;

    c.h1SpaceBefore      = 1;
    c.h1SpaceAfter       = 0;
    c.h2SpaceBefore      = 1;
    c.h2SpaceAfter       = 0;
    c.h3SpaceBefore      = 0;
    c.h3SpaceAfter       = 0;
    c.sectionSpaceBefore = 1;
    c.sectionSpaceAfter  = 0;

    Srgb bright(0, 255, 65);
    Srgb mid(0, 180, 40);
    Srgb dimGr(0, 120, 25);
    Srgb faint(0, 80, 15);

    c.h1Fg     = bright;
    c.h1Border = mid;
    c.h1Font   = TermStyle::BOLD;
    c.h1Glyph  = Glyph::ARROW;
    c.h1Sep    = Glyph::DOUBLE;
    c.h1Align  = TermStyle::LEFT;
    c.h1Bg     = Srgb(  0,  20,   5);
    c.h1HasBg  = true;
    c.h1PadV   = 1;

    c.h2Fg     = bright;
    c.h2Border = dimGr;
    c.h2Font   = TermStyle::BOLD;
    c.h2Glyph  = Glyph::ARROW;
    c.h2Sep    = Glyph::HDASH;

    c.h3Fg     = mid;
    c.h3Font   = TermStyle::NONE;
    c.h3Glyph  = Glyph::TRIANGLE;

    c.infoFg     = mid;
    c.infoGlyph  = Glyph::BULLET;
    c.warnFg     = Srgb(200, 200, 0);
    c.warnGlyph  = Glyph::WARN;
    c.errorFg    = Srgb(255, 50, 50);
    c.errorGlyph = Glyph::CROSS;
    c.successFg  = bright;
    c.successGlyph = Glyph::CHECK;

    c.quoteFg    = dimGr;
    c.quoteGlyph = Glyph::VBAR;
    c.textFg     = mid;
    c.sepBorder  = faint;
    c.sepStr     = Glyph::DOTTED;

    c.calloutFg     = mid;
    c.calloutBg     = Srgb(  0,  15,   3);
    c.calloutBorder = bright;
    c.calloutGlyph  = Glyph::VBAR;
    c.calloutHasBg  = true;

    c.apply(ts);
}

/* ═══════════════════════════════════════════════════════════
 *  Helper: apply the selected preset
 *
 *  All four functions are referenced so -Wunused-function
 *  never fires regardless of which README_STYLE is active.
 * ═══════════════════════════════════════════════════════════ */
typedef void (*ThemeFn)(TermStyle&);
static const ThemeFn g_themes[] = {
    &applyRich, &applyClean, &applyMono, &applyHacker
};

static void applyTheme(TermStyle& ts) {
    g_themes[README_STYLE](ts);
}

/* ═══════════════════════════════════════════════════════════
 *  Shorthand — every call goes through the same TermStyle
 * ═══════════════════════════════════════════════════════════ */
#define H1(t)        std::cout << ts.h1(t)        << std::endl
#define H2(t)        std::cout << ts.h2(t)        << std::endl
#define H3(t)        std::cout << ts.h3(t)        << std::endl
#define TEXT(t)      std::cout << ts.text(t)       << std::endl
#define BOLD(t)      std::cout << ts.bold(t)       << std::endl
#define ITALIC(t)    std::cout << ts.italic(t)     << std::endl
#define DIM(t)       std::cout << ts.dim(t)        << std::endl
#define UL(t)        std::cout << ts.underline(t)  << std::endl
#define STRIKE(t)    std::cout << ts.strike(t)     << std::endl
#define QUOTE(t)     std::cout << ts.quote(t)      << std::endl
#define INFO(t)      std::cout << ts.info(t)       << std::endl
#define WARN(t)      std::cout << ts.warn(t)       << std::endl
#define ERR(t)       std::cout << ts.error(t)      << std::endl
#define OK(t)        std::cout << ts.success(t)    << std::endl
#define SEP()        std::cout << ts.separator()   << std::endl
#define HR()         std::cout << ts.hr()          << std::endl
#define SEC(t,b)     std::cout << ts.section(t,b)  << std::endl
#define NL()         std::cout << std::endl

/* ═══════════════════════════════════════════════════════════
 *  main
 * ═══════════════════════════════════════════════════════════ */
int main() {
    TermStyle ts;
    applyTheme(ts);

    /* ─────────────────────────── TITLE ──────────────────── */
    H1("CPP MODULE 04 — Subtype Polymorphism");
    TEXT("42 School  |  C++98  |  dlesieur  |  March 2026");
    SEP();
    NL();

    /* ─────────────────────── OVERVIEW ───────────────────── */
    H2("Overview");
    TEXT("Module 04 is the gateway to object-oriented design");
    TEXT("in the 42 C++ piscine.  It covers four exercises");
    TEXT("that progressively build an understanding of:");
    NL();
    std::cout << ts.callout("Exercises",
        "ex00 -- Subtype polymorphism & virtual destructors",
        "ex01 -- Deep copy with Brain* and array of Animals",
        "ex02 -- Abstract classes (pure virtual = 0)",
        "ex03 -- Interfaces (ICharacter, IMateriaSource)") << std::endl;
    NL();
    QUOTE("The goal is not just to make it compile.");
    QUOTE("It is to understand WHY each mechanism exists.");
    SEP();

    /* ──────────── EXERCISE 00: POLYMORPHISM ─────────────── */
    H1("Exercise 00 — Polymorphism");

    H2("The Setup");
    TEXT("We create a base class Animal with a protected");
    TEXT("std::string type and a virtual makeSound() method.");
    TEXT("Dog and Cat inherit from Animal and override it.");
    NL();
    H3("Class hierarchy");
    TEXT("  Animal           <-- base, virtual dtor + makeSound");
    TEXT("    +-- Dog        <-- type=\"Dog\", barks");
    TEXT("    +-- Cat        <-- type=\"Cat\", meows");
    NL();
    TEXT("  WrongAnimal      <-- same structure but NO virtual");
    TEXT("    +-- WrongCat   <-- type=\"WrongCat\"");
    SEP();

    H2("The Core Problem: Why Virtual?");
    TEXT("When you delete a derived object through a base");
    TEXT("pointer, the compiler needs to know which destructor");
    TEXT("chain to call.  Without the virtual keyword, it only");
    TEXT("sees the base class destructor.");
    NL();
    SEC("Virtual (correct)",
        "delete dogPtr => ~Dog() then ~Animal() => full cleanup");
    NL();
    SEC("Non-virtual (WRONG)",
        "delete wrongCatPtr => ~WrongAnimal() ONLY => leak!");
    NL();
    std::cout << ts.callout("Warning",
        "This is the #1 cause of subtle memory leaks in C++.",
        "If a class is meant to be a base class, its",
        "destructor MUST be virtual.") << std::endl;
    SEP();

    H2("Static vs Dynamic Binding");
    H3("Dynamic binding (virtual)");
    TEXT("The compiler generates a vtable — a lookup table of");
    TEXT("function pointers.  At runtime, the program follows");
    TEXT("the vtable to call the correct override.");
    NL();
    OK("Animal* a = new Dog();");
    OK("a->makeSound();  // calls Dog::makeSound()");
    NL();
    H3("Static binding (no virtual)");
    TEXT("The compiler resolves the call at compile time based");
    TEXT("on the POINTER TYPE, not the actual object type.");
    NL();
    ERR("WrongAnimal* w = new WrongCat();");
    ERR("w->makeSound();  // calls WrongAnimal::makeSound()!");
    SEP();

    H2("The WrongAnimal Trap");
    TEXT("The subject forces us to implement WrongAnimal and");
    TEXT("WrongCat side-by-side with the correct ones.");
    TEXT("This contrast is the whole teaching moment:");
    NL();
    QUOTE("WrongAnimal is not a mistake you fix.");
    QUOTE("It is a mistake you understand.");
    NL();
    TEXT("By seeing both outcomes in the same terminal, you");
    TEXT("learn to never forget the virtual keyword again.");
    HR();

    /* ──────────── EXERCISE 01: DEEP COPY ────────────────── */
    H1("Exercise 01 — Deep Copy & Brain");

    H2("The Problem");
    TEXT("Dog and Cat now own a Brain* allocated with new.");
    TEXT("When you copy a Dog, a shallow copy just duplicates");
    TEXT("the pointer — both objects point to the same Brain.");
    NL();
    std::cout << ts.callout("Danger: Shallow Copy",
        "Shallow copy: two Dogs share one Brain.",
        "Delete one => dangling pointer in the other.",
        "Delete both => double free => crash.") << std::endl;
    NL();

    H2("The Solution");
    TEXT("Implement proper copy constructor and copy assignment");
    TEXT("operator that allocate a NEW Brain and copy its 100");
    TEXT("idea strings one by one.  This is a deep copy.");
    NL();
    OK("Deep copy: each Dog owns its own Brain.");
    OK("Delete one => the other is unaffected.");
    NL();
    H3("The Orthodox Canonical Form");
    TEXT("Every class that manages a resource needs:");
    TEXT("  1. Default constructor");
    TEXT("  2. Copy constructor         (deep copy)");
    TEXT("  3. Copy assignment operator  (deep copy)");
    TEXT("  4. Destructor               (release resource)");
    NL();
    std::cout << ts.callout("Warning",
        "If you forget even one, the compiler generates a",
        "shallow version that will bite you at runtime.") << std::endl;
    SEP();

    /* ──────────── EXERCISE 02: ABSTRACT ─────────────────── */
    H1("Exercise 02 — Abstract Classes");

    H2("Why Abstract?");
    TEXT("Does it make sense to instantiate a generic Animal?");
    TEXT("What sound does a Generic Animal make?  None.");
    TEXT("The class exists only as a contract — a promise that");
    TEXT("every derived class will implement makeSound().");
    NL();
    TEXT("By making makeSound() pure virtual:");
    QUOTE("virtual void makeSound() const = 0;");
    NL();
    TEXT("Animal becomes abstract.  The compiler enforces that");
    TEXT("nobody writes new Animal() — only Dog and Cat.");
    NL();
    H3("Naming convention");
    TEXT("The subject suggests renaming Animal to AAnimal.");
    TEXT("The A prefix is a 42 convention meaning Abstract.");
    std::cout << ts.callout("Naming",
        "AAnimal => abstract, not instantiable",
        "Dog, Cat => concrete, instantiable") << std::endl;
    SEP();

    /* ──────────── EXERCISE 03: INTERFACES ───────────────── */
    H1("Exercise 03 — Interfaces & Materia");

    H2("Interfaces in C++98");
    TEXT("C++ has no 'interface' keyword.  Instead, we use");
    TEXT("pure abstract classes — classes where EVERY method");
    TEXT("is pure virtual (= 0) and there are no data members.");
    NL();
    std::cout << ts.callout("ICharacter",
        "virtual std::string const& getName() const = 0;",
        "virtual void equip(AMateria* m) = 0;",
        "virtual void unequip(int idx) = 0;",
        "virtual void use(int idx, ICharacter& t) = 0;") << std::endl;
    NL();
    std::cout << ts.callout("IMateriaSource",
        "virtual void learnMateria(AMateria*) = 0;",
        "virtual AMateria* createMateria(string const&) = 0;") << std::endl;
    NL();
    TEXT("Character and MateriaSource are the concrete classes");
    TEXT("that implement these interfaces.  The Materia system");
    TEXT("(Ice, Cure) uses clone() to create copies — a");
    TEXT("textbook application of the Prototype pattern.");
    NL();
    H2("Memory Management Gotchas");
    std::cout << ts.callout("Watch Out",
        "unequip() must NOT delete the Materia.",
        "You must track floor Materias to avoid leaks.",
        "Deep copy of Character must delete old Materias",
        "before cloning the new ones into the inventory.") << std::endl;
    SEP();

    /* ──────────── PROBLEMS & LESSONS ────────────────────── */
    H1("Problems Encountered & Lessons Learned");

    H2("1. Pointer vs Reference Syntax");
    TEXT("Our very first bug: calling a.getType() instead of");
    TEXT("a->getType() on a pointer.  Simple, but the kind of");
    TEXT("mistake that C++ will punish you for.");
    NL();
    ERR("const Animal* a = new Dog();");
    ERR("a.getType();   // won't compile");
    OK("a->getType();  // correct");
    SEP();

    H2("2. C++98 Compatibility");
    TEXT("42 mandates -std=c++98.  This means:");
    NL();
    std::cout << ts.callout("C++98 Restrictions",
        "No std::to_string    => use std::stringstream",
        "No override keyword  => just careful coding",
        "No range-based for   => classic index loops",
        "No auto              => explicit types everywhere") << std::endl;
    NL();
    TEXT("Every modern convenience we take for granted is gone.");
    TEXT("You learn what the language actually IS under the");
    TEXT("syntactic sugar.");
    SEP();

    H2("3. The Decorator/Logger System");
    TEXT("We built a reusable verbose output system from");
    TEXT("scratch to make constructor/destructor messages");
    TEXT("readable.  It evolved through several iterations:");
    NL();
    H3("Architecture evolution");
    std::cout << ts.callout("Evolution",
        "v1: Decorator base class (canonical form)",
        "v2: + VerboseDecorator (ANSI color + font flags)",
        "v3: + Srgb class (true-color with bitwise ops)",
        "v4: + Logger.hpp (factory functions, not classes)",
        "v5: + TermStyle (markdown-like terminal renderer)",
        "v6: + TermConf (centralized configuration presets)") << std::endl;
    NL();
    TEXT("The key insight: separate the WHAT (message content)");
    TEXT("from the HOW (color, glyph, spacing, alignment).");
    QUOTE("Configuration should live in one place.");
    QUOTE("Rendering should never hardcode style decisions.");
    SEP();

    H2("4. Compile-Time Verbose Toggle");
    TEXT("Using #define VERBOSE 0/1 in Decorator.hpp, all");
    TEXT("PRINT_* macros expand to ((void)0) when disabled.");
    NL();
    OK("VERBOSE=1 => full styled output, ~zero-cost macros");
    OK("VERBOSE=0 => every PRINT_* becomes ((void)0)");
    WARN("No runtime branch, no dead code, zero overhead.");
    SEP();

    H2("5. Unicode in C++98");
    TEXT("C++98 has no native Unicode support.  We embed");
    TEXT("UTF-8 byte sequences as hex escape strings:");
    NL();
    QUOTE("static const char BULLET[] = \"\\xe2\\x97\\x8f\";");
    NL();
    TEXT("The visLen() function skips ANSI escapes AND counts");
    TEXT("multi-byte UTF-8 characters correctly for alignment.");
    TEXT("tileSep() tiles these multi-byte units across the");
    TEXT("full terminal width.");
    SEP();

    /* ──────────── THE STYLE SYSTEM ──────────────────────── */
    H1("The TermStyle System");

    H2("Design");
    TEXT("TermStyle is a markdown-like renderer for terminals.");
    TEXT("Each element type (h1, h2, info, quote, etc.) has");
    TEXT("its own ElemStyle descriptor containing:");
    NL();
    std::cout << ts.callout("ElemStyle Fields",
        "fg         -- foreground color (Srgb)",
        "border     -- separator color (Srgb)",
        "font       -- bitfield (BOLD|DIM|ITALIC|UNDERLINE)",
        "align      -- LEFT, CENTER, or RIGHT",
        "width      -- total line width",
        "padL/padR  -- inner padding",
        "marginL    -- left margin",
        "spaceBefore/spaceAfter -- auto-injected newlines",
        "glyph      -- prefix icon (from Glyph:: namespace)",
        "sepStr     -- separator unit (tiled to fill width)") << std::endl;
    SEP();

    H2("TermConf — Centralized Configuration");
    TEXT("Instead of tweaking individual ElemStyle fields,");
    TEXT("you build a TermConf object, set its properties,");
    TEXT("and call conf.apply(ts).  One call configures");
    TEXT("every element at once.");
    NL();
    TEXT("Four built-in presets:");
    OK("TermConf::applyDefault(ts)  — the standard look");
    OK("TermConf::applyMinimal(ts)  — 50-col, simple");
    OK("TermConf::applyCompact(ts)  — zero spacing");
    OK("TermConf::applyWide(ts)     — 80-col, generous");
    NL();
    TEXT("Or create your own, like this README does:");
    QUOTE("Rich, Clean, Mono, Hacker — pick a number.");
    SEP();

    H2("The Srgb Class");
    TEXT("A compact RGB color class with bitwise operators:");
    NL();
    std::cout << ts.callout("Srgb API",
        "Stores (r << 16) | (g << 8) | b as an int",
        "operator| merges, operator& masks, operator~ inverts",
        "toAnsi() emits \\033[38;2;R;G;Bm escape codes",
        "toHex() returns #RRGGBB for debugging") << std::endl;
    NL();
    TEXT("This lets you treat colors as first-class values,");
    TEXT("combine them with bitwise logic, and convert to");
    TEXT("ANSI true-color in one call.");
    SEP();

    /* ──────────── FINAL THOUGHTS ────────────────────────── */
    H1("Final Thoughts");

    TEXT("CPP Module 04 teaches three connected ideas:");
    NL();
    SEC("Polymorphism",
        "Virtual functions let the runtime choose the right behavior.");
    NL();
    SEC("Abstraction",
        "Abstract classes enforce contracts that derived classes must honor.");
    NL();
    SEC("Interfaces",
        "Pure abstract classes simulate interfaces, decoupling API from implementation.");
    NL();
    TEXT("These are not just C++ features.  They are design");
    TEXT("principles that appear in every object-oriented");
    TEXT("language.  Master them here, use them everywhere.");
    NL();
    HR();
    QUOTE("If your base class has virtual functions,");
    QUOTE("its destructor must be virtual too.");
    QUOTE("That is the lesson.  Everything else is details.");
    HR();
    NL();

    DIM("Built with TermStyle + TermConf  |  C++98  |  42 school");
    DIM("Change README_STYLE (0-3) at the top of readme.cpp");
    DIM("to see this same content in different visual themes.");

    return 0;
}
