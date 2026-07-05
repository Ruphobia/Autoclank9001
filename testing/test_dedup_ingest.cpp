// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 044_dedup_ingest.

#include "test_runner.hpp"
#include "../modules/044_dedup_ingest/dedup_ingest.hpp"

namespace {

testing::TestOutcome run() {
    dedup_ingest::init();

    // Canonicalization
    auto c1 = dedup_ingest::canonicalize_url(
        "HTTPS://Example.COM:443/foo//bar/?utm_source=x&z=1&a=2#frag");
    if (c1 != "https://example.com/foo/bar?a=2&z=1")
        return testing::fail("canonicalize mishap: " + c1);

    auto c2 = dedup_ingest::canonicalize_url("http://example.com:80/");
    if (c2 != "http://example.com/")
        return testing::fail("default port not dropped: " + c2);

    auto c3 = dedup_ingest::canonicalize_url(
        "https://example.com/path?fbclid=abc&utm_medium=cpc");
    if (c3 != "https://example.com/path")
        return testing::fail("tracking params not dropped: " + c3);

    // Two URLs differing only in tracking + ordering should canonicalize equal.
    auto e1 = dedup_ingest::canonicalize_url("https://EX.com/x?b=2&a=1");
    auto e2 = dedup_ingest::canonicalize_url("https://ex.com/x/?a=1&b=2&utm_source=q");
    if (e1 != e2)
        return testing::fail("canonical equality broken: '" + e1 + "' vs '" + e2 + "'");

    // SimHash sanity.
    uint64_t h_empty = dedup_ingest::simhash64("");
    if (h_empty != 0) return testing::fail("empty simhash != 0");
    uint64_t h_a = dedup_ingest::simhash64(
        "the quick brown fox jumps over the lazy dog twice for repetition");
    uint64_t h_a2 = dedup_ingest::simhash64(
        "the quick brown fox jumps over the lazy dog twice for repetition");
    if (h_a != h_a2) return testing::fail("simhash not deterministic");

    uint64_t h_b = dedup_ingest::simhash64(
        "the quick brown fox jumps over the lazy dog twice for repetition!");
    int d_close = dedup_ingest::hamming_distance(h_a, h_b);
    if (d_close > 3)
        return testing::fail("near-identical inputs too distant: " + std::to_string(d_close));

    uint64_t h_c = dedup_ingest::simhash64(
        "an entirely unrelated body about argon flow rates in chambers");
    int d_far = dedup_ingest::hamming_distance(h_a, h_c);
    if (d_far <= 8)
        return testing::fail("unrelated inputs too close: " + std::to_string(d_far));

    if (!dedup_ingest::is_near_duplicate(
            "the quick brown fox jumps over the lazy dog",
            "the quick brown fox jumps over the lazy dog.", 3))
        return testing::fail("trailing dot broke near-dup");
    if (dedup_ingest::is_near_duplicate(
            "carrier gas argon at 25 sccm",
            "the dispenser stalls at 12 psi rinse pressure", 3))
        return testing::fail("clearly different texts flagged as near-dup");

    dedup_ingest::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "dedup_ingest",
    "044_dedup_ingest: URL canonicalization + SimHash near-duplicate detection",
    &run);

}
