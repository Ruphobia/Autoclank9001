// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 035_export_import.

#include "test_runner.hpp"
#include "../modules/035_export_import/export_import.hpp"
#include "../modules/022_project_memory/project_memory.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_export";
    return fs::path("/tmp") / "tool_test_export";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    project_memory::open(root.string());

    auto a = project_memory::add("fact",
        "Dispenser stalls below 12 psi.", {"dispenser"}, "chat:turn-3");
    auto b = project_memory::add("decision",
        "Use SQLite for memory backends.", {"infra","db"}, "manual");
    auto c = project_memory::add("preference",
        "No em dashes.", {"style"}, "manual");

    fs::path jsonl  = root / "export.jsonl";
    fs::path bundle = root / "bundle";

    size_t n_jsonl  = export_import::export_jsonl(
        export_import::Scope::Project, root.string(), jsonl.string());
    size_t n_bundle = export_import::export_markdown_bundle(
        export_import::Scope::Project, root.string(), bundle.string());
    if (n_jsonl != 3 || n_bundle != 3) return testing::fail("export counts wrong");
    if (!fs::exists(jsonl) || fs::file_size(jsonl) == 0)
        return testing::fail("jsonl export empty");
    if (!fs::exists(bundle / "index.json"))
        return testing::fail("bundle missing index.json");
    if (!fs::exists(bundle / "entry-00000001.md"))
        return testing::fail("bundle missing per-entry md");

    // Wipe live store, re-import the JSONL, content survives.
    project_memory::close();
    fs::remove_all(root / ".tool", ec);
    project_memory::open(root.string());
    if (!project_memory::list().empty()) return testing::fail("scratch not clean");

    auto cnt = export_import::import_jsonl(
        export_import::Scope::Project, root.string(), jsonl.string());
    if (cnt.entries != 3 || cnt.skipped != 0)
        return testing::fail("jsonl import counts wrong");
    auto rehydrated = project_memory::list();
    if (rehydrated.size() != 3) return testing::fail("rehydrated count wrong");
    bool saw_dispenser = false, saw_sqlite = false, saw_emdash = false;
    for (const auto & e : rehydrated) {
        if (e.content.find("Dispenser stalls") != std::string::npos) saw_dispenser = true;
        if (e.content.find("SQLite") != std::string::npos)            saw_sqlite    = true;
        if (e.content.find("em dashes") != std::string::npos)         saw_emdash    = true;
        if (e.source.find("import:original-ts=") == std::string::npos)
            return testing::fail("rehydrated entry missing import marker");
    }
    if (!saw_dispenser || !saw_sqlite || !saw_emdash)
        return testing::fail("rehydrated content incomplete");

    // Re-import the markdown bundle on top: adds another three (additive,
    // by design).
    auto cnt2 = export_import::import_markdown_bundle(
        export_import::Scope::Project, root.string(), bundle.string());
    if (cnt2.entries != 3) return testing::fail("bundle import counts wrong");
    if (project_memory::list().size() != 6)
        return testing::fail("after bundle re-import count != 6");

    // Skipped: write a bad jsonl line, count.
    fs::path bad = root / "bad.jsonl";
    {
        std::ofstream out(bad);
        out << "this is not json\n";
        out << R"({"category":"fact","content":"valid"})" << "\n";
    }
    auto cnt3 = export_import::import_jsonl(
        export_import::Scope::Project, root.string(), bad.string());
    if (cnt3.entries != 1 || cnt3.skipped != 1)
        return testing::fail("bad-line counts wrong");

    (void)a; (void)b; (void)c;
    project_memory::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "export_import",
    "035_export_import: jsonl + markdown bundle round trip + skipped count",
    &run);

}
