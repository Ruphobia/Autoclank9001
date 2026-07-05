#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

// Memory Browser: faceted query layer over the project + personal
// memory stores. The underlying SQLite stores expose flat list / FTS
// search; this module adds the filter-by-multiple-facets +
// facet-count surface a browse UI needs (and the chat itself can use
// for "show me everything tagged X with confidence >= 0.8").
namespace memory_browser {

enum class Scope { Project = 0, Personal = 1, Both = 2 };

struct EntryRow {
    Scope                    scope;
    int64_t                  id;
    int64_t                  ts;
    std::string              category;
    std::string              content;
    std::vector<std::string> tags;
    std::string              source;
    double                   confidence = 1.0;  // parsed from source if present
};

struct Filter {
    Scope                    scope            = Scope::Both;
    std::vector<std::string> categories;        // any-match; empty = all
    std::vector<std::string> tags_all;          // every tag must be present
    std::vector<std::string> tags_any;          // at least one
    std::string              text_substring;    // case-insensitive content substring
    int64_t                  ts_min           = 0;       // 0 disables
    int64_t                  ts_max           = 0;       // 0 disables
    double                   confidence_min   = 0.0;
};

enum class Facet {
    Scope    = 0,
    Category = 1,
    Tag      = 2,
    Source   = 3,
    YearMonth = 4,    // YYYY-MM bucket from ts
};
const char * to_string(Facet);

void init();
void shutdown();

// Returns matching rows, newest first, capped at `limit`.
std::vector<EntryRow> query(const Filter & f,
                            std::string_view project_root,
                            size_t limit = 100);

// Faceted counts: for the given facet, what are the distinct values
// among rows that match `f`, and how many of each?
std::map<std::string, size_t> facet_counts(const Filter & f,
                                           Facet facet,
                                           std::string_view project_root);

}
