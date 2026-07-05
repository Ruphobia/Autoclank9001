// Smoke test for 022_project_memory.

#include "test_runner.hpp"
#include "../modules/022_project_memory/project_memory.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_project_memory";
    return fs::path("/tmp") / "tool_test_project_memory";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    project_memory::open(root.string());

    auto id1 = project_memory::add("observation",
        "The dispenser pump stalls if rinse pressure drops below 12 psi.",
        {"hardware","dispenser"}, "chat:turn-3");
    auto id2 = project_memory::add("decision",
        "Settled on SQLite for project memory because we already link it.",
        {"infra","db"}, "manual");
    auto id3 = project_memory::add("fact",
        "Customer uses anhydrous IPA, never the azeotrope.",
        {"chemistry","ipa","customer"}, "datasheet:p7");

    if (id1 == id2 || id2 == id3) return testing::fail("ids not unique");

    auto e = project_memory::get(id2);
    if (!e || e->content.find("SQLite") == std::string::npos)
        return testing::fail("get round trip broken");

    auto by_cat = project_memory::list("decision");
    if (by_cat.size() != 1 || by_cat[0].id != id2)
        return testing::fail("list by category wrong");

    auto by_tag = project_memory::list({}, "chemistry");
    if (by_tag.size() != 1 || by_tag[0].id != id3)
        return testing::fail("list by tag wrong: " + std::to_string(by_tag.size()));

    auto hits = project_memory::search("dispenser pressure");
    if (hits.empty()) return testing::fail("no hits for dispenser pressure");
    if (hits[0].entry.id != id1)
        return testing::fail("expected first hit to be id1");

    project_memory::update(id2, "Reaffirmed SQLite as the project memory backend.",
                           {"infra","db","reaffirmed"}, "manual");
    auto upd = project_memory::get(id2);
    if (!upd || upd->content.find("Reaffirmed") == std::string::npos)
        return testing::fail("update did not stick");
    bool saw_reaff = false;
    for (const auto & t : upd->tags) if (t == "reaffirmed") saw_reaff = true;
    if (!saw_reaff) return testing::fail("update did not persist new tag");

    project_memory::forget(id3);
    if (project_memory::get(id3)) return testing::fail("forget did not delete");

    auto all = project_memory::list();
    if (all.size() != 2) return testing::fail("list count after forget wrong");

    project_memory::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "project_memory",
    "022_project_memory: SQLite + FTS5 entry store with categories and tags",
    &run);

}
