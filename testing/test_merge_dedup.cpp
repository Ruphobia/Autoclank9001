// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 026_merge_dedup: trigram Jaccard, find_duplicates,
// propose_merge / apply_merge round trip with soft-forget recovery.

#include "test_runner.hpp"
#include "../modules/026_merge_dedup/merge_dedup.hpp"
#include "../modules/022_project_memory/project_memory.hpp"
#include "../modules/025_forget_redact/forget_redact.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_merge_dedup";
    return fs::path("/tmp") / "tool_test_merge_dedup";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    project_memory::open(root.string());

    // Trigram sanity.
    if (merge_dedup::trigram_jaccard("abcdef", "abcdef") < 0.99)
        return testing::fail("identical trigram jaccard not ~1");
    if (merge_dedup::trigram_jaccard("alpha", "zebra") > 0.2)
        return testing::fail("disjoint trigram jaccard too high");

    auto a = project_memory::add("fact",
        "The dispenser pump stalls below 12 psi rinse pressure.",
        {"dispenser","hardware"}, "manual");
    auto b = project_memory::add("fact",
        "Dispenser pump stalls below 12 psi rinse pressure observed twice.",
        {"dispenser","observed"}, "manual");
    auto c = project_memory::add("fact",
        "Carrier gas flow set to 25 sccm for the chamber.",
        {"gas","setup"}, "manual");

    auto groups = merge_dedup::find_duplicates(
        merge_dedup::Scope::Project, root.string(), 0.5, "fact");
    if (groups.size() != 1)
        return testing::fail("expected 1 duplicate group, got " + std::to_string(groups.size()));
    if (groups[0].ids.size() != 2) return testing::fail("group should hold 2 ids");
    bool has_a = false, has_b = false;
    for (auto id : groups[0].ids) { if (id == a) has_a = true; if (id == b) has_b = true; }
    if (!has_a || !has_b) return testing::fail("dup group missed expected ids");
    if (groups[0].similarity < 0.5) return testing::fail("similarity below threshold");

    // Standalone C should not surface.
    bool saw_c = false;
    for (const auto & g : groups) for (auto id : g.ids) if (id == c) saw_c = true;
    if (saw_c) return testing::fail("non-duplicate flagged");

    auto draft = merge_dedup::propose_merge(
        merge_dedup::Scope::Project, {a, b}, root.string());
    if (draft.content.find("Dispenser pump stalls") == std::string::npos &&
        draft.content.find("dispenser pump stalls") == std::string::npos)
        return testing::fail("merged content missing canonical sentence");
    bool has_observed_tag = false, has_hardware_tag = false;
    for (const auto & t : draft.tags) {
        if (t == "observed") has_observed_tag = true;
        if (t == "hardware") has_hardware_tag = true;
    }
    if (!has_observed_tag || !has_hardware_tag)
        return testing::fail("merged tags missed union");

    int64_t merged_id = merge_dedup::apply_merge(
        merge_dedup::Scope::Project, draft, root.string());
    if (merged_id == 0) return testing::fail("apply_merge returned 0");

    if (project_memory::get(a)) return testing::fail("a not soft-forgotten");
    if (project_memory::get(b)) return testing::fail("b not soft-forgotten");
    auto merged = project_memory::get(merged_id);
    if (!merged) return testing::fail("merged entry not in live store");
    if (merged->source.find("merge-dedup") == std::string::npos)
        return testing::fail("merged entry missing source marker");

    auto ts = forget_redact::list_tombstones(
        forget_redact::Scope::Project, root.string());
    if (ts.size() != 2) return testing::fail("expected 2 tombstones from merge");

    project_memory::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "merge_dedup",
    "026_merge_dedup: trigram Jaccard, find_duplicates, propose/apply merge",
    &run);

}
