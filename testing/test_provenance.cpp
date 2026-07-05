// Smoke test for 030_provenance.

#include "test_runner.hpp"
#include "../modules/030_provenance/provenance.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_provenance";
    return fs::path("/tmp") / "tool_test_provenance";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    provenance::init();

    // hash sanity.
    auto h_empty = provenance::hash_fnv64("");
    auto h_foo   = provenance::hash_fnv64("foo");
    auto h_foo2  = provenance::hash_fnv64("foo");
    if (h_empty == h_foo) return testing::fail("empty and 'foo' hash to same value");
    if (h_foo != h_foo2)  return testing::fail("hash not deterministic");
    if (h_foo.rfind("fnv64:", 0) != 0) return testing::fail("hash missing fnv64 prefix");

    int64_t p1 = provenance::track(provenance::Scope::Project, 42,
        provenance::Kind::Url, "https://example.com/datasheet.pdf",
        "datasheet body bytes", root.string());
    int64_t p2 = provenance::track(provenance::Scope::Project, 42,
        provenance::Kind::Chat, "session:abc:turn:7",
        "user observation about the dispenser", root.string());
    int64_t p3 = provenance::track(provenance::Scope::Project, 99,
        provenance::Kind::Manual, "ui",
        "manual entry", root.string());
    if (p1 != 1 || p2 != 2 || p3 != 3)
        return testing::fail("ids not sequential");

    auto for42 = provenance::for_entry(provenance::Scope::Project, 42, root.string());
    if (for42.size() != 2) return testing::fail("for_entry count wrong");

    auto r1 = provenance::get(provenance::Scope::Project, p1, root.string());
    if (!r1 || r1->kind != provenance::Kind::Url || r1->ref.find("example.com") == std::string::npos)
        return testing::fail("get returned wrong record");

    // verify: matching content matches, drifted content does not.
    if (!provenance::verify(*r1, "datasheet body bytes"))
        return testing::fail("verify rejected unchanged content");
    if (provenance::verify(*r1, "datasheet body bytes!!"))
        return testing::fail("verify accepted drifted content");

    if (provenance::all(provenance::Scope::Project, root.string()).size() != 3)
        return testing::fail("all() count wrong");

    provenance::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "provenance",
    "030_provenance: JSONL provenance log + FNV64 hash + drift detection",
    &run);

}
