/* ═══════════════════════════════════════════════════════════
 *  writer_demo.cpp — Demonstrate TermWriter
 *
 *  Three modes:
 *    1) Imperative:   w.h1("Title"); w.text("...");
 *    2) parseFile():  w.parseFile("doc.md");
 *    3) operator<<:   w << "# Title" << "> " + obj.method();
 *
 *  Mode 3 is the sweet spot — write markdown naturally in
 *  code AND inject runtime values with string concatenation.
 *
 *  Build:   make test
 *  Run:     ./build/bin/writer_demo
 * ═══════════════════════════════════════════════════════════ */

#include "TermWriter.hpp"
#include "TermConf.hpp"
#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include "Srgb.hpp"
#include <iostream>

/* ═══════════════════════════════════════════════════════════
 *  Theme setup
 * ═══════════════════════════════════════════════════════════ */

static void setupTheme(TermStyle& ts) {
    TermConf c;
    c.globalWidth = 72;

    c.h1SpaceBefore = 2;  c.h1SpaceAfter = 1;
    c.h2SpaceBefore = 1;  c.h2SpaceAfter = 0;
    c.h3SpaceBefore = 1;  c.h3SpaceAfter = 0;

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
    c.h3Font   = TermStyle::BOLD | TermStyle::ITALIC;
    c.h3Glyph  = Glyph::TRIANGLE;

    c.infoFg       = Srgb( 80, 200, 240);
    c.infoGlyph    = Glyph::INFO;
    c.warnFg       = Srgb(255, 190,  50);
    c.warnGlyph    = Glyph::WARN;
    c.errorFg      = Srgb(255,  70,  70);
    c.errorGlyph   = Glyph::CROSS;
    c.successFg    = Srgb( 50, 230, 130);
    c.successGlyph = Glyph::CHECK;

    c.bulletFg    = Srgb(200, 200, 180);
    c.bulletGlyph = Glyph::TRIANGLE;
    c.olFg        = Srgb(180, 210, 230);

    c.quoteFg    = Srgb(200, 180, 150);
    c.quoteGlyph = Glyph::VBAR;
    c.textFg     = Srgb(220, 215, 205);
    c.sepBorder  = Srgb(120, 100,  70);
    c.sepStr     = Glyph::HDASH;

    c.calloutFg     = Srgb(210, 200, 180);
    c.calloutBg     = Srgb( 35,  25,  15);
    c.calloutBorder = Srgb(200, 140,  50);
    c.calloutGlyph  = Glyph::VBAR;
    c.calloutHasBg  = true;

    c.apply(ts);
}

/* ═══════════════════════════════════════════════════════════
 *  Register custom callout presets
 * ═══════════════════════════════════════════════════════════ */

static void registerCallouts(TermWriter& w) {
    {
        TermStyle::ElemStyle danger;
        danger.fg        = Srgb(255, 180, 160);
        danger.bg        = Srgb( 50,  15,  10);
        danger.border    = Srgb(255,  70,  70);
        danger.glyph     = Glyph::CROSS;
        danger.bodyGlyph = Glyph::VBAR;
        danger.hasBg     = true;
        danger.width     = 72;
        danger.padL      = 3;
        danger.padR      = 2;
        danger.marginL   = 1;
        w.defineCallout("danger", danger);
    }
    {
        TermStyle::ElemStyle tip;
        tip.fg        = Srgb(160, 230, 160);
        tip.bg        = Srgb( 10,  35,  15);
        tip.border    = Srgb( 50, 230, 130);
        tip.glyph     = Glyph::CHECK;
        tip.bodyGlyph = Glyph::VBAR;
        tip.hasBg     = true;
        tip.width     = 72;
        tip.padL      = 3;
        tip.padR      = 2;
        tip.marginL   = 1;
        w.defineCallout("tip", tip);
    }
    {
        TermStyle::ElemStyle note;
        note.fg        = Srgb(180, 200, 240);
        note.bg        = Srgb( 15,  20,  40);
        note.border    = Srgb( 80, 140, 255);
        note.glyph     = Glyph::INFO;
        note.bodyGlyph = Glyph::VBAR;
        note.hasBg     = true;
        note.width     = 72;
        note.padL      = 3;
        note.padR      = 2;
        note.marginL   = 1;
        w.defineCallout("note", note);
    }
}

/* ═══════════════════════════════════════════════════════════
 *  main — the user just writes markdown, freely
 * ═══════════════════════════════════════════════════════════ */

/* ═══════════════════════════════════════════════════════════
 *  main — real-world usage: objects + formatted output
 * ═══════════════════════════════════════════════════════════ */

int main() {
    TermStyle ts;
    setupTheme(ts);

    TermWriter w(ts);
    registerCallouts(w);

    /* ── Create the animals (runtime data) ─────────────── */
    const Animal* dog = new Dog();
    const Animal* cat = new Cat();
    const WrongAnimal* wrongCat = new WrongCat();

    /* ── Write the document — one << per line ──────────── *
     *                                                       *
     *  Each << is one markdown line.  No \n needed.         *
     *  Use + to inject any runtime value.  That's it.       *
     * ───────────────────────────────────────────────────── */

    w << "# C++04 — Polymorphism Report"
      << "Generated at runtime with real objects."
      << "---"
      << ""
      << "## Registered Animals"
      << "- Type: " + dog->getType()
      << "- Type: " + cat->getType()
      << "- Type (wrong): " + wrongCat->getType()
      << ""
      << "## Virtual Dispatch"
      << "### Correct: Animal* pointing to " + dog->getType();
    w << "> Calling makeSound() on Animal* that holds a " + dog->getType() + ":";
    w << "!v Output:";
    w.flush();
    dog->makeSound();

    w << ""
      << "### Correct: Animal* pointing to " + cat->getType();
    w << "> Calling makeSound() on Animal* that holds a " + cat->getType() + ":";
    w << "!v Output:";
    w.flush();
    cat->makeSound();

    w << ""
      << "### Wrong: WrongAnimal* pointing to " + wrongCat->getType();
    w << "> Calling makeSound() on WrongAnimal* (non-virtual):";
    w << "!x Output:";
    w.flush();
    wrongCat->makeSound();

    w << ""
      << ">![danger] Non-virtual Destructor"
      << "> WrongAnimal::makeSound() is NOT virtual."
      << "> The pointer type decides the call, not the object type."
      << "> Same problem with the destructor: only ~WrongAnimal() runs."
      << ">"
      << ""
      << ">![tip] Why Virtual Matters"
      << "> Virtual functions let the OBJECT decide behavior."
      << "> Without virtual, the POINTER type decides."
      << "> Always use virtual destructors in polymorphic bases."
      << ">"
      << ""
      << "## Orthodox Canonical Form"
      << "1. Default constructor"
      << "2. Copy constructor (deep copy)"
      << "3. Copy assignment operator (deep copy)"
      << "4. Virtual destructor"
      << ""
      << "## Sections"
      << "$$ delete Animal*(" + dog->getType() + ") / ~" + dog->getType() + "() then ~Animal() — correct chain"
      << ""
      << "$$ delete WrongAnimal*(" + wrongCat->getType() + ") / ~WrongAnimal() ONLY — leak!"
      << ""
      << ">![note] Key Takeaway"
      << "> Polymorphism is not just syntax."
      << "> It determines which destructor runs, which method"
      << "> dispatches, and whether your program leaks or crashes."
      << ">"
      << ""
      << "## Animal Registry (imperative table)";

    /* ── Imperative table: built with runtime data ───── */
    {
        TermTable t(3, w.tableStyle());
        t.title("Polymorphic Animals");
        t.header("Type", "Pointer", "Virtual?");
        t.row(dog->getType(),      "Animal*",      "Yes");
        t.row(cat->getType(),      "Animal*",      "Yes");
        t.row(wrongCat->getType(), "WrongAnimal*", "No");
        w.table(t);
    }

    w << ""
      << "## Canonical Form (markdown table)"
      << ""
      << "| Step | Operation | Rule | hhello|my name|"
      << "|------|-------------------------|------|"
      << "| 1 | Default constructor | Zero-init members |fsadf|"
      << "| 2 | Copy constructor | Deep copy resources | " + dog->getType() + " |"
      << "| 3 | operator= | Release + deep copy |"
      << "| 4 | Destructor | Release all resources |"
      << ""
      << "==="
      << "~Built with TermWriter + TermStyle + TermTable  |  C++98  |  42 school~";

    w.flush();

    /* ── cleanup ───────────────────────────────────────── */
    delete dog;
    delete cat;
    delete wrongCat;
    return 0;
}

