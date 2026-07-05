// SPDX-License-Identifier: GPL-3.0-or-later
// Deterministic fuzz for modules/344_sexpr (todo.txt task 90).
// Generates pseudo-random s-expression-like inputs and asserts:
//   * parser never crashes / infinite-loops
//   * successful parse -> writer -> parse round-trips to the same tree
// Uses a fixed LCG so we don't rely on Date.now()/random() which are
// blocked in some scripting environments; seed changes with SEEDS[].

#include "test_runner.hpp"
#include "../modules/344_sexpr/sexpr.hpp"

#include <cstdint>
#include <sstream>
#include <string>

namespace {

// Xorshift32 deterministic RNG.
struct Rng {
    std::uint32_t s;
    explicit Rng(std::uint32_t seed) : s(seed ? seed : 1) {}
    std::uint32_t next() {
        s ^= s << 13; s ^= s >> 17; s ^= s << 5;
        return s;
    }
    std::uint32_t range(std::uint32_t max) { return max ? next() % max : 0; }
};

// Generate a small random tree, at most `depth` deep.
sexpr::SExprPtr gen(Rng & rng, int depth) {
    if (depth <= 0 || rng.range(4) == 0) {
        std::uint32_t kind = rng.range(3);
        if (kind == 0) return sexpr::SExpr::make_atom("a" + std::to_string(rng.range(1000)));
        if (kind == 1) return sexpr::SExpr::make_number(std::to_string(static_cast<int>(rng.range(1000)) - 500));
        return sexpr::SExpr::make_string(std::string("s") + std::to_string(rng.range(1000)));
    }
    auto l = sexpr::SExpr::make_list();
    l->list().push_back(sexpr::SExpr::make_atom(std::string("h") + std::to_string(rng.range(50))));
    int n = static_cast<int>(rng.range(5));
    for (int i = 0; i < n; ++i) l->list().push_back(gen(rng, depth - 1));
    return l;
}

// Also generate raw-junk inputs to make sure the parser tolerates garbage.
std::string junk(Rng & rng, std::size_t n) {
    static const char alphabet[] = "()\"\\ \n\ttbncdefG12";
    std::string s; s.reserve(n);
    for (std::size_t i = 0; i < n; ++i) s += alphabet[rng.range(sizeof(alphabet) - 1)];
    return s;
}

testing::TestOutcome run() {
    // Structured round-trip fuzz.
    for (std::uint32_t seed : {1u, 42u, 12345u, 7357u, 424242u}) {
        Rng rng(seed);
        for (int k = 0; k < 32; ++k) {
            auto tree = gen(rng, 4);
            std::string text = sexpr::to_kicad_string(*tree);
            sexpr::ParseError e;
            auto reparsed = sexpr::parse(text, &e);
            if (!reparsed) return testing::fail("structured fuzz: reparse failed seed=" +
                                                std::to_string(seed) + " msg=" + e.message);
            std::string text2 = sexpr::to_kicad_string(*reparsed);
            if (text != text2) return testing::fail("structured fuzz: not fixed point after two emit passes");
        }
    }

    // Junk-tolerance fuzz. Parser may return null; must not crash or hang.
    for (std::uint32_t seed : {9999u, 12321u}) {
        Rng rng(seed);
        for (int k = 0; k < 128; ++k) {
            std::string j = junk(rng, 64);
            sexpr::ParseError e;
            (void) sexpr::parse(j, &e);
        }
    }
    return testing::ok();
}

const int _r = testing::register_test(
    "sexpr_fuzz",
    "Deterministic sexpr fuzz: structured trees round-trip through writer, junk input tolerated without crash.",
    &run);

} // namespace
