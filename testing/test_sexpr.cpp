// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/344_sexpr.

#include "test_runner.hpp"
#include "../modules/344_sexpr/sexpr.hpp"

#include <string>

namespace {

testing::TestOutcome run() {
    // Trivial list.
    {
        sexpr::ParseError err;
        auto n = sexpr::parse("(a b c)", &err);
        if (!n)                 return testing::fail("parse trivial: " + err.message);
        if (!n->is_list())      return testing::fail("root not list");
        if (n->size() != 3)     return testing::fail("child count");
        if (n->head() != "a")   return testing::fail("head");
    }

    // Numbers vs atoms.
    {
        auto n = sexpr::parse("(v 20250114 -1.5e-3 name)");
        if (!n)               return testing::fail("parse number");
        auto & children = n->list();
        if (!children[1]->is_number()) return testing::fail("20250114 not number");
        if (!children[2]->is_number()) return testing::fail("-1.5e-3 not number");
        if (!children[3]->is_atom())   return testing::fail("name not atom");
    }

    // Escaped strings.
    {
        auto n = sexpr::parse(R"((prop "hello \"world\"" "one\ttwo"))");
        if (!n) return testing::fail("parse string");
        if (n->list()[1]->string() != "hello \"world\"") return testing::fail("string 1 escape");
        if (n->list()[2]->string() != "one\ttwo")        return testing::fail("string 2 escape");
    }

    // Round-trip a small KiCad-like snippet.
    {
        const char * src =
            "(kicad_sch\n"
            "\t(version 20250114)\n"
            "\t(generator \"tool\")\n"
            "\t(uuid \"11111111-2222-3333-4444-555555555555\")\n"
            "\t(paper \"A4\")\n"
            "\t(lib_symbols\n"
            "\t\t(symbol \"Device:R\"\n"
            "\t\t\t(property \"Reference\" \"R\" (at 0 2.54 0))\n"
            "\t\t)\n"
            "\t)\n"
            ")\n";
        auto n = sexpr::parse(src);
        if (!n) return testing::fail("parse snippet");
        std::string out = sexpr::to_kicad_string(*n);
        // Reparse the output; must produce an equivalent tree.
        auto n2 = sexpr::parse(out);
        if (!n2) return testing::fail("reparse round-trip");
        if (n2->head() != "kicad_sch") return testing::fail("round-trip head");
        auto lib = n2->find("lib_symbols");
        if (!lib) return testing::fail("round-trip lib_symbols missing");
        auto sym = lib->find("symbol");
        if (!sym) return testing::fail("round-trip symbol missing");
        if (sym->size() < 2 || !sym->list()[1]->is_string()
            || sym->list()[1]->string() != "Device:R")
            return testing::fail("round-trip symbol id");
    }

    // find / find_all.
    {
        auto n = sexpr::parse("(x (a 1) (a 2) (b 3))");
        if (n->find("a")->list()[1]->as_int() != 1) return testing::fail("find first");
        if (n->find_all("a").size() != 2)           return testing::fail("find_all count");
        if (!n->find("z") == false) return testing::fail("find miss returns nullptr expected");
        if (n->find("z"))           return testing::fail("find miss returned non-null");
    }

    // Number formatting.
    if (sexpr::format_double(12.7000000) != "12.7")   return testing::fail("format 12.7");
    if (sexpr::format_double(0.0)        != "0")      return testing::fail("format 0");
    if (sexpr::format_double(-1.5)       != "-1.5")   return testing::fail("format -1.5");

    return testing::ok();
}

const int _r = testing::register_test(
    "sexpr",
    "S-expression parser + writer: parse, kinds, strings with escapes, round-trip a KiCad-shaped snippet, find/find_all, number format.",
    &run);

} // namespace
