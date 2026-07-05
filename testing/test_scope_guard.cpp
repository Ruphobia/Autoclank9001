// Smoke test for 031_scope_guard.

#include "test_runner.hpp"
#include "../modules/031_scope_guard/scope_guard.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_scope_guard";
    return fs::path("/tmp") / "tool_test_scope_guard";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    scope_guard::init();

    // Defaults: AssistantVisible
    auto def = scope_guard::get_label(scope_guard::Scope::Project, 99, root.string());
    if (def != scope_guard::Label::AssistantVisible)
        return testing::fail("default label not assistant-visible");

    using L = scope_guard::Label;
    using D = scope_guard::Destination;
    scope_guard::set_label(scope_guard::Scope::Project, 1, L::LocalOnly,        root.string());
    scope_guard::set_label(scope_guard::Scope::Project, 2, L::ProjectShared,    root.string());
    scope_guard::set_label(scope_guard::Scope::Project, 3, L::AssistantVisible, root.string());
    scope_guard::set_label(scope_guard::Scope::Project, 4, L::Redacted,         root.string());

    // Round trip persistence.
    if (scope_guard::get_label(scope_guard::Scope::Project, 2, root.string()) != L::ProjectShared)
        return testing::fail("label did not persist");

    // ModelContext: 1,2,3 allowed; 4 blocked.
    auto v_mc = scope_guard::preflight(scope_guard::Scope::Project,
        {1,2,3,4}, D::ModelContext, root.string());
    if (v_mc.allowed.size() != 3 || v_mc.blocked.size() != 1)
        return testing::fail("ModelContext preflight wrong: " +
                             std::to_string(v_mc.allowed.size()) + "/" + std::to_string(v_mc.blocked.size()));
    if (v_mc.blocked.front().id != 4) return testing::fail("ModelContext didn't block redacted");

    // Sync: only ProjectShared (id 2) allowed.
    auto v_sync = scope_guard::preflight(scope_guard::Scope::Project,
        {1,2,3,4}, D::Sync, root.string());
    if (v_sync.allowed.size() != 1 || v_sync.allowed.front().id != 2)
        return testing::fail("Sync preflight wrong");

    // Share: nothing allowed by default.
    auto v_share = scope_guard::preflight(scope_guard::Scope::Project,
        {1,2,3,4}, D::Share, root.string());
    if (!v_share.allowed.empty()) return testing::fail("Share preflight let something through");
    for (const auto & b : v_share.blocked) if (b.reason.empty())
        return testing::fail("Share block reason missing");

    // Display: only Redacted blocks? Per rules: Display allows everything.
    auto v_disp = scope_guard::preflight(scope_guard::Scope::Project,
        {1,2,3,4}, D::Display, root.string());
    if (v_disp.allowed.size() != 3 || v_disp.blocked.size() != 1)
        return testing::fail("Display preflight wrong");

    scope_guard::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "scope_guard",
    "031_scope_guard: per-entry labels + preflight against destinations",
    &run);

}
