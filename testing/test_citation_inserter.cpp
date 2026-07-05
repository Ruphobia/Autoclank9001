// Smoke test for 061_citation_inserter.

#include "test_runner.hpp"
#include "../modules/061_citation_inserter/citation_inserter.hpp"

namespace {

testing::TestOutcome run() {
    using citation_inserter::Style;
    using citation_inserter::Format;

    nlohmann::json item = {
        {"id", "bauer2026capybara"},
        {"author", nlohmann::json::array({{{"family","Bauer"},{"given","Jane"}}})},
        {"issued", {{"date-parts", nlohmann::json::array({{2026}})}}}
    };

    auto md = citation_inserter::render(item, Style::AuthorDate, Format::Markdown);
    if (md != "(Bauer, 2026)")
        return testing::fail("markdown author-date wrong: " + md);

    auto md_loc = citation_inserter::render(item,
        Style::AuthorDateLocator, Format::Markdown, "p.5");
    if (md_loc != "(Bauer, 2026, p.5)")
        return testing::fail("markdown locator wrong: " + md_loc);

    auto pd = citation_inserter::render(item, Style::AuthorDate, Format::Pandoc);
    if (pd != "[@bauer2026capybara]")
        return testing::fail("pandoc wrong: " + pd);
    auto pd_loc = citation_inserter::render(item, Style::AuthorDate,
        Format::Pandoc, "p.5");
    if (pd_loc != "[@bauer2026capybara, p.5]")
        return testing::fail("pandoc locator wrong: " + pd_loc);

    auto lx = citation_inserter::render(item, Style::Numeric, Format::LaTeX);
    if (lx != "\\cite{bauer2026capybara}")
        return testing::fail("latex cite wrong: " + lx);
    auto lx_loc = citation_inserter::render(item, Style::Numeric,
        Format::LaTeX, "p.5");
    if (lx_loc != "\\cite[p.5]{bauer2026capybara}")
        return testing::fail("latex locator wrong: " + lx_loc);
    auto lx_text = citation_inserter::render(item, Style::NatbibTextual, Format::LaTeX);
    if (lx_text != "\\citet{bauer2026capybara}")
        return testing::fail("natbib textual wrong: " + lx_text);

    auto tp = citation_inserter::render(item, Style::AuthorDate, Format::Typst);
    if (tp != "@bauer2026capybara")
        return testing::fail("typst wrong: " + tp);

    auto org = citation_inserter::render(item, Style::AuthorDate, Format::Org);
    if (org != "[cite:@bauer2026capybara]")
        return testing::fail("org wrong: " + org);

    // prefix / suffix wrap the rendered marker with spaces.
    auto wrap = citation_inserter::render(item, Style::AuthorDate,
        Format::Markdown, {}, "see", "for an extended treatment");
    if (wrap != "see (Bauer, 2026) for an extended treatment")
        return testing::fail("prefix/suffix wrap wrong: " + wrap);

    // Missing date -> n.d.
    nlohmann::json bare = {
        {"id", "anon"},
        {"author", nlohmann::json::array({{{"family","Doe"}}})}
    };
    auto nd = citation_inserter::render(bare, Style::AuthorDate, Format::Markdown);
    if (nd != "(Doe, n.d.)")
        return testing::fail("n.d. fallback wrong: " + nd);

    return testing::ok();
}

const int _r = testing::register_test(
    "citation_inserter",
    "061_citation_inserter: render across Style x Format matrix",
    &run);

}
