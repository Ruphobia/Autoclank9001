#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

// Reference Library: local-first reference database. The canonical
// record shape is CSL-JSON (https://github.com/citation-style-language/schema)
// stored verbatim, plus a denormalized index for fast filtering and
// FTS5 over title + abstract + author names. Lives at
// <project>/.tool/refs/refs.sqlite alongside attached PDFs under
// <project>/.tool/refs/attachments/.
namespace reference_library {

struct Item {
    int64_t                  id           = 0;
    std::string              citation_key;     // user-facing key (BibTeX-style)
    std::string              type;             // CSL "type" field
    std::string              title;
    std::string              year;             // string so "2026" / "in press" both fit
    std::vector<std::string> authors;          // "Family, Given" strings
    std::string              container_title;  // journal / book / conference
    std::vector<std::string> tags;
    nlohmann::json           csl;              // full CSL-JSON record
};

struct Hit {
    Item        item;
    double      score   = 0.0;
    std::string snippet;
};

void open(std::string_view project_root);
void close();

// Insert a CSL-JSON record. Returns the new id. `citation_key` is
// taken as-is; collision detection / disambiguation is the Citation
// Key Generator's job.
int64_t add(std::string_view citation_key,
            const nlohmann::json & csl,
            const std::vector<std::string> & tags = {});

void                    update(int64_t id, const nlohmann::json & csl,
                               const std::vector<std::string> & tags);
void                    forget(int64_t id);
std::optional<Item>     get(int64_t id);
std::optional<Item>     by_citation_key(std::string_view key);

// Filtered listings; empty filter = all, newest first.
std::vector<Item> list(std::string_view type = {},
                       std::string_view tag  = {},
                       size_t           limit = 100);

// FTS over title + abstract + author names.
std::vector<Hit> search(std::string_view query, size_t top_k = 8);

}
