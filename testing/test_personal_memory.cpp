// Smoke test for 023_personal_memory.

#include "test_runner.hpp"
#include "../modules/023_personal_memory/personal_memory.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_personal_memory";
    return fs::path("/tmp") / "tool_test_personal_memory";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    personal_memory::open(root.string());
    if (personal_memory::store_path().find("personal.sqlite") == std::string::npos)
        return testing::fail("store_path not reported: " + personal_memory::store_path());

    auto a = personal_memory::add("preference",
        "Prefer no em dashes in prose; use colons or semicolons.",
        {"writing","style"}, "manual");
    auto b = personal_memory::add("equipment",
        "Owns Keysight 34465A DMM, Rigol DS1054Z scope.",
        {"hardware","bench"}, "manual");
    auto c = personal_memory::add("contact",
        "ACME Arms FFL; 555-0100; processes NFA Form 4 in-house.",
        {"ffl","arms"}, "manual");

    if (personal_memory::list().size() != 3)
        return testing::fail("list count after add wrong");

    auto pref = personal_memory::list("preference");
    if (pref.size() != 1 || pref[0].id != a)
        return testing::fail("list by category wrong");

    auto hw = personal_memory::list({}, "hardware");
    if (hw.size() != 1 || hw[0].id != b)
        return testing::fail("list by tag wrong");

    auto hits = personal_memory::search("em dashes");
    if (hits.empty() || hits[0].entry.id != a)
        return testing::fail("FTS search missed em-dash preference");

    personal_memory::update(c, "ACME Arms FFL; 555-0100; processes NFA Form 4 and Form 1.",
                            {"ffl","arms","nfa"}, "manual");
    auto c2 = personal_memory::get(c);
    if (!c2 || c2->tags.size() != 3) return testing::fail("update did not persist tags");

    auto md = personal_memory::export_markdown();
    if (md.find("Personal memory export") == std::string::npos)
        return testing::fail("export markdown missing header");
    if (md.find("Owns Keysight") == std::string::npos)
        return testing::fail("export markdown missing equipment entry");

    personal_memory::forget(a);
    if (personal_memory::get(a)) return testing::fail("forget did not delete");

    personal_memory::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "personal_memory",
    "023_personal_memory: user-scoped store with markdown export",
    &run);

}
