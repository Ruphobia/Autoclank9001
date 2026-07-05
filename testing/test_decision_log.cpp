// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 027_decision_log: append-only record, amend, reverse,
// history view.

#include "test_runner.hpp"
#include "../modules/027_decision_log/decision_log.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_decision_log";
    return fs::path("/tmp") / "tool_test_decision_log";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    decision_log::init();

    int64_t d1 = decision_log::record(
        "Use SQLite + FTS5 for memory backends",
        "Need a search-capable store with low operational cost.",
        {"sqlite-fts5: shipped, in-process",
         "duckdb: heavier; column-store overkill",
         "raw markdown + grep: no FTS"},
        "sqlite-fts5",
        "All memory stores link sqlite3_amalgamation; reuse pattern from 005.",
        {/*linked_entries*/},
        {"commit:4455b3e"},
        root.string());
    int64_t d2 = decision_log::record(
        "Council convergence uses Jaccard token overlap",
        "Cheap dependency-free metric for 017_council.",
        {"jaccard tokens: simple; coarse",
         "embedding cosine: needs an embedder",
         "string equality: too brittle"},
        "jaccard tokens",
        "Replaceable later by a structured-claim metric.",
        {/*linked_entries*/},
        {"commit:4fd4dd6"},
        root.string());
    if (d1 != 1 || d2 != 2) return testing::fail("ids not 1,2");

    int64_t a1 = decision_log::amend(
        d1, {}, {"commit:b99937d","commit:29b707b"},
        "Confirmed by 015_doc_reader and 022_project_memory reuse.",
        root.string());
    if (a1 != 3) return testing::fail("amend id not 3");

    int64_t r1 = decision_log::reverse(
        d2, "Replaced by claim-set metric; see 028.", root.string());
    if (r1 != 4) return testing::fail("reverse id not 4");

    auto h1 = decision_log::history(d1, root.string());
    if (!h1) return testing::fail("history(d1) missing");
    if (h1->amendments.size() != 1) return testing::fail("d1 amendments missing");
    if (h1->reversals.size()  != 0) return testing::fail("d1 has spurious reversals");
    if (h1->amendments[0].linked_code.size() != 2)
        return testing::fail("amendment did not carry links");

    auto h2 = decision_log::history(d2, root.string());
    if (!h2 || h2->reversals.size() != 1)
        return testing::fail("d2 reversal missing");

    auto all_rec = decision_log::all(root.string());
    if (all_rec.size() != 4) return testing::fail("not 4 records");

    // The original record body must not have been edited.
    auto orig = decision_log::get(d1, root.string());
    if (!orig || orig->chosen != "sqlite-fts5")
        return testing::fail("original mutated");

    // File is append-only JSONL: every line parses, no rewrites.
    std::ifstream in(root / ".tool" / "decision-log" / "decisions.jsonl");
    std::string line;
    int n = 0;
    while (std::getline(in, line)) { if (!line.empty()) ++n; }
    if (n != 4) return testing::fail("jsonl line count wrong: " + std::to_string(n));

    decision_log::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "decision_log",
    "027_decision_log: append-only JSONL + amend + reverse + history",
    &run);

}
