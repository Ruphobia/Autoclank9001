#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

// Export and Replay: write the harvested crawl corpus to a portable
// archive (WARC for byte-accurate replay, JSONL for tooling that
// wants to grep), and read it back to drive the extractor without
// re-fetching. Minimal WARC subset: warcinfo + response records,
// CRLF-terminated, length-prefixed bodies, matching ISO 28500 enough
// for tools like warcio / wget --warc-input to consume.
namespace export_replay {

struct Record {
    std::string warc_type;       // "warcinfo", "response", "resource", ...
    std::string date_iso;        // YYYY-MM-DDThh:mm:ssZ
    std::string record_id;       // <urn:uuid:...> or any opaque id
    std::string target_uri;
    std::string content_type;    // e.g. "text/html"
    std::string body;            // for "response": raw HTTP+body; for "resource": body only
};

void init();
void shutdown();

// Write `records` to a WARC file. Returns count written.
size_t write_warc(const std::vector<Record> & records,
                  std::string_view path);

// Read every record from a WARC file in order.
std::vector<Record> read_warc(std::string_view path);

// Replay: stream each record through `callback` without holding the
// whole archive in memory.
size_t replay_warc(std::string_view path,
                   const std::function<void(const Record &)> & callback);

// JSONL flavour for tooling that prefers grepability over byte
// accuracy. Each line is {"warc_type":..., "date":..., "uri":...,
// "content_type":..., "body": "..."} with `body` base64-encoded
// (since JSONL is line-delimited).
size_t write_jsonl(const std::vector<Record> & records,
                   std::string_view path);
std::vector<Record> read_jsonl(std::string_view path);

}
