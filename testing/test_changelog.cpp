// Smoke test for 067_changelog.

#include "test_runner.hpp"
#include "../modules/067_changelog/changelog.hpp"

namespace {

testing::TestOutcome run() {
    changelog::init();

    using changelog::Commit;
    std::vector<Commit> commits = {
        {"aaa1111", "feat(api): add /quote endpoint",          ""},
        {"aaa2222", "fix: handle empty body in extractor",     ""},
        {"aaa3333", "docs: README touch-up",                   ""},
        {"aaa4444", "refactor!: drop legacy field",
                    "BREAKING CHANGE: removes the deprecated `legacy_id` column."},
        {"aaa5555", "merged PR #99 from contributor",          ""},   // unrecognized prefix
        {"aaa6666", "chore: bump deps",                        ""},
    };

    if (changelog::detect_type("feat(api): foo") != "feat")
        return testing::fail("detect_type wrong");
    if (changelog::detect_type("fix!: bar") != "fix")
        return testing::fail("detect_type '!' wrong");
    if (!changelog::detect_type("merged PR #99").empty())
        return testing::fail("no-prefix not detected as empty");

    auto groups = changelog::group_by_type(commits);
    bool saw_feat = false, saw_fix = false, saw_other = false, saw_refactor = false;
    for (const auto & g : groups) {
        if (g.ccm_type == "feat"     && g.commits.size() == 1) saw_feat     = true;
        if (g.ccm_type == "fix"      && g.commits.size() == 1) saw_fix      = true;
        if (g.ccm_type == "refactor" && g.commits.size() == 1) saw_refactor = true;
        if (g.ccm_type.empty()       && g.commits.size() == 1) saw_other    = true;
    }
    if (!saw_feat || !saw_fix || !saw_refactor || !saw_other)
        return testing::fail("group_by_type missed expected groups");

    auto bump = changelog::suggest_bump(commits);
    if (bump != changelog::SemverBump::Major)
        return testing::fail("BREAKING CHANGE should produce major bump");

    // Without the breaking refactor: still has feat -> minor.
    std::vector<Commit> minor = {commits[0], commits[1], commits[5]};
    if (changelog::suggest_bump(minor) != changelog::SemverBump::Minor)
        return testing::fail("feat without breaking should produce minor");

    // Only fix -> patch.
    std::vector<Commit> patch = {commits[1], commits[5]};
    if (changelog::suggest_bump(patch) != changelog::SemverBump::Patch)
        return testing::fail("fix+chore should produce patch");

    auto md = changelog::render_markdown(commits, "## v0.1.0 (test)");
    if (md.find("## v0.1.0") == std::string::npos)
        return testing::fail("changelog title missing");
    if (md.find("### Features") == std::string::npos ||
        md.find("### Bug fixes") == std::string::npos ||
        md.find("### Refactors") == std::string::npos ||
        md.find("### Other")    == std::string::npos)
        return testing::fail("changelog section headers missing");
    if (md.find("add /quote endpoint") == std::string::npos)
        return testing::fail("feat commit text missing");

    changelog::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "changelog",
    "067_changelog: conventional-commit grouping + semver suggest + render",
    &run);

}
