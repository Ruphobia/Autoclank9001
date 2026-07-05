// Smoke test for 021_artifact_decisions: record three ADRs, supersede
// one, verify ordinals, status flip, and the bidirectional link.

#include "test_runner.hpp"
#include "../modules/021_artifact_decisions/decisions_folder.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_decisions";
    return fs::path("/tmp") / "tool_test_decisions";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    decisions_folder::init();

    int n1 = decisions_folder::record(
        "use sqlite for context", "Use SQLite for per-session context",
        "Need durable per-session log with full-text search.",
        "Pick SQLite + FTS5 over a custom file format.",
        "All session state migrates on schema bumps; SQLite is a dependency.",
        root.string());
    int n2 = decisions_folder::record(
        "use jaccard for council convergence",
        "Use Jaccard overlap as council convergence proxy",
        "Need a cheap dependency-free convergence metric for 017_council.",
        "Pick mean pairwise Jaccard over content-word bags.",
        "Replaceable later by a structured-claim metric.",
        root.string());
    int n3 = decisions_folder::record(
        "council uses claim sets",
        "Council convergence uses structured claim sets",
        "Jaccard token overlap is a noisy approximation.",
        "Switch to per-specialist claim sets; agreement = set intersection.",
        "Specialists must emit structured claims.",
        root.string(),
        decisions_folder::Status::Proposed);

    if (n1 != 1 || n2 != 2 || n3 != 3)
        return testing::fail("ordinals not 1,2,3 (got " + std::to_string(n1) +
                             "," + std::to_string(n2) + "," + std::to_string(n3) + ")");

    decisions_folder::supersede(n3, n2, root.string());

    auto a = decisions_folder::read(n2, root.string());
    auto b = decisions_folder::read(n3, root.string());
    if (!a || !b) return testing::fail("read after supersede returned nullopt");
    if (a->status != decisions_folder::Status::Superseded)
        return testing::fail("older not flipped to superseded");
    if (a->superseded_by.size() != 1 || a->superseded_by[0] != 3)
        return testing::fail("older missing superseded-by link");
    if (b->supersedes.size() != 1 || b->supersedes[0] != 2)
        return testing::fail("newer missing supersedes link");

    if (a->title.find("council convergence proxy") == std::string::npos)
        return testing::fail("title parse wrong");
    if (a->context.find("convergence metric") == std::string::npos)
        return testing::fail("context section missing");
    if (b->decision.find("claim sets") == std::string::npos)
        return testing::fail("decision section missing");

    auto all = decisions_folder::list(root.string());
    if (all != std::vector<int>{1,2,3}) return testing::fail("list ordering wrong");

    decisions_folder::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "artifact_decisions",
    "021_artifact_decisions: ADR record + supersede + status link",
    &run);

}
