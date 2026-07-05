// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 048_kb_store.

#include "test_runner.hpp"
#include "../modules/048_kb_store/kb_store.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_kb_store";
    return fs::path("/tmp") / "tool_test_kb_store";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    kb_store::open(root.string());

    int64_t id1 = kb_store::ingest(
        "https://example.com/p1", "example.com", 1700000000,
        "text/html", "raw body of page 1",
        "extracted text discussing capybara behavior",
        {"animal","wiki"});
    int64_t id2 = kb_store::ingest(
        "https://example.com/p2", "example.com", 1700000100,
        "text/html", "raw body of page 2",
        "extracted text about argon flow in chambers",
        {"chemistry"});
    int64_t id3 = kb_store::ingest(
        "https://other.com/q", "other.com", 1700000200,
        "text/html", "raw body of q",
        "extracted text about capybara swimming",
        {"animal"});

    if (id1 == 0 || id2 == 0 || id3 == 0) return testing::fail("ingest returned 0");

    // Content-address dedup: re-ingesting identical body returns the same id.
    int64_t id1_again = kb_store::ingest(
        "https://example.com/p1", "example.com", 1700000300,
        "text/html", "raw body of page 1", "ignored extracted",
        {"animal"});
    if (id1_again != id1) return testing::fail("content-address dedup broken");

    // by_host
    auto eh = kb_store::list_by_host("example.com");
    if (eh.size() != 2) return testing::fail("by_host count wrong");

    // by_time
    auto et = kb_store::list_by_time(1700000050, 1700000250);
    if (et.size() != 2) return testing::fail("by_time count wrong");

    // search FTS
    auto hits = kb_store::search("capybara", 5);
    if (hits.size() != 2) return testing::fail("FTS capybara hit count != 2");
    bool ok = false;
    for (const auto & h : hits) if (h.item.id == id1 || h.item.id == id3) ok = true;
    if (!ok) return testing::fail("FTS did not surface expected items");
    if (hits.front().snippet.find("[[") == std::string::npos)
        return testing::fail("snippet missing highlight markers");

    // raw round trip via content hash.
    auto blob = kb_store::raw(hits.front().item.content_hash);
    if (!blob || blob->empty()) return testing::fail("raw blob missing");

    kb_store::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "kb_store",
    "048_kb_store: content-addressed ingest + dedup + by-host / by-time / FTS",
    &run);

}
