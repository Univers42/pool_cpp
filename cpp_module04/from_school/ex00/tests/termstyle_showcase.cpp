/* ═══════════════════════════════════════════════════════════
 *  termstyle_showcase.cpp
 *
 *  Full demonstration of the TermStyle + TermConf + Logger
 *  system.  Zero hardcoded spacing — everything is driven
 *  by the configuration.
 *
 *  Build:  make test
 * ═══════════════════════════════════════════════════════════ */

#include "TermStyle.hpp"
#include "TermConf.hpp"
#include "Logger.hpp"
#include "Srgb.hpp"
#include <iostream>

int main() {

    /* ══════════════════════════════════════════════════════
     *  PART A — Default preset
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::applyDefault(ts);

        std::cout << ts.h1("PART A: Default Preset") << std::endl;
        std::cout << ts.h2("Heading Levels") << std::endl;
        std::cout << ts.h3("h3 — Third-Level Heading") << std::endl;
        std::cout << ts.separator() << std::endl;

        std::cout << ts.info("Informational message")   << std::endl;
        std::cout << ts.warn("Warning message")          << std::endl;
        std::cout << ts.error("Error message")           << std::endl;
        std::cout << ts.success("Success message")       << std::endl;
        std::cout << ts.separator() << std::endl;

        std::cout << ts.h3("Inline Styles") << std::endl;
        std::cout << ts.bold("Bold text")                << std::endl;
        std::cout << ts.italic("Italic text")            << std::endl;
        std::cout << ts.underline("Underlined text")     << std::endl;
        std::cout << ts.dim("Dim text")                  << std::endl;
        std::cout << ts.strike("Strikethrough text")     << std::endl;
        std::cout << ts.separator() << std::endl;

        std::cout << ts.h3("Block Elements") << std::endl;
        std::cout << ts.quote("The only way to do great work") << std::endl;
        std::cout << ts.quote("is to love what you do.")       << std::endl;
        std::cout << ts.separator() << std::endl;
        std::cout << ts.hr() << std::endl;
        std::cout << ts.section("Section Title",
            "Body text inside a section block.") << std::endl;
        std::cout << ts.text("Plain text rendering.") << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART B — Minimal preset
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::applyMinimal(ts);

        std::cout << ts.h1("PART B: Minimal Preset") << std::endl;
        std::cout << ts.h2("Second-Level Heading")   << std::endl;
        std::cout << ts.info("Minimal info")         << std::endl;
        std::cout << ts.warn("Minimal warning")      << std::endl;
        std::cout << ts.error("Minimal error")       << std::endl;
        std::cout << ts.success("Minimal success")   << std::endl;
        std::cout << ts.separator() << std::endl;
        std::cout << ts.quote("A minimal quote.") << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART C — Compact preset (zero spacing)
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::applyCompact(ts);

        std::cout << ts.h1("PART C: Compact Preset") << std::endl;
        std::cout << ts.info("Compact info")         << std::endl;
        std::cout << ts.warn("Compact warning")      << std::endl;
        std::cout << ts.error("Compact error")       << std::endl;
        std::cout << ts.success("Compact success")   << std::endl;
        std::cout << ts.separator() << std::endl;
        std::cout << ts.quote("Compact quote.") << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART D — Wide preset (80 cols, generous spacing)
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::applyWide(ts);

        std::cout << ts.h1("PART D: Wide Preset") << std::endl;
        std::cout << ts.h2("Wide Second-Level Heading") << std::endl;
        std::cout << ts.info("Wide info")            << std::endl;
        std::cout << ts.warn("Wide warning")         << std::endl;
        std::cout << ts.separator() << std::endl;
        std::cout << ts.section("Wide Section",
            "Body text in wide mode.") << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART E — Custom user configuration
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf conf;

        // user customizes the config
        conf.globalWidth        = 70;
        conf.h1SpaceBefore      = 2;
        conf.h1SpaceAfter       = 1;
        conf.h1Fg               = Srgb(255, 100, 200);
        conf.h1Border           = Srgb(200, 50, 150);
        conf.h1Font             = TermStyle::BOLD | TermStyle::ITALIC;
        conf.h1Glyph            = Glyph::STAR;
        conf.h1Sep              = Glyph::HDASH_HVY;
        conf.h1Align            = TermStyle::CENTER;

        conf.h2Fg               = Srgb(100, 220, 255);
        conf.h2Glyph            = Glyph::ARROW;
        conf.h2Sep              = Glyph::DOUBLE;

        conf.infoFg             = Srgb(120, 255, 120);
        conf.infoGlyph          = Glyph::ARROW;
        conf.errorFg            = Srgb(255, 0, 0);
        conf.errorGlyph         = Glyph::CROSS;

        conf.quoteGlyph         = Glyph::VBAR_THIN;
        conf.quoteFg            = Srgb(0, 220, 220);

        conf.sepStr             = Glyph::DOTTED;
        conf.sepBorder          = Srgb(200, 100, 50);

        // apply once, then use
        conf.apply(ts);

        std::cout << ts.h1("PART E: Custom Config") << std::endl;
        std::cout << ts.h2("Custom Second-Level")   << std::endl;
        std::cout << ts.info("Custom green info with arrow")    << std::endl;
        std::cout << ts.error("Custom red error")               << std::endl;
        std::cout << ts.success("Custom success message")       << std::endl;
        std::cout << ts.separator() << std::endl;
        std::cout << ts.quote("Custom cyan quote with thin bar") << std::endl;
        std::cout << ts.section("Custom Section",
            "Body with dotted separators.") << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART F — Glyph constants catalog
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::applyDefault(ts);

        std::cout << ts.h1("PART F: Glyph Constants") << std::endl;

        std::cout << ts.text(std::string("BULLET    = ") + Glyph::BULLET)    << std::endl;
        std::cout << ts.text(std::string("DIAMOND   = ") + Glyph::DIAMOND)   << std::endl;
        std::cout << ts.text(std::string("TRIANGLE  = ") + Glyph::TRIANGLE)  << std::endl;
        std::cout << ts.text(std::string("ARROW     = ") + Glyph::ARROW)     << std::endl;
        std::cout << ts.text(std::string("STAR      = ") + Glyph::STAR)      << std::endl;
        std::cout << ts.text(std::string("INFO      = ") + Glyph::INFO)      << std::endl;
        std::cout << ts.text(std::string("WARN      = ") + Glyph::WARN)      << std::endl;
        std::cout << ts.text(std::string("CROSS     = ") + Glyph::CROSS)     << std::endl;
        std::cout << ts.text(std::string("CHECK     = ") + Glyph::CHECK)     << std::endl;
        std::cout << ts.text(std::string("VBAR      = ") + Glyph::VBAR)      << std::endl;
        std::cout << ts.text(std::string("VBAR_THIN = ") + Glyph::VBAR_THIN) << std::endl;
        std::cout << ts.separator() << std::endl;

        std::cout << ts.h3("Separator units (tiled x4)") << std::endl;
        std::cout << ts.text(std::string("HDASH     = ") + Glyph::HDASH + Glyph::HDASH + Glyph::HDASH + Glyph::HDASH) << std::endl;
        std::cout << ts.text(std::string("HDASH_HVY = ") + Glyph::HDASH_HVY + Glyph::HDASH_HVY + Glyph::HDASH_HVY + Glyph::HDASH_HVY) << std::endl;
        std::cout << ts.text(std::string("DOUBLE    = ") + Glyph::DOUBLE + Glyph::DOUBLE + Glyph::DOUBLE + Glyph::DOUBLE) << std::endl;
        std::cout << ts.text(std::string("DOTTED    = ") + Glyph::DOTTED + Glyph::DOTTED + Glyph::DOTTED + Glyph::DOTTED) << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART G — Macro-based logging (via Logger.hpp)
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::applyDefault(ts);

        std::cout << ts.h1("PART G: PRINT_LOG Macros") << std::endl;

        PRINT_LOG(InfoLog("Macro-based info"));
        PRINT_LOG(WarnLog("Macro-based warning"));
        PRINT_LOG(ErrorLog("Macro-based error"));
        PRINT_LOG(SuccessLog("Macro-based success"));
        PRINT_SEP();
        PRINT_HEADER("Macro h1 heading");
        PRINT_H2("Macro h2 heading");
        PRINT_H3("Macro h3 heading");
        PRINT_QUOTE("Macro-based quote");
        PRINT_TEXT("Macro-based text");
        PRINT_BOLD("Macro-based bold");
        PRINT_ITALIC("Macro-based italic");
        PRINT_HR();
    }

    /* ══════════════════════════════════════════════════════
     *  PART H — Font flag combinations
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::applyDefault(ts);

        std::cout << ts.h1("PART H: Font Flag Combinations") << std::endl;

        TermStyle::ElemStyle original = ts.textStyle();

        ts.textStyle().font = TermStyle::BOLD;
        std::cout << ts.text("BOLD") << std::endl;

        ts.textStyle().font = TermStyle::DIM;
        std::cout << ts.text("DIM") << std::endl;

        ts.textStyle().font = TermStyle::ITALIC;
        std::cout << ts.text("ITALIC") << std::endl;

        ts.textStyle().font = TermStyle::UNDERLINE;
        std::cout << ts.text("UNDERLINE") << std::endl;

        ts.textStyle().font = TermStyle::STRIKE;
        std::cout << ts.text("STRIKETHROUGH") << std::endl;

        ts.textStyle().font = TermStyle::REVERSE;
        std::cout << ts.text("REVERSE") << std::endl;

        ts.textStyle().font = TermStyle::BOLD | TermStyle::ITALIC | TermStyle::UNDERLINE;
        std::cout << ts.text("BOLD + ITALIC + UNDERLINE") << std::endl;

        ts.textStyle().font = TermStyle::DIM | TermStyle::ITALIC | TermStyle::STRIKE;
        std::cout << ts.text("DIM + ITALIC + STRIKETHROUGH") << std::endl;

        ts.textStyle() = original;
    }

    {
        TermStyle ts;
        TermConf::applyDefault(ts);
        std::cout << ts.h1("Showcase Complete") << std::endl;
    }

    return 0;
}
