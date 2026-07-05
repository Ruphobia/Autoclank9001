// Smoke test for 064_regex_designer.

#include "test_runner.hpp"
#include "../modules/064_regex_designer/regex_designer.hpp"

namespace {

testing::TestOutcome run() {
    regex_designer::init();

    // test(): find_all + captures.
    auto r = regex_designer::test(R"((\w+)=(\d+))", "a=1 b=22 cc=333", false, true);
    if (!r.valid_pattern) return testing::fail("valid pattern reported invalid");
    if (r.matches.size() != 3)
        return testing::fail("expected 3 matches, got " + std::to_string(r.matches.size()));
    if (r.matches[1].captures.size() != 2 ||
        r.matches[1].captures[0].text != "b" ||
        r.matches[1].captures[1].text != "22")
        return testing::fail("group capture wrong");

    // Named groups.
    auto named = regex_designer::test(R"((?<key>\w+)=(?<val>\d+))", "a=1", false, false);
    if (named.matches.empty() ||
        named.matches[0].captures[0].name != "key" ||
        named.matches[0].captures[1].name != "val")
        return testing::fail("named groups not surfaced");

    // case insensitive
    auto ci = regex_designer::test("HELLO", "hello world", true, true);
    if (ci.matches.empty()) return testing::fail("case-insensitive flag ignored");

    // Invalid pattern reports error, does not throw.
    auto bad = regex_designer::test("(", "x");
    if (bad.valid_pattern) return testing::fail("invalid pattern not caught");
    if (bad.error.empty()) return testing::fail("error text missing");

    // Translate named groups PCRE <-> ECMA.
    auto to_pcre = regex_designer::translate(R"((?<name>\w+))",
        regex_designer::Dialect::Ecma, regex_designer::Dialect::Pcre);
    if (to_pcre.find("(?P<name>") == std::string::npos)
        return testing::fail("ECMA->PCRE named-group rewrite missed");

    auto to_ecma = regex_designer::translate(R"((?P<name>\w+))",
        regex_designer::Dialect::Pcre, regex_designer::Dialect::Ecma);
    if (to_ecma.find("(?<name>") == std::string::npos)
        return testing::fail("PCRE->ECMA named-group rewrite missed");

    // Possessive quantifier degrades to greedy when target lacks it.
    auto degraded = regex_designer::translate("a++b",
        regex_designer::Dialect::Pcre, regex_designer::Dialect::Re2);
    if (degraded != "a+b")
        return testing::fail("possessive degrade wrong: " + degraded);

    // Synthesis seed
    auto syn = regex_designer::synthesize_from_examples({"item-7", "item-9", "item-3"});
    if (syn != "item-.")
        return testing::fail("synthesis wrong: " + syn);

    auto syn_tail = regex_designer::synthesize_from_examples({"abc", "abcd", "abcde"});
    if (syn_tail != "abc.*")
        return testing::fail("variable-length tail synthesis wrong: " + syn_tail);

    auto syn_empty = regex_designer::synthesize_from_examples({});
    if (!syn_empty.empty()) return testing::fail("empty input not handled");

    regex_designer::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "regex_designer",
    "064_regex_designer: tester + dialect translate + synthesis seed",
    &run);

}
