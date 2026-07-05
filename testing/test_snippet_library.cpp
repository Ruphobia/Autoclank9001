// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 065_snippet_library.

#include "test_runner.hpp"
#include "../modules/065_snippet_library/snippet_library.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_snips";
    return fs::path("/tmp") / "tool_test_snips";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    snippet_library::open(root.string());

    snippet_library::Snippet s1;
    s1.name        = "git checkout new branch";
    s1.language    = "bash";
    s1.body        = "git checkout -b {{branch}} && git push -u origin {{branch}}";
    s1.description = "Create and push a new feature branch";
    s1.tags        = {"git","branch"};
    int64_t id1 = snippet_library::add(s1);

    snippet_library::Snippet s2;
    s2.name        = "list big files";
    s2.language    = "bash";
    s2.body        = "find {{dir}} -type f -size +{{min_mb}}M";
    s2.description = "Find files larger than N megabytes";
    s2.tags        = {"find","disk"};
    snippet_library::add(s2);

    snippet_library::Snippet s3;
    s3.name        = "sqlite count rows";
    s3.language    = "sql";
    s3.body        = "SELECT COUNT(*) FROM {{table}};";
    s3.description = "Quick row count";
    s3.tags        = {"sqlite"};
    snippet_library::add(s3);

    // Param extraction
    auto p = snippet_library::extract_params(
        "git checkout -b {{branch}} && git push -u origin {{branch}} -- {{remote}}");
    if (p.size() != 2 || p[0] != "branch" || p[1] != "remote")
        return testing::fail("param extraction wrong");

    auto loaded = snippet_library::get(id1);
    if (!loaded || loaded->params.size() != 1 || loaded->params[0] != "branch")
        return testing::fail("get did not populate params");

    // Render with substitution; missing keys leave the marker.
    auto rendered = snippet_library::render(s1.body,
        {{"branch", "feat-x"}});
    if (rendered != "git checkout -b feat-x && git push -u origin feat-x")
        return testing::fail("render substitution wrong: " + rendered);
    auto missing = snippet_library::render(s2.body, {{"dir","/tmp"}});
    if (missing.find("{{min_mb}}") == std::string::npos)
        return testing::fail("render did not leave missing marker");

    // Listings
    auto by_lang = snippet_library::list_by_language("bash");
    if (by_lang.size() != 2) return testing::fail("list_by_language count != 2");
    auto by_tag = snippet_library::list_by_tag("git");
    if (by_tag.size() != 1 || by_tag[0].id != id1)
        return testing::fail("list_by_tag wrong");

    // Search
    auto hits = snippet_library::search("checkout branch");
    if (hits.empty()) return testing::fail("FTS returned nothing");
    if (hits[0].snippet.id != id1)
        return testing::fail("FTS top hit wrong");

    snippet_library::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "snippet_library",
    "065_snippet_library: store + params + render + FTS",
    &run);

}
