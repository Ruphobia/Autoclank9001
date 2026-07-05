// Smoke test for 063_project_search.

#include "test_runner.hpp"
#include "../modules/063_project_search/project_search.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_psearch";
    return fs::path("/tmp") / "tool_test_psearch";
}

void write(const fs::path & p, const std::string & body) {
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary);
    o << body;
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    project_search::init();

    write(root / "src" / "main.cpp",
        "int main() {\n  // capybara\n  return 0;\n}\n");
    write(root / "src" / "util.cpp",
        "void util() { /* capybara helper */ }\n");
    write(root / "docs" / "notes.md",
        "capybara observations: pump stalls below 12 psi.\nNo retry on argon.\n");
    write(root / ".git" / "HEAD", "ref: refs/heads/main\n"); // ignored dir
    write(root / "build" / "junk.txt", "capybara should be ignored"); // ignored dir
    // a binary file with a NUL
    write(root / "data" / "blob.bin", std::string("hello\x00capybara", 13));

    project_search::Options opts;
    auto hits = project_search::search(root.string(), "capybara", opts);
    // Should match main.cpp:2, util.cpp:1, notes.md:1; NOT match .git/, build/, or blob.bin.
    if (hits.size() != 3)
        return testing::fail("expected 3 matches, got " + std::to_string(hits.size()));
    bool saw_main = false, saw_util = false, saw_notes = false;
    for (const auto & m : hits) {
        if (m.path.find("main.cpp")  != std::string::npos && m.line_number == 2) saw_main  = true;
        if (m.path.find("util.cpp")  != std::string::npos && m.line_number == 1) saw_util  = true;
        if (m.path.find("notes.md")  != std::string::npos && m.line_number == 1) saw_notes = true;
        if (m.path.find(".git")  != std::string::npos) return testing::fail(".git not ignored");
        if (m.path.find("build") != std::string::npos) return testing::fail("build not ignored");
        if (m.path.find("blob.bin") != std::string::npos) return testing::fail("binary file scanned");
    }
    if (!saw_main || !saw_util || !saw_notes)
        return testing::fail("expected hits missed");

    // case-insensitive search.
    opts.case_insensitive = true;
    auto ci = project_search::search(root.string(), "CAPYBARA", opts);
    if (ci.size() != 3)
        return testing::fail("case-insensitive search wrong");

    // extension filter
    opts.case_insensitive = false;
    opts.include_extensions = {"md"};
    auto only_md = project_search::search(root.string(), "capybara", opts);
    if (only_md.size() != 1 || only_md[0].path.find("notes.md") == std::string::npos)
        return testing::fail("extension filter wrong");

    // literal search escapes regex chars
    write(root / "doc.txt", "x = a.b+c (literal regex stuff)\n");
    project_search::Options bare;
    auto lit = project_search::search_literal(root.string(), "a.b+c", bare);
    bool saw_lit = false;
    for (const auto & m : lit) if (m.path.find("doc.txt") != std::string::npos) saw_lit = true;
    if (!saw_lit) return testing::fail("literal escape missed");

    project_search::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "project_search",
    "063_project_search: regex over a tree + ignore dirs + binary skip + literal escape",
    &run);

}
