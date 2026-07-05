// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 033_people.

#include "test_runner.hpp"
#include "../modules/033_people/people.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_people";
    return fs::path("/tmp") / "tool_test_people";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    people::init();

    people::Person a;
    a.name         = "Jane Bauer";
    a.aliases      = {"jbauer","Bauer"};
    a.role         = "EE lead";
    a.affiliations = {"ACME"};
    a.notes        = "Layout reviewer for the dispenser board.";
    int64_t a_id = people::add(people::Scope::Project, a, root.string());
    if (a_id != 1) return testing::fail("first id not 1: " + std::to_string(a_id));

    people::Person b;
    b.name    = "Sam Lee";
    b.aliases = {"slee"};
    b.role    = "FFL dealer";
    int64_t b_id = people::add(people::Scope::Project, b, root.string());
    if (b_id != 2) return testing::fail("second id not 2");

    if (people::list(people::Scope::Project, root.string()).size() != 2)
        return testing::fail("list count wrong");

    // Lookup by name and alias, case-insensitive.
    auto by_name = people::find(people::Scope::Project, "Jane Bauer", root.string());
    if (!by_name || by_name->id != a_id) return testing::fail("find by name failed");
    auto by_alias = people::find(people::Scope::Project, "JBAUER", root.string());
    if (!by_alias || by_alias->id != a_id) return testing::fail("find by alias failed");
    auto miss = people::find(people::Scope::Project, "nobody", root.string());
    if (miss) return testing::fail("find returned hit for unknown");

    // Mentions: both people are named in the prompt, in order.
    auto mentions = people::find_mentions(people::Scope::Project,
        "Layout: jbauer reviewed; please ship to Sam Lee for transfer.",
        root.string());
    if (mentions.size() != 2 || mentions[0] != a_id || mentions[1] != b_id)
        return testing::fail("mentions order wrong");

    // Bauer alone (substring) should still match Jane via word boundary; but
    // filed should NOT match anything.
    auto m_bauer = people::find_mentions(people::Scope::Project,
        "Bauer signed the layout.", root.string());
    if (m_bauer.size() != 1 || m_bauer[0] != a_id)
        return testing::fail("word boundary missed Bauer");
    auto m_neg = people::find_mentions(people::Scope::Project,
        "Files were filed; nothing to do.", root.string());
    if (!m_neg.empty()) return testing::fail("word boundary let a false hit through");

    // link_entry is idempotent.
    people::link_entry(people::Scope::Project, a_id, 42, root.string());
    people::link_entry(people::Scope::Project, a_id, 42, root.string());
    auto a2 = people::get(people::Scope::Project, a_id, root.string());
    if (!a2 || a2->linked_entries.size() != 1 || a2->linked_entries[0] != 42)
        return testing::fail("link_entry idempotence broken");

    // Update keeps the id and replaces fields.
    people::Person au = *a2;
    au.role = "EE lead (interim)";
    people::update(people::Scope::Project, a_id, au, root.string());
    auto a3 = people::get(people::Scope::Project, a_id, root.string());
    if (!a3 || a3->role.find("interim") == std::string::npos)
        return testing::fail("update did not stick");

    people::forget(people::Scope::Project, b_id, root.string());
    if (people::get(people::Scope::Project, b_id, root.string()))
        return testing::fail("forget did not remove person");
    if (people::list(people::Scope::Project, root.string()).size() != 1)
        return testing::fail("list count after forget wrong");

    people::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "people",
    "033_people: CRM-style records + alias find + boundary-aware mention scan",
    &run);

}
