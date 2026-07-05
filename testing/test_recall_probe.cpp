// Smoke test for 032_recall_probe.

#include "test_runner.hpp"
#include "../modules/032_recall_probe/recall_probe.hpp"
#include "../modules/022_project_memory/project_memory.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_recall_probe";
    return fs::path("/tmp") / "tool_test_recall_probe";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    project_memory::open(root.string());

    auto a = project_memory::add("fact",
        "Capybara is the largest rodent; native to South America.",
        {"animals"}, "manual");
    auto b = project_memory::add("fact",
        "The carrier gas in the chamber is argon.",
        {"chemistry"}, "manual");
    auto c = project_memory::add("fact",
        "Capybaras swim well; they prefer warm climates.",
        {"animals","behavior"}, "manual");

    auto hits = recall_probe::probe(recall_probe::Scope::Project,
        "capybara swim", 5);
    if (hits.empty()) return testing::fail("no probe hits");
    if (hits.front().entry_id != c)
        return testing::fail("expected capybara/swim entry first, got " +
                             std::to_string(hits.front().entry_id));
    bool saw_b = false;
    for (const auto & h : hits) if (h.entry_id == b) saw_b = true;
    if (saw_b) return testing::fail("argon entry leaked in");

    // gloss carries both numeric scores and at least one term match.
    if (hits.front().gloss.find("bm25=") == std::string::npos)
        return testing::fail("gloss missing bm25");
    if (hits.front().gloss.find("trigram=") == std::string::npos)
        return testing::fail("gloss missing trigram");
    if (hits.front().gloss.find("capybara") == std::string::npos &&
        hits.front().gloss.find("swim") == std::string::npos)
        return testing::fail("gloss missed term identification");

    // Per-hit fields populated.
    if (hits.front().snippet.find("[[") == std::string::npos)
        return testing::fail("snippet missing highlight markers");
    if (hits.front().tags.empty()) return testing::fail("tags not carried through");
    (void)a;

    project_memory::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "recall_probe",
    "032_recall_probe: hybrid bm25 + trigram with explanation gloss",
    &run);

}
