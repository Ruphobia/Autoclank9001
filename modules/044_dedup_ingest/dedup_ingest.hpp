#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Deduplicating Ingest: URL canonicalization plus SimHash-based near-
// duplicate detection. Used by the crawl pipeline to collapse trivial
// URL variants (utm tracking, default ports, fragment ids) and to
// notice when two pages with different URLs hold essentially the
// same content.
//
// SimHash uses 64-bit FNV-1a per token, weighted by token count; the
// final fingerprint is the sign-vector of the per-bit accumulator.
// hamming_distance over two SimHashes is the standard proximity
// measure; threshold around 3 bits is the textbook cutoff for "near
// duplicate".
namespace dedup_ingest {

void init();
void shutdown();

// Canonical form of `url`:
//   - lowercase the scheme and host
//   - drop the default port for the scheme (80 / 443)
//   - strip the fragment
//   - drop common tracking query params (utm_*, fbclid, gclid, mc_*, etc.)
//   - sort remaining query params alphabetically
//   - collapse "//" inside the path
//   - drop a trailing "/" from a non-empty path
// Returns the original string on parse failure.
std::string canonicalize_url(std::string_view url);

// 64-bit SimHash over whitespace-separated tokens in `text`.
uint64_t simhash64(std::string_view text);

int hamming_distance(uint64_t a, uint64_t b);

// True if the SimHashes of `a` and `b` are within `hamming_threshold`
// bits of each other.
bool is_near_duplicate(std::string_view a, std::string_view b,
                       int hamming_threshold = 3);

}
