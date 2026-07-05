#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "../051_api_harvester/api_harvester.hpp"

// Wayback Sweeper: snapshot listing and bulk-fetch against the
// Internet Archive CDX API. Reuses 051_api_harvester::HttpClient so
// the HTTP plumbing is injectable; tests use an in-memory client.
namespace wayback {

struct Snapshot {
    std::string timestamp;     // YYYYMMDDhhmmss
    std::string original_url;
    std::string mime;
    std::string status_code;
    std::string length;
    std::string digest;
};

void init();
void shutdown();

// List captures for `url_or_host`. `match_type` is "exact" (default),
// "prefix", "host", or "domain". `limit` caps the number of CDX rows
// requested.
std::vector<Snapshot> snapshots(std::string_view url_or_host,
                                const api_harvester::HttpClient & client,
                                std::string_view match_type = "exact",
                                int               limit     = 100);

// Build the per-snapshot replay URL.
std::string snapshot_url(const Snapshot & s);

// Fetch the snapshot via the injected client. Returns the response
// body or std::nullopt on HTTP failure.
std::optional<std::string> fetch_snapshot(const Snapshot & s,
                                          const api_harvester::HttpClient & client);

// Build the CDX JSON-output query URL the snapshots() call would
// issue. Exposed for callers that already know how to drive the CDX
// API and want to swap our pagination loop for their own.
std::string cdx_query_url(std::string_view url_or_host,
                          std::string_view match_type,
                          int limit);

}
