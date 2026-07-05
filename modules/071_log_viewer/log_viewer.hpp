#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

// Log viewer: parse structured log lines into a common Entry shape,
// then filter and facet. Two flavors covered: JSON-per-line (one
// JSON object per line) and logfmt (key=value space-separated). A
// pluggable "plain text" sniffer handles unstructured lines by
// stuffing the whole line into Entry::message.
namespace log_viewer {

enum class Level { Trace = 0, Debug = 1, Info = 2, Warn = 3, Error = 4, Fatal = 5, Unknown = 6 };
const char * to_string(Level);
Level         level_from_string(std::string_view);

struct Entry {
    std::string                        raw;
    int64_t                            ts_unix = 0;     // 0 when unknown
    Level                              level   = Level::Unknown;
    std::string                        message;
    std::map<std::string, std::string> fields;
};

struct Filter {
    Level                                level_min = Level::Trace;
    int64_t                              since_unix = 0;
    int64_t                              until_unix = 0;
    std::string                          contains;       // case-insensitive message substring
    std::map<std::string, std::string>   field_equals;   // every key/value must match
};

struct Burst {
    int64_t  start_unix = 0;
    int64_t  end_unix   = 0;
    size_t   count      = 0;
};

void init();
void shutdown();

// Sniffs each line: leading '{' -> JSON, "k=v" -> logfmt, else plain.
std::vector<Entry> parse(std::string_view text);

Entry parse_json_line  (std::string_view line);
Entry parse_logfmt_line(std::string_view line);

std::vector<Entry> apply_filter(const std::vector<Entry> & entries,
                                const Filter & f);

// Count distinct values for `field_key` ("level" / "host" / ...). Use
// the literal "_level" to facet the parsed Level enum.
std::map<std::string, size_t> facet_counts(const std::vector<Entry> & entries,
                                           std::string_view field_key);

// Detect bursts of >= `min_per_window` Error-or-higher entries within
// any contiguous `window_seconds` interval.
std::vector<Burst> error_bursts(const std::vector<Entry> & entries,
                                int64_t window_seconds = 60,
                                size_t  min_per_window = 5);

}
