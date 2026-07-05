// Smoke test for 025_forget_redact: soft_forget + restore round trip,
// redact substitutes in place, audit log accumulates lines.

#include "test_runner.hpp"
#include "../modules/025_forget_redact/forget_redact.hpp"
#include "../modules/022_project_memory/project_memory.hpp"
#include "../modules/023_personal_memory/personal_memory.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_forget_redact";
    return fs::path("/tmp") / "tool_test_forget_redact";
}

size_t line_count(const fs::path & p) {
    if (!fs::exists(p)) return 0;
    std::ifstream in(p);
    std::string l; size_t n = 0;
    while (std::getline(in, l)) if (!l.empty()) ++n;
    return n;
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    project_memory::open(root.string());

    auto id = project_memory::add("observation",
        "User john.doe@example.com submitted a bug report.",
        {"email","support"}, "manual");

    // Redact replaces email in place.
    size_t matches = forget_redact::redact(
        forget_redact::Scope::Project, id,
        R"([\w.+-]+@[\w-]+\.[\w.-]+)",
        root.string());
    if (matches != 1) return testing::fail("expected 1 email match, got " + std::to_string(matches));

    auto e = project_memory::get(id);
    if (!e || e->content.find("john.doe") != std::string::npos)
        return testing::fail("redact did not remove the email");
    if (e->content.find("[REDACTED]") == std::string::npos)
        return testing::fail("redact did not insert placeholder");

    // soft_forget then restore.
    forget_redact::soft_forget(forget_redact::Scope::Project, id,
        "GDPR delete request", root.string());
    if (project_memory::get(id))
        return testing::fail("soft_forget did not remove from live store");
    auto ts = forget_redact::list_tombstones(forget_redact::Scope::Project, root.string());
    if (ts.size() != 1) return testing::fail("tombstone not created");
    if (ts[0].original_id != id) return testing::fail("tombstone wrong original id");
    if (ts[0].reason != "GDPR delete request")
        return testing::fail("tombstone reason not saved");

    int64_t new_id = forget_redact::restore(forget_redact::Scope::Project,
                                            ts[0].tombstone_id, root.string());
    if (new_id == 0) return testing::fail("restore returned 0");
    auto restored = project_memory::get(new_id);
    if (!restored || restored->content.find("[REDACTED]") == std::string::npos)
        return testing::fail("restored content lost");

    auto ts_after = forget_redact::list_tombstones(forget_redact::Scope::Project, root.string());
    if (!ts_after.empty()) return testing::fail("tombstone not consumed by restore");

    // hard_delete is idempotent.
    forget_redact::hard_delete(forget_redact::Scope::Project, new_id, root.string());
    forget_redact::hard_delete(forget_redact::Scope::Project, new_id, root.string());
    if (project_memory::get(new_id)) return testing::fail("hard_delete failed");

    size_t lines = line_count(root / ".tool" / "memory" / "audit.jsonl");
    if (lines < 4) return testing::fail("audit log short: " + std::to_string(lines));

    project_memory::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "forget_redact",
    "025_forget_redact: redact/soft_forget/restore/hard_delete + audit log",
    &run);

}
