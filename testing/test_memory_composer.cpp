// Smoke test for 024_memory_composer: validate / submit into both
// scopes / edit round trip.

#include "test_runner.hpp"
#include "../modules/024_memory_composer/composer.hpp"
#include "../modules/022_project_memory/project_memory.hpp"
#include "../modules/023_personal_memory/personal_memory.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_composer";
    return fs::path("/tmp") / "tool_test_composer";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    project_memory::open(root.string());
    personal_memory::open(root.string());

    // Validation fires on empty content and bad confidence.
    memory_composer::Draft bad;
    if (memory_composer::validate(bad).empty())
        return testing::fail("empty content not flagged");
    memory_composer::Draft outc;
    outc.content    = "ok";
    outc.confidence = 1.5;
    auto bad_warns  = memory_composer::validate(outc);
    if (bad_warns.empty()) return testing::fail("bad confidence not flagged");

    memory_composer::Draft d;
    d.scope       = memory_composer::Scope::Project;
    d.kind        = memory_composer::EntryKind::Observation;
    d.content     = "The dispenser leaks if rinse pressure drops below 12 psi.";
    d.tags        = {"hardware","dispenser"};
    d.source_link = "chat:turn-3";
    d.confidence  = 0.8;

    auto sub = memory_composer::submit(d);
    if (sub.scope != memory_composer::Scope::Project || sub.id == 0)
        return testing::fail("project submit returned no id");

    auto e = project_memory::get(sub.id);
    if (!e) return testing::fail("submitted entry not found in project store");
    if (e->category != "observation") return testing::fail("category not mapped");
    if (e->source.find("confidence=0.80") == std::string::npos)
        return testing::fail("source missing confidence: '" + e->source + "'");
    if (e->source.find("link=chat:turn-3") == std::string::npos)
        return testing::fail("source missing link");

    // Personal scope.
    memory_composer::Draft p = d;
    p.scope   = memory_composer::Scope::Personal;
    p.kind    = memory_composer::EntryKind::Preference;
    p.content = "Prefer no em dashes.";
    auto ps   = memory_composer::submit(p);
    auto pe   = personal_memory::get(ps.id);
    if (!pe || pe->category != "preference")
        return testing::fail("personal submit/get round trip broken");

    // Edit.
    memory_composer::edit(memory_composer::Scope::Project, sub.id,
        "Dispenser leaks if rinse pressure drops below 10 psi (revised).",
        {"hardware","dispenser","revised"}, "chat:turn-7", 0.95);
    auto e2 = project_memory::get(sub.id);
    if (!e2 || e2->content.find("revised") == std::string::npos)
        return testing::fail("edit did not persist new content");
    if (e2->source.find("confidence=0.95") == std::string::npos)
        return testing::fail("edit did not persist new confidence");
    if (e2->tags.size() != 3)
        return testing::fail("edit lost tags");

    // Tag with a space should fail validation.
    memory_composer::Draft t;
    t.content = "x";
    t.tags    = {"bad tag"};
    auto tw = memory_composer::validate(t);
    if (tw.empty()) return testing::fail("space in tag not flagged");

    project_memory::close();
    personal_memory::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "memory_composer",
    "024_memory_composer: typed draft + scope routing into project / personal stores",
    &run);

}
