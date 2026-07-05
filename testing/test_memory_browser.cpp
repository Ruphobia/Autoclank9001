// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 038_memory_browser.

#include "test_runner.hpp"
#include "../modules/038_memory_browser/memory_browser.hpp"
#include "../modules/022_project_memory/project_memory.hpp"
#include "../modules/023_personal_memory/personal_memory.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_browser";
    return fs::path("/tmp") / "tool_test_browser";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    project_memory::open(root.string());
    personal_memory::open(root.string());

    project_memory::add("fact",       "Dispenser stalls below 12 psi.",
        {"dispenser","hardware"}, "composer:confidence=0.90 link=chat:turn-3");
    project_memory::add("decision",   "Use SQLite for memory.",
        {"infra","db"},           "composer:confidence=0.95 link=manual");
    project_memory::add("preference", "No em dashes.",
        {"style"},                "composer:confidence=0.50 link=manual");
    personal_memory::add("preference","Prefer metric units.",
        {"style","units"},        "composer:confidence=0.99 link=manual");
    personal_memory::add("equipment", "Owns Rigol DS1054Z.",
        {"hardware","bench"},     "manual");

    memory_browser::Filter f;
    f.scope = memory_browser::Scope::Both;
    auto everything = memory_browser::query(f, root.string(), 100);
    if (everything.size() != 5) return testing::fail("all-rows count != 5");

    // Tag filter
    memory_browser::Filter ftag;
    ftag.scope    = memory_browser::Scope::Both;
    ftag.tags_all = {"hardware"};
    auto hw = memory_browser::query(ftag, root.string(), 100);
    if (hw.size() != 2) return testing::fail("tag filter count != 2");

    // Confidence floor
    memory_browser::Filter fconf;
    fconf.scope          = memory_browser::Scope::Both;
    fconf.confidence_min = 0.90;
    auto high = memory_browser::query(fconf, root.string(), 100);
    // Entries without an explicit composer:confidence= marker (here:
    // the "manual"-sourced equipment row) default to 1.0 and pass.
    if (high.size() != 4)
        return testing::fail("confidence floor count != 4: " + std::to_string(high.size()));

    // Scope filter
    memory_browser::Filter fper;
    fper.scope = memory_browser::Scope::Personal;
    auto per   = memory_browser::query(fper, root.string(), 100);
    if (per.size() != 2) return testing::fail("scope=personal count != 2");

    // Text substring
    memory_browser::Filter ftxt;
    ftxt.scope          = memory_browser::Scope::Both;
    ftxt.text_substring = "DISPENSER";  // case-insensitive
    auto txt = memory_browser::query(ftxt, root.string(), 100);
    if (txt.size() != 1) return testing::fail("substring filter count != 1");

    // Facet counts: category over all
    auto by_cat = memory_browser::facet_counts(f, memory_browser::Facet::Category, root.string());
    if (by_cat["preference"] != 2) return testing::fail("preference count wrong");
    if (by_cat["fact"]       != 1) return testing::fail("fact count wrong");

    auto by_tag = memory_browser::facet_counts(f, memory_browser::Facet::Tag, root.string());
    if (by_tag["hardware"] != 2 || by_tag["style"] != 2)
        return testing::fail("tag facet counts wrong");

    auto by_scope = memory_browser::facet_counts(f, memory_browser::Facet::Scope, root.string());
    if (by_scope["project"] != 3 || by_scope["personal"] != 2)
        return testing::fail("scope facet counts wrong");

    // YearMonth bucket exists (we don't know the value precisely; just non-empty).
    auto by_ym = memory_browser::facet_counts(f, memory_browser::Facet::YearMonth, root.string());
    if (by_ym.empty()) return testing::fail("year-month facet empty");

    project_memory::close();
    personal_memory::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "memory_browser",
    "038_memory_browser: faceted query + facet counts over project + personal",
    &run);

}
