// Smoke test for 039_inline_editor.

#include "test_runner.hpp"
#include "../modules/039_inline_editor/inline_editor.hpp"
#include "../modules/022_project_memory/project_memory.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_inline_editor";
    return fs::path("/tmp") / "tool_test_inline_editor";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    project_memory::open(root.string());
    inline_editor::init();

    int64_t e = project_memory::add("fact",
        "Line one\nLine two\nLine three", {"x"}, "manual");

    auto h0 = inline_editor::history(inline_editor::Scope::Project, e, root.string());
    if (!h0.empty()) return testing::fail("history non-empty for fresh entry");

    int64_t r1 = inline_editor::edit(inline_editor::Scope::Project, e,
        "Line one\nLine two changed\nLine three", {"x","edited"}, "manual",
        root.string());
    if (r1 != 1) return testing::fail("first revision != 1");

    int64_t r2 = inline_editor::edit(inline_editor::Scope::Project, e,
        "Line one\nLine two changed\nLine three\nLine four", {"x","edited"}, "manual",
        root.string());
    if (r2 != 2) return testing::fail("second revision != 2");

    auto h = inline_editor::history(inline_editor::Scope::Project, e, root.string());
    if (h.size() != 2) return testing::fail("history size != 2");
    if (h[0].content.find("Line two\n") == std::string::npos &&
        h[0].content != "Line one\nLine two\nLine three")
        return testing::fail("rev1 snapshot wrong");
    if (h[1].content.find("Line two changed") == std::string::npos)
        return testing::fail("rev2 snapshot wrong");

    // Diff between rev1 (original 3 lines) and current (4 lines with change).
    auto d = inline_editor::diff_revisions(inline_editor::Scope::Project, e,
        1, -1, root.string());
    bool saw_added = false, saw_removed = false;
    for (const auto & line : d) {
        if (line.kind == inline_editor::LineDiff::Kind::Added) {
            if (line.text == "Line two changed" || line.text == "Line four") saw_added = true;
        }
        if (line.kind == inline_editor::LineDiff::Kind::Removed) {
            if (line.text == "Line two") saw_removed = true;
        }
    }
    if (!saw_added || !saw_removed)
        return testing::fail("diff did not flag the change region");

    // Link validation.
    int64_t target = project_memory::add("fact", "target entry", {}, "manual");
    auto links = inline_editor::validate_links(
        std::string("See [[") + std::to_string(target) + "]] for context. Also [[99999]].",
        inline_editor::Scope::Project, root.string());
    if (links.size() != 2) return testing::fail("link count != 2");
    bool real_ok = false, fake_ok = false;
    for (const auto & l : links) {
        if (l.target_id == target) {
            if (!l.exists) return testing::fail("real link not resolved");
            real_ok = true;
        } else if (l.target_id == 99999) {
            if (l.exists) return testing::fail("fake link resolved");
            fake_ok = true;
        }
    }
    if (!real_ok || !fake_ok) return testing::fail("link checks incomplete");

    project_memory::close();
    inline_editor::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "inline_editor",
    "039_inline_editor: snapshot-on-edit history + line diff + [[N]] link validation",
    &run);

}
