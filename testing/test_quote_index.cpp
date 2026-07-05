// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 058_quote_index.

#include "test_runner.hpp"
#include "../modules/058_quote_index/quote_index.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_quotes";
    return fs::path("/tmp") / "tool_test_quotes";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    quote_index::open(root.string());

    quote_index::Quote q1;
    q1.source_ref = "ref:42";
    q1.locator    = "p.5";
    q1.text       = "Capybara thermoregulation across seasons.";
    q1.before     = "We measure";
    q1.after      = "across observed habitats.";
    q1.tags       = {"animal","experiment"};
    int64_t id1 = quote_index::add(q1);

    quote_index::Quote q2;
    q2.source_ref = "url:https://example.com/argon";
    q2.locator    = "§3.2";
    q2.text       = "Argon flow is set to 25 sccm in the chamber.";
    q2.tags       = {"chemistry","setup"};
    int64_t id2 = quote_index::add(q2);

    if (id1 == id2) return testing::fail("ids not unique");

    auto by_src = quote_index::list_by_source("ref:42");
    if (by_src.size() != 1 || by_src[0].id != id1)
        return testing::fail("list_by_source wrong");

    auto hits = quote_index::search("thermoregulation");
    if (hits.empty() || hits[0].quote.id != id1)
        return testing::fail("FTS missed quote");
    if (hits[0].snippet.find("[[") == std::string::npos)
        return testing::fail("snippet missing highlight");

    // Update + get round trip.
    q1.text = "Capybara thermoregulation across all four seasons (revised).";
    quote_index::update(id1, q1);
    auto upd = quote_index::get(id1);
    if (!upd || upd->text.find("revised") == std::string::npos)
        return testing::fail("update did not stick");

    auto md = quote_index::format_markdown(*upd);
    if (md.find("> Capybara") == std::string::npos ||
        md.find("ref:42") == std::string::npos ||
        md.find("p.5") == std::string::npos)
        return testing::fail("format_markdown wrong: " + md);

    quote_index::forget(id2);
    if (quote_index::get(id2)) return testing::fail("forget did not delete");

    quote_index::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "quote_index",
    "058_quote_index: FTS5 store for highlighted passages + markdown render",
    &run);

}
